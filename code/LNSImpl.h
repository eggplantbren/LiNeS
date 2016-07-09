#include <fstream>

namespace LiNeS
{

template<class ModelType>
LNS<ModelType>::LNS(unsigned int run_id, const char* levels_file,
                                         DNest4::RNG rng)
:run_id(run_id)
,rng(rng)
,iteration(0)
,logX(0.0)
,active(true)
{
    // Open the levels file
    std::fstream fin(levels_file, std::ios::in);

    // Skip comments at the top of file
    while(fin.peek() == '#')
        fin.ignore(1000000, '\n');

    // Skip the zero level
    fin.ignore(1000000, '\n');
    double temp1, temp2, temp3;
    while(fin>>temp1 && fin>>temp2 && fin>>temp3)
    {
        levels_log_likelihoods.push_back(temp2);
        levels_tiebreakers.push_back(temp3);
        fin.ignore(1000000, '\n');
    }
    fin.close();

    std::cout<<std::setprecision(8);

    std::fstream fout;
    fout.open("levels_logL.txt", std::ios::out);
    fout<<std::setprecision(12);
    for(auto logl: levels_log_likelihoods)
        fout<<logl<<' ';
    fout.close();
}

template<class ModelType>
LNS<ModelType>::LNS(unsigned int run_id, const ClassicLogger& classic_logger,
                    DNest4::RNG rng)
:run_id(run_id)
,rng(rng)
,iteration(0)
,logX(0.0)
,active(true)
{
    size_t num_particles = classic_logger.get_num_particles();
    std::vector<double> logl = classic_logger.get_log_likelihoods();
    std::vector<double> tb = classic_logger.get_tiebreakers();

    // Extract every nth to become a level
    for(size_t i=1; i<=(logl.size()/num_particles); ++i)
    {
        levels_log_likelihoods.push_back(logl[num_particles*i-1]);
        levels_tiebreakers.push_back(tb[num_particles*i-1]);
    }
    std::cout<<std::setprecision(8);

    std::fstream fout;
    fout.open("levels_logL.txt", std::ios::out);
    fout<<std::setprecision(12);
    for(auto logl: levels_log_likelihoods)
        fout<<logl<<' ';
    fout.close();
}

template<class ModelType>
void LNS<ModelType>::run(unsigned int mcmc_steps, unsigned int thin)
{
    for(size_t i=0; i<(levels_log_likelihoods.size()+1); ++i)
        do_iteration(mcmc_steps, thin);
}

template<class ModelType>
void LNS<ModelType>::do_iteration(unsigned int mcmc_steps, unsigned int thin)
{
    // Allocate space
    if(stash.size() != mcmc_steps/thin+1)
    {
        stash.resize(mcmc_steps/thin+1);
        logl_stash.resize(mcmc_steps/thin+1);
        tb_stash.resize(mcmc_steps/thin+1);
    }

    // Set the likelihood threshold
    double logl_threshold = (iteration == 0)?
                                (-std::numeric_limits<double>::max()):
                                (levels_log_likelihoods[iteration-1]);
    double tb_threshold = (iteration == 0)?
                                (0.0):
                                (levels_tiebreakers[iteration-1]);

    // Count number of particles in the stash that are above the threshold    
    size_t count_above = 0;
    std::vector<bool> above(stash.size(), false);
    if(iteration > 0)
    {
        for(size_t i=0; i<stash.size(); ++i)
        {
            if(logl_stash[i] > logl_threshold ||
                    (logl_stash[i] == logl_threshold &&
                     tb_stash[i] > tb_threshold))
            {
                above[i] = true;
                ++count_above;
            }
        }
        size_t count_below = stash.size() - count_above;

        for(size_t i=0; i<stash.size(); ++i)
        {
            if(active && !above[i] && rng.rand() <= 1.0/count_below)
            {
                logger.log_particle_info(run_id, iteration-1, logl_stash[i]);
                logger.save_particle(stash[i]);
            }
        }

        if(count_above == 0)
        {
            logX = -std::numeric_limits<double>::max();
            active = false;
        }
        else
            logX += log(count_above) - log(stash.size());

        logger.log_level(logX, logl_threshold);
    }

    std::cout<<"# Linked NS run "<<run_id<<", iteration "<<iteration<<". ";
    std::cout<<"(log(X), log(L)) = ("<<logX<<", "<<logl_threshold<<").\n";
    if(!active)
    {
        std::cout<<"#    Skipping MCMC.\n#"<<std::endl;
        ++iteration;
        return;
    }

    std::cout<<"#    Doing MCMC..."<<std::flush;

    unsigned int K;
    if(iteration == 0)
    {
        K = rng.rand_int(stash.size());
        stash[K].from_prior(rng);
        logl_stash[K] = stash[K].log_likelihood();
        tb_stash[K] = rng.rand();
    }
    else
    {
        unsigned int attempts = 0;
        while(true)
        {
            K = rng.rand_int(stash.size());
            ++attempts;
            if(above[K])
                break;
        }
    }

    ModelType particle = stash[K];
    double logl_particle = logl_stash[K];
    double tb_particle = tb_stash[K];

    ModelType proposal;
    double logl_proposal, tb_proposal, logH;

    unsigned int tries = 0;
    unsigned int accepts = 0;

    // Forwards
    for(unsigned int i=K+1; i<stash.size(); ++i)
    {
        for(unsigned int j=0; j<thin; ++j)
        {
            ++tries;

            // Do the proposal
            proposal = particle;
            logH = proposal.perturb(rng);
            logl_proposal = proposal.log_likelihood();
            tb_proposal = tb_particle + rng.randh();
            DNest4::wrap(tb_proposal, 0.0, 1.0);

            if(rng.rand() <= exp(logH))
            {
                if(logl_proposal > logl_threshold ||
                    (logl_proposal == logl_threshold &&
                     tb_proposal   == tb_threshold))
                {
                    particle = proposal;
                    logl_particle = logl_proposal;
                    tb_particle = tb_proposal;
                    ++accepts;
                }
            }
        }

        stash[i] = particle;
        logl_stash[i] = logl_particle;
        tb_stash[i] = tb_particle;
    }

    // Backwards
    particle = stash[K];
    logl_particle = logl_stash[K];
    tb_particle = tb_stash[K];

    for(int i=K-1; i>=0; --i)
    {
        for(unsigned int j=0; j<thin; ++j)
        {
            ++tries;

            // Do the proposal
            proposal = particle;
            logH = proposal.perturb(rng);
            logl_proposal = proposal.log_likelihood();
            tb_proposal = tb_particle + rng.randh();
            DNest4::wrap(tb_proposal, 0.0, 1.0);

            if(rng.rand() <= exp(logH))
            {
                if(logl_proposal > logl_threshold ||
                    (logl_proposal == logl_threshold &&
                     tb_proposal   == tb_threshold))
                {
                    particle = proposal;
                    logl_particle = logl_proposal;
                    tb_particle = tb_proposal;
                    ++accepts;
                }
            }
        }

        stash[i] = particle;
        logl_stash[i] = logl_particle;
        tb_stash[i] = tb_particle;
    }

    std::cout<<"done. Accepted "<<accepts<<'/'<<tries<<".\n#"<<std::endl;

    ++iteration;
}

} // namespace LiNeS


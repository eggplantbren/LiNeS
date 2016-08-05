#include <fstream>

namespace LiNeS
{

template<class ModelType>
LNS<ModelType>::LNS(unsigned int run_id, const ClassicLogger& classic_logger,
                                DNest4::RNG rng)
:run_id(run_id)
,rng(rng)
,iteration(0)
,logX(0.0)
,active(true)
,mcmc_steps_taken(0)
{
    size_t num_particles = classic_logger.get_num_particles();

    // Get stuff from the logger
    auto  s = classic_logger.get_scalars();
    auto tb = classic_logger.get_tiebreakers();

    // Extract every nth to become a level
    for(size_t i=1; i<=(s.size()/num_particles); ++i)
    {
        levels_scalars.push_back(s[num_particles*i-1]);
        levels_tiebreakers.push_back(tb[num_particles*i-1]);
    }
    std::cout<<std::setprecision(8);

    std::fstream fout;
    fout.open("levels_scalars.txt", std::ios::out);
    fout<<std::setprecision(12);
    for(size_t i=0; i<levels_scalars.size(); ++i)
    {
        for(size_t j=0; j<levels_scalars[i].size(); ++j)
            fout<<levels_scalars[i][j]<<' ';
    }
    fout.close();
}

template<class ModelType>
void LNS<ModelType>::run(unsigned int mcmc_steps, unsigned int thin)
{
    for(size_t i=0; i<(levels_scalars.size()+1); ++i)
        do_iteration(mcmc_steps, thin);
}

template<class ModelType>
void LNS<ModelType>::do_iteration(unsigned int mcmc_steps, unsigned int thin)
{
    // Allocate space
    if(stashed_particles.size() != mcmc_steps/thin+1)
    {
        stashed_particles.resize(mcmc_steps/thin+1);
        stashed_scalars.resize(mcmc_steps/thin+1);
        stashed_tiebreakers.resize(mcmc_steps/thin+1);
    }

    // Set the threshold
    std::vector< std::vector<double> > s_threshold, tb_threshold;
    if(iteration == 0)
    {
        s_threshold = std::vector<double>(ModelType::get_num_scalars(), -1E300);
        tb_threshold = std::vector<double>(ModelType::get_num_scalars(), 0.0);
    }
    else
    {
        s_threshold = levels_scalars[iteration-1];
        tb_threshold = levels_tiebreakers[iteration-1];
    }

    // Count number of particles in the stash that are above the threshold    
    size_t count_above = 0;
    std::vector<bool> above(stashed_particles.size(), false);
    if(iteration > 0)
    {
        for(size_t i=0; i<stashed_particles.size(); ++i)
        {
//            if(logl_stash[i] > logl_threshold ||
//                    (logl_stash[i] == logl_threshold &&
//                     tb_stash[i] > tb_threshold))
//            {
//                above[i] = true;
//                ++count_above;
//            }
        }
//        size_t count_below = stash.size() - count_above;

//        for(size_t i=0; i<stash.size(); ++i)
//        {
//            if(active && !above[i] && rng.rand() <= 1.0/count_below)
//            {
//                logger.log_particle_info(run_id, iteration-1, logl_stash[i]);
//                logger.save_particle(stash[i]);
//            }
//        }

//        if(count_above == 0)
//        {
//            logX = -std::numeric_limits<double>::max();
//            active = false;
//        }
//        else
//            logX += log(count_above) - log(stash.size());

//        logger.log_level(logX, logl_threshold);
    }

//    std::cout<<"# Linked NS run "<<run_id<<", iteration "<<iteration<<". ";
//    std::cout<<"(log(X), log(L)) = ("<<logX<<", "<<logl_threshold<<").\n";
//    if(!active)
//    {
//        std::cout<<"#    Skipping MCMC.\n#"<<std::endl;
//        ++iteration;
//        return;
//    }

//    std::cout<<"#    Doing MCMC..."<<std::flush;

//    unsigned int K;
//    if(iteration == 0)
//    {
//        K = rng.rand_int(stash.size());
//        stash[K].from_prior(rng);
//        logl_stash[K] = stash[K].log_likelihood();
//        tb_stash[K] = rng.rand();
//    }
//    else
//    {
//        unsigned int attempts = 0;
//        while(true)
//        {
//            K = rng.rand_int(stash.size());
//            ++attempts;
//            if(above[K])
//                break;
//        }
//    }

//    ModelType particle = stash[K];
//    double logl_particle = logl_stash[K];
//    double tb_particle = tb_stash[K];

//    ModelType proposal;
//    double logl_proposal, tb_proposal, logH;

//    unsigned int tries = 0;
//    unsigned int accepts = 0;

//    // Forwards
//    for(unsigned int i=K+1; i<stash.size(); ++i)
//    {
//        for(unsigned int j=0; j<thin; ++j)
//        {
//            ++tries;

//            // Do the proposal
//            proposal = particle;
//            logH = proposal.perturb(rng);
//            logl_proposal = proposal.log_likelihood();
//            tb_proposal = tb_particle + rng.randh();
//            DNest4::wrap(tb_proposal, 0.0, 1.0);

//            if(rng.rand() <= exp(logH))
//            {
//                if(logl_proposal > logl_threshold ||
//                    (logl_proposal == logl_threshold &&
//                     tb_proposal   == tb_threshold))
//                {
//                    particle = proposal;
//                    logl_particle = logl_proposal;
//                    tb_particle = tb_proposal;
//                    ++accepts;
//                }
//            }
//        }

//        stash[i] = particle;
//        logl_stash[i] = logl_particle;
//        tb_stash[i] = tb_particle;
//        ++mcmc_steps_taken;
//    }

//    // Backwards
//    particle = stash[K];
//    logl_particle = logl_stash[K];
//    tb_particle = tb_stash[K];

//    for(int i=K-1; i>=0; --i)
//    {
//        for(unsigned int j=0; j<thin; ++j)
//        {
//            ++tries;

//            // Do the proposal
//            proposal = particle;
//            logH = proposal.perturb(rng);
//            logl_proposal = proposal.log_likelihood();
//            tb_proposal = tb_particle + rng.randh();
//            DNest4::wrap(tb_proposal, 0.0, 1.0);

//            if(rng.rand() <= exp(logH))
//            {
//                if(logl_proposal > logl_threshold ||
//                    (logl_proposal == logl_threshold &&
//                     tb_proposal   == tb_threshold))
//                {
//                    particle = proposal;
//                    logl_particle = logl_proposal;
//                    tb_particle = tb_proposal;
//                    ++accepts;
//                }
//            }
//        }

//        stash[i] = particle;
//        logl_stash[i] = logl_particle;
//        tb_stash[i] = tb_particle;
//        ++mcmc_steps_taken;
//    }

//    std::cout<<"done. Accepted "<<accepts<<'/'<<tries<<".\n#"<<std::endl;

//    ++iteration;
}

} // namespace LiNeS


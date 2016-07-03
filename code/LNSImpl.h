#include <iostream>

namespace LiNeS
{

template<class ModelType>
LNS<ModelType>::LNS(const ClassicLogger& classic_logger,
                                        unsigned int seed)
:iteration(0)
,logX(0.0)
{
    rng.set_seed(seed);

    size_t num_particles = classic_logger.get_num_particles();
    std::vector<double> logl = classic_logger.get_log_likelihoods();
    std::vector<double> tb = classic_logger.get_tiebreakers();

    // Extract every nth to become a level
    for(size_t i=1; i<(logl.size()/num_particles); ++i)
    {
        levels_log_likelihoods.push_back(logl[num_particles*i]);
        levels_tiebreakers.push_back(tb[num_particles*i]);
    }
}

template<class ModelType>
void LNS<ModelType>::run(unsigned int mcmc_steps)
{
    for(size_t i=0; i<levels_log_likelihoods.size(); ++i)
        do_iteration(mcmc_steps);
}

template<class ModelType>
void LNS<ModelType>::do_iteration(unsigned int mcmc_steps)
{
    // Allocate space
    if(stash.size() != mcmc_steps+1)
    {
        stash.resize(mcmc_steps+1);
        logl_stash.resize(mcmc_steps+1);
        tb_stash.resize(mcmc_steps+1);
    }

    // Set the likelihood threshold
    double logl_threshold = (iteration == 0)?
                                (-std::numeric_limits<double>::max()):
                                (levels_log_likelihoods[iteration-1]);
    double tb_threshold = (iteration == 0)?
                                (0.0):
                                (levels_tiebreakers[iteration-1]);

    // Number of particles in the stash that are above the threshold    
    size_t count_above = 0;
    if(iteration > 0)
    {
        for(size_t i=0; i<stash.size(); ++i)
        {
            if(logl_stash[i] > logl_threshold ||
                    (logl_stash[i] == logl_threshold &&
                     tb_stash[i]   == tb_threshold))
                ++count_above;
        }
        if(count_above == 0)
            return;
        logX += log(count_above) - log(stash.size());
    }

    std::cout<<"# Linked NS Iteration "<<iteration<<". ";
    std::cout<<"log(X) = "<<logX<<".\n";
    std::cout<<"    Doing MCMC..."<<std::flush;

    unsigned int K;
    if(iteration == 0)
    {
        K = rng.rand_int(mcmc_steps);
        stash[K].from_prior(rng);
        logl_stash[K] = stash[K].log_likelihood();
        tb_stash[K] = rng.rand();
    }
    else
    {
        while(true)
        {
            K = rng.rand_int(mcmc_steps);
            if(logl_stash[K] > logl_threshold ||
                (logl_stash[K] == logl_threshold &&
                 tb_stash[K]   == tb_threshold))
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
        stash[i] = particle;
        logl_stash[i] = logl_particle;
        tb_stash[i] = tb_particle;
    }

    std::cout<<"done. Accepted "<<accepts<<'/'<<tries<<".\n"<<std::endl;

    ++iteration;
}

} // namespace LiNeS


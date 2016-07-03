#include <iostream>

namespace LiNeS
{

template<class ModelType>
LiNeSampler<ModelType>::LiNeSampler(const ClassicLogger& classic_logger,
                                        unsigned int seed)
:iteration(0)
{
    rng.set_seed(seed);

    size_t num_particles = classic_logger.get_num_particles();
    std::vector<double> logl = classic_logger.get_log_likelihoods();
    std::vector<double> tb = classic_logger.get_tiebreakers();

    // Extract every nth to become a level
    for(size_t i=0; i<(logl.size()/num_particles); ++i)
    {
        levels_log_likelihoods.push_back(logl[num_particles*i]);
        levels_tiebreakers.push_back(tb[num_particles*i]);
    }

    for(size_t i=0; i<levels_log_likelihoods.size(); ++i)
        std::cout<<i<<' '<<levels_log_likelihoods[i]<<std::endl;
}

template<class ModelType>
void LiNeSampler<ModelType>::do_iteration(unsigned int mcmc_steps)
{
    if(iteration == 0)
        particle.from_prior(rng);

    // Save particles from the MCMC
    stash.resize(mcmc_steps);
    logl_stash.resize(mcmc_steps);
    tb_stash.resize(mcmc_steps);

    ModelType proposal;
    double logl_proposal, tb_proposal, logH;
    for(unsigned int i=0; i<mcmc_steps; ++i)
    {
        // Do the proposal
        proposal = particle;
        logH = particle.perturb(rng);

        if(rng.rand() <= exp(logH))
        {
            
        }
        stash[i] = particle;
        
    }

    ++iteration;
}

} // namespace LiNeS


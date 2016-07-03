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
    {
        particle.from_prior(rng);
        logl_particle = particle.log_likelihood();
        tb_particle = rng.rand();
    }

    // Set the likelihood threshold
    double logl_threshold = (iteration == 0)?
                                (-std::numeric_limits<double>::max()):
                                (levels_log_likelihoods[iteration-1]);
    double tb_threshold = (iteration == 0)?
                                (0.0):
                                (levels_tiebreakers[iteration-1]);

    // Save particles from the MCMC
    stash.resize(mcmc_steps);
    logl_stash.resize(mcmc_steps);
    tb_stash.resize(mcmc_steps);

    ModelType proposal;
    double logl_proposal, tb_proposal, logH;

    unsigned int K = rng.rand_int(mcmc_steps);
    stash[K] = particle;
    logl_stash[K] = logl_particle;
    tb_stash[K] = tb_particle;

    // Forwards
    for(unsigned int i=K+1; i<mcmc_steps; ++i)
    {
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
            }
        }
        stash[i] = particle;
        logl_stash[i] = logl_particle;
        tb_stash[i] = tb_particle;
        std::cout<<i<<' ';
    }

    // Backwards
    particle = stash[K];
    logl_particle = logl_stash[K];
    tb_particle = tb_stash[K];
    for(int i=K-1; i>=0; --i)
    {
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
            }
        }
        stash[i] = particle;
        logl_stash[i] = logl_particle;
        tb_stash[i] = tb_particle;
        std::cout<<i<<' ';
    }

    ++iteration;
}

} // namespace LiNeS


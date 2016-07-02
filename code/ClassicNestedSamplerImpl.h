#include <stdexcept>
#include <limits>
#include <iostream>
#include <iomanip>
#include "DNest4/code/Utils.h"

namespace LiNeS
{

template<class ModelType>
ClassicNestedSampler<ModelType>::
    ClassicNestedSampler(size_t num_particles, unsigned int seed)
:rng()
,num_particles(num_particles)
,iteration(-1)  // -1 = particles not initialised.
,particles(num_particles)
,log_likelihoods(num_particles)
,tiebreakers(num_particles)
,result_logger(num_particles)
{
    if(num_particles == 0)
        throw std::domain_error("ERROR constructing ClassicNestedSampler:\
                                            num_particles can't be zero.");
    rng.set_seed(seed);
}

template<class ModelType>
void ClassicNestedSampler<ModelType>::initialise_particles()
{
    std::cout<<"# Generating "<<num_particles<<" particles from the prior...";
    std::cout<<std::flush;

    for(size_t i=0; i<num_particles; ++i)
    {
        particles[i].from_prior(rng);
        log_likelihoods[i] = particles[i].log_likelihood();
        tiebreakers[i] = rng.rand();
    }
    std::cout<<"done."<<std::endl<<std::endl;

    // Set iteration to zero
    iteration = 0;
}

template<class ModelType>
size_t ClassicNestedSampler<ModelType>::find_worst_particle() const
{
    size_t which;

    double logl = std::numeric_limits<double>::max();
    double tb = 1.0;

    bool success = false;

    for(size_t i=0; i<num_particles; ++i)
    {
        if(log_likelihoods[i] < logl ||
                (log_likelihoods[i] == logl && tiebreakers[i] < tb))
        {
            logl = log_likelihoods[i];
            tb = tiebreakers[i];
            which = i;
            success = true;
        }
    }
    if(!success)
        std::cerr<<"# WARNING in ClassicNestedSampler::find_worst_particle"<<
                    std::endl;

    return which;
}

template<class ModelType>
double ClassicNestedSampler<ModelType>::get_depth() const
{
    return iteration*(1.0/num_particles);
}

template<class ModelType>
void ClassicNestedSampler<ModelType>::do_iteration(unsigned int mcmc_steps)
{
    // Initialise the particles if it hasn't been done already
    if(iteration == -1)
        initialise_particles();
    ++iteration;

    // Find and save worst particle
    size_t worst = find_worst_particle();
    result_logger.log_particle(log_likelihoods[worst], tiebreakers[worst]);

    std::cout<<std::setprecision(12);
    std::cout<<"# Iteration "<<iteration<<". ";
    std::cout<<"log(X) = "<<iteration*(-1.0/num_particles)<<". ";
    std::cout<<"log(L) = "<<log_likelihoods[worst]<<"."<<std::endl;
    std::cout<<"#    log(Z) = "<<result_logger.calculate_logZ()<<". ";

    // Keep threshold
    double logl_threshold = log_likelihoods[worst];
    double tb_threshold = tiebreakers[worst];

    // An alias
    size_t& which = worst;

    // Clone a survivor
    if(num_particles != 1)
    {
        size_t copy;
        do
        {
            copy = rng.rand_int(num_particles);
        }while(copy == worst);
        particles[which] = particles[copy];
        log_likelihoods[which] = log_likelihoods[copy];
        tiebreakers[which] = tiebreakers[copy];
    }

    // Print messages
    std::cout<<"\n#    Generating new particle..."<<std::flush;

    // Do MCMC
    ModelType proposal;
    double logl_proposal, tb_proposal, logH;
    unsigned int accepts = 0;
    for(size_t i=0; i<mcmc_steps; ++i)
    {
        proposal = particles[which];
        logH = proposal.perturb(rng);

        if(rng.rand() <= exp(logH))
        {
            logl_proposal = proposal.log_likelihood();
            tb_proposal = tiebreakers[which] + rng.randh();
            DNest4::wrap(tb_proposal, 0.0, 1.0);

            if(logl_proposal > logl_threshold ||
                (logl_proposal == logl_threshold && tb_proposal > tb_threshold))
            {
                particles[which] = proposal;
                log_likelihoods[which] = logl_proposal;
                tiebreakers[which] = tb_proposal;
                ++accepts;
            }
        }
    }

    std::cout<<"done. Accepted "<<accepts<<"/"<<mcmc_steps<<"."<<std::endl;
    std::cout<<"#\n";
}

} // namespace LiNeS


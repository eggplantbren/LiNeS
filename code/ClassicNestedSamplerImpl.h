#include <stdexcept>
#include <limits>
#include <iostream>

namespace LiNeS
{

template<class ModelType>
ClassicNestedSampler<ModelType>::
    ClassicNestedSampler(std::size_t num_particles, unsigned int seed)
:rng()
,num_particles(num_particles)
,iteration(-1)  // -1 = not initialised
,particles(num_particles)
,log_likelihoods(num_particles)
,tiebreakers(num_particles)
{
    if(num_particles == 0)
        throw std::domain_error("ERROR: num_particles can't be zero.");
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
    std::cout<<"done."<<std::endl;

    // Set iteration to zero
    iteration = 0;
}

template<class ModelType>
size_t ClassicNestedSampler<ModelType>::find_worst_particle() const
{
    size_t which;

    double logl = std::numeric_limits<double>::max();
    double tb = 1.0;

    for(size_t i=0; i<num_particles; ++i)
    {
        if(log_likelihoods[i] < logl ||
                (log_likelihoods[i] == logl && tiebreakers[i] < tb))
        {
            logl = log_likelihoods[i];
            tb = tiebreakers[i];
            which = i;
        }
    }

    return which;
}

template<class ModelType>
void ClassicNestedSampler<ModelType>::do_iteration()
{
    if(iteration == -1)
        initialise_particles();
}

} // namespace LiNeS

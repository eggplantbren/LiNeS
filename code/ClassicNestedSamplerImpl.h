#include <stdexcept>

namespace LiNeS
{

template<class ModelType>
ClassicNestedSampler<ModelType>::
    ClassicNestedSampler(std::size_t num_particles, unsigned int seed)
:num_particles(num_particles)
,particles(num_particles)
,log_likelihoods(num_particles)
,rng()
{
    if(num_particles == 0)
        throw std::domain_error("ERROR: num_particles can't be zero.");
    rng.set_seed(seed);
}

template<class ModelType>
void ClassicNestedSampler<ModelType>::initialise_particles()
{
    for(size_t i=0; i<num_particles; ++i)
    {
        particles[i].from_prior(rng);
        log_likelihoods[i] = particles[i].get_log_likelihood();
    }
}

} // namespace LiNeS


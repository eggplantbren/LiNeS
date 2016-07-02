#include <stdexcept>

namespace LiNeS
{

template<class ModelType>
ClassicNestedSampler<ModelType>::
    ClassicNestedSampler(std::size_t num_particles, unsigned int seed)
:particles(num_particles)
,rng()
{
    if(num_particles == 0)
        throw std::domain_error("ERROR: num_particles can't be zero.");
    rng.set_seed(seed);
}

template<class ModelType>
void ClassicNestedSampler<ModelType>::initialise_particles()
{
    for(auto& particle: particles)
        particle.from_prior();
}

} // namespace LiNeS


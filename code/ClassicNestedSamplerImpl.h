#include <stdexcept>
#include <limits>

namespace LiNeS
{

template<class ModelType>
ClassicNestedSampler<ModelType>::
    ClassicNestedSampler(std::size_t num_particles, unsigned int seed)
:rng()
,num_particles(num_particles)
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
    for(size_t i=0; i<num_particles; ++i)
    {
        particles[i].from_prior(rng);
        log_likelihoods[i] = particles[i].log_likelihood();
        tiebreakers[i] = rng.rand();
    }
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

} // namespace LiNeS


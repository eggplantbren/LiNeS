#ifndef LiNeS_ClassicNestedSampler
#define LiNeS_ClassicNestedSampler

#include <vector>
#include <stddef.h>
#include "DNest4/code/RNG.h"

namespace LiNeS
{

/*
* An object of this class is a classic nested sampler.
*/
template<class ModelType>
class ClassicNestedSampler
{
    private:
        // A random number generator to use.
        DNest4::RNG rng;

        // The number of particles
        size_t num_particles;

        // Iteration counter
        int iteration;

        // Readiness flag
        bool ready;

        // The particles, their log-likelihoods, and tiebreakers.
        std::vector<ModelType> particles;
        std::vector<double> log_likelihoods;
        std::vector<double> tiebreakers;

    public:
        /*
        * Constructor arguments:
        *   size_t num_particles = the number of particles
        *   unsigned int seed = random number generator seed
        */
        ClassicNestedSampler(size_t num_particles, unsigned int seed=0);

        /*
        * Do an iteration of Nested Sampling.
        */
        void do_iteration();

    private:
        /*
        * Initialise the particles by generating them from the prior.
        */
        void initialise_particles();

        /*
        * Find the worst (lowest likelihood) particle and returns its index.
        */
        size_t find_worst_particle() const;

};

} // namespace LiNeS

#include "ClassicNestedSamplerImpl.h"

#endif


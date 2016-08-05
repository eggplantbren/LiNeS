#ifndef LiNeS_ClassicNestedSampler
#define LiNeS_ClassicNestedSampler

#include <vector>
#include <stddef.h>
#include <ctime>
#include <tuple>
#include "DNest4/code/RNG.h"
#include "ClassicLogger.h"
#include "LiNeS.h"

namespace LiNeS
{

enum class Verbosity {low, medium, high};

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

        // The particles, their log-likelihoods, and tiebreakers.
        std::vector<ModelType> particles;
        std::vector< std::vector<double> > scalars;
        std::vector< std::vector<double> > tiebreakers;

        // Lower limits and selection probabilities for scalars
        std::vector<double> floor;
        std::vector<double> tb_floor;
        std::vector<double> selection_probs;

        // Results get stored here.
        ClassicLogger logger;

        // How much output to print
        Verbosity verbosity;

    public:
        /*
        * Constructor arguments:
        *   size_t num_particles = the number of particles
        *   unsigned int seed = random number generator seed
        */
        ClassicNestedSampler(size_t num_particles, DNest4::RNG rng);

        /*
        * Do an iteration of Nested Sampling.
        */
        void do_iteration(unsigned int mcmc_steps=1000);

        /*
        * Run to a certain depth
        */
        void run(double max_depth, unsigned int mcmc_steps=1000);

        // Setter
        void set_verbosity(Verbosity v)
        { verbosity = v; }

        /*
        * Get the logger (returns a const reference)
        */
        const ClassicLogger& get_logger() const
        { return logger; }

        /* Get a copy of the Random Number Generator */
        DNest4::RNG get_rng() const
        { return rng; }

    private:
        /*
        * Initialise the particles by generating them from the prior.
        */
        void initialise_particles();

        /*
        * Find the worst (lowest likelihood) particle and returns its index.
        */
        std::tuple<size_t, size_t> find_worst_particle();

        /*
        * Estimate -log(X) of the most recent iteration.
        */
        double get_depth() const;
};

} // namespace LiNeS

#include "ClassicNestedSamplerImpl.h"

#endif


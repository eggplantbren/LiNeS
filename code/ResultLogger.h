#ifndef LiNeS_ResultLogger
#define LiNeS_ResultLogger

#include <vector>
#include <stddef.h>

namespace LiNeS
{

class ResultLogger
{
    private:
        // Number of particles in the run
        size_t num_particles;

        // Log likelihoods and tiebreakers
        std::vector<double> log_likelihoods;
        std::vector<double> tiebreakers;

    public:
        /*
        * Constructor arguments:
        *       size_t num_particles - number of particles in the run
        */
        ResultLogger(size_t num_particles);

        /*
        * log a particle
        * arguments:
        *       double logl = log likelihood
        *       double tb   = tiebreaker
        */
        void log_particle(double logl, double tb);
};

} // namespace LiNeS

#endif


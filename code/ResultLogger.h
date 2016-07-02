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

        // Iteration and log likelihood
        std::vector<size_t> iteration;
        std::vector<double> log_likelihood;

    public:
        ResultLogger(size_t num_particles);

};

} // namespace LiNeS

#endif


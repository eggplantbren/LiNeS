#ifndef LiNeS_LNS
#define LiNeS_LNS

#include "DNest4/code/RNG.h"
#include "ClassicLogger.h"

namespace LiNeS
{

template <class ModelType>
class LNS
{
    private:
        // An RNG to use
        DNest4::RNG rng;

        // The particle stash
        std::vector<ModelType> stash;
        std::vector<double> logl_stash;
        std::vector<double> tb_stash;

        // The levels
        std::vector<double> levels_log_likelihoods;
        std::vector<double> levels_tiebreakers;

        // Iteration counter
        unsigned int iteration;

        // Compression estimate
        double logX;

    public:
        LNS(const ClassicLogger& classic_logger,
                                                    unsigned int seed=time(0));

        // Do an iteration
        void do_iteration(unsigned int mcmc_steps=1000);

};

} // namespace LiNeS

#include "LNSImpl.h"

#endif


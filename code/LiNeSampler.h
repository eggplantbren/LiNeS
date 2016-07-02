#ifndef LiNeS_LiNeSampler
#define LiNeS_LiNeSampler

#include "DNest4/code/RNG.h"
#include "ClassicLogger.h"

namespace LiNeS
{

template <class ModelType>
class LiNeSampler
{
    private:
        // An RNG to use
        DNest4::RNG rng;

        // The single particle
        ModelType particle;

        std::vector<double> levels_log_likelihoods;
        std::vector<double> levels_tiebreakers;

    public:
        LiNeSampler(const ClassicLogger& classic_logger,
                                                    unsigned int seed=time(0));

        // Do an iteration
        void do_iteration();

};

} // namespace LiNeS

#include "LiNeSamplerImpl.h"

#endif


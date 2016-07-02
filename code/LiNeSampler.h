#ifndef LiNeS_LiNeSampler
#define LiNeS_LiNeSampler

#include "ClassicLogger.h"

namespace LiNeS
{

template <class ModelType>
class LiNeSampler
{
    private:
        // The single particle
        ModelType particle;

        std::vector<double> levels_log_likelihoods;
        std::vector<double> levels_tiebreakers;

    public:
        LiNeSampler(const ClassicLogger& classic_logger);

};

} // namespace LiNeS

#include "LiNeSamplerImpl.h"

#endif


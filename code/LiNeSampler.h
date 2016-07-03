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
        double logl_particle;
        double tb_particle;

        // The particle stash
        std::vector<ModelType> stash;
        std::vector<double> logl_stash;
        std::vector<double> tb_stash;

        // The levels
        std::vector<double> levels_log_likelihoods;
        std::vector<double> levels_tiebreakers;

        // Iteration counter
        unsigned int iteration;

    public:
        LiNeSampler(const ClassicLogger& classic_logger,
                                                    unsigned int seed=time(0));

        // Do an iteration
        void do_iteration(unsigned int mcmc_steps=1000);

};

} // namespace LiNeS

#include "LiNeSamplerImpl.h"

#endif


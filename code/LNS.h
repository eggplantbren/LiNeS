#ifndef LiNeS_LNS
#define LiNeS_LNS

#include "DNest4/code/RNG.h"
#include "DNest4/code/Utils.h"
#include "ClassicLogger.h"
#include "LNSLogger.h"
#include "LiNeS.h"

#include <iostream>
#include <iomanip>

namespace LiNeS
{

template <class ModelType>
class LNS
{
    private:
        // An identity for the run
        unsigned int run_id;

        // An RNG to use
        DNest4::RNG rng;

        // The particle stash
        std::vector<ModelType> stashed_particles;
        std::vector< std::vector<double> > stashed_scalars;
        std::vector< std::vector<double> > stashed_tiebreakers;

        // The levels
        std::vector< std::vector<double> > levels_scalars;
        std::vector< std::vector<double> > levels_tiebreakers;

        // Iteration counter
        unsigned int iteration;

        // Compression estimate
        double logX;

        // Logger for results
        LNSLogger logger;

        // If active
        bool active;

        // Count MCMC steps taken
        unsigned int mcmc_steps_taken;

    public:
        /* Get levels from a ClassicLogger. */
        LNS(unsigned int run_id, const ClassicLogger& classic_logger,
                                        DNest4::RNG rng=DNest4::RNG());

        // Run LNS
        void run(unsigned int mcmc_steps=1000, unsigned int thin=1);

        // Getter
        const LNSLogger& get_logger() const
        { return logger; }

        // Getter
        unsigned int get_mcmc_steps_taken() const
        { return mcmc_steps_taken; }

        // Return a copy of the RNG
        DNest4::RNG get_rng() const
        { return rng; }

    private:
        // Do an iteration
        void do_iteration(unsigned int mcmc_steps, unsigned int thin);
};

} // namespace LiNeS

#include "LNSImpl.h"

#endif


#include <iostream>
#include "LiNeS.h"
#include "ClassicNestedSampler.h"
#include "LNS.h"
#include "Models/SpikeSlab.h"

int main()
{
    // Clear previous output files
    LiNeS::clear_all_output_files();

    // Create a classic sampler with 100 particles.
    LiNeS::ClassicNestedSampler<SpikeSlab> classic_sampler(100);

    // Run to 100 nats depth, with 1000 MCMC steps per iteration
    classic_sampler.run(100.0, 500);

    // Get a copy of the RNG from the Classic Nested Sampler
    DNest4::RNG rng = classic_sampler.get_rng();

    for(int i=0; i<1000000; ++i)
    {
        // Create a Linked Nested Sampler
        LiNeS::LNS<SpikeSlab> lns(i, classic_sampler.get_logger(), rng);

        // Run it
        lns.run(10000, 10);

        // Continue using the same rng
        rng = lns.get_rng();
    }

    return 0;
}


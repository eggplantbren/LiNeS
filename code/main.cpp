#include <iostream>
#include "LiNeS.h"
#include "ClassicNestedSampler.h"
#include "Models/SimpleExample.h"
#include "LNS.h"

int main()
{
    // Clear previous output files
    LiNeS::clear_all_output_files();

    // Create an RNG
    DNest4::RNG rng;
    rng.set_seed(time(0));

    for(int i=0; i<10; ++i)
    {
        // Create a classic sampler with 100 particles.
        LiNeS::ClassicNestedSampler<SimpleExample> classic_sampler(1000, rng);

        // Run to 200 nats depth, with 1000 MCMC steps per iteration
        classic_sampler.run(200.0, 1000);

        // Get a copy of the RNG from the Classic Nested Sampler
        DNest4::RNG rng = classic_sampler.get_rng();

        // Create a Linked Nested Sampler
        LiNeS::LNS<SimpleExample> lns(i+1, classic_sampler.get_logger(), rng);

        // Run it
        lns.run(30000, 30);

        // Continue using the same rng
        rng = lns.get_rng();
    }

    return 0;
}


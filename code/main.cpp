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
    LiNeS::ClassicNestedSampler<SpikeSlab> sampler(100);

    // Run to 100 nats depth, with 1000 MCMC steps per iteration
    sampler.run(100.0, 500);

    for(int i=0; i<1000000; ++i)
    {
        // Create a Linked Nested Sampler
        LiNeS::LNS<SpikeSlab> lns(i, sampler.get_logger());

        // Run it
        lns.run(100000, 10);
    }

    return 0;
}


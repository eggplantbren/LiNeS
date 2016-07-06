#include <iostream>
#include "ClassicNestedSampler.h"
#include "LNS.h"
#include "Models/SpikeSlab.h"

int main()
{
    // Create a classic sampler with 100 particles.
    LiNeS::ClassicNestedSampler<SpikeSlab> sampler(1000);

    // Run to 100 nats depth, with 1000 MCMC steps per iteration
    sampler.run(100.0, 500);

    // Clear previous output files (well, LNS ones anyway)
    LNSLogger::clear_output_files();

    for(int i=0; i<1000000; ++i)
    {
        // Create a Linked Nested Sampler
        LiNeS::LNS<SpikeSlab> lns(i, sampler.get_logger());

        // Run it
        lns.run(100000, 10);

        // Save it
        lns.get_logger().save(i > 0);
    }

    return 0;
}


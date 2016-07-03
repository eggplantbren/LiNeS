#include <iostream>
#include "ClassicNestedSampler.h"
#include "LiNeSampler.h"
#include "Models/SpikeSlab.h"

int main()
{
    // Create a classic sampler with 100 particles.
    LiNeS::ClassicNestedSampler<SpikeSlab> sampler(100);

    // Run to 100 nats depth, with 1000 MCMC steps per iteration
    sampler.run(100.0, 1000);

    // Create a Linked Nested Sampler
    LiNeS::LiNeSampler<SpikeSlab> lns(sampler.get_logger());

    lns.do_iteration();

    return 0;
}


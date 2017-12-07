#include <iostream>
#include "LiNeS.h"
#include "ClassicNestedSampler.h"
#include "LNS.h"
#include "Models/Rosenbrock.h"

int main()
{
    // Clear previous output files
    LiNeS::clear_all_output_files();

    // Create a classic sampler with 100 particles.
    LiNeS::ClassicNestedSampler<Rosenbrock> classic_sampler(100);

    // Run to 100 nats depth, with 1000 MCMC steps per iteration
    classic_sampler.run(500.0, 10000);

    // Get a copy of the RNG from the Classic Nested Sampler
    DNest4::RNG rng = classic_sampler.get_rng();

    // Count MCMC steps
    unsigned int count_mcmc_steps = 0;

    for(int i=0; i<100; ++i)
    {
        // Create a Linked Nested Sampler
        LiNeS::LNS<Rosenbrock> lns(i+1, classic_sampler.get_logger(), rng);

        // Continue using the same rng
        rng = lns.get_rng();

        // Number of MCMC steps per level
        int k = 100000*exp(rng.randn());

        // Run it
        lns.run(k, 100);

        // Accumulate number of mcmc steps
        count_mcmc_steps += lns.get_mcmc_steps_taken();

        // Continue using the same rng
        rng = lns.get_rng();
    }

    std::cout << "# Total number of MCMC steps taken (excluding classic NS) = ";
    std::cout << count_mcmc_steps << '.' << std::endl;

    return 0;
}


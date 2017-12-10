#include <iostream>
#include "LiNeS.h"
#include "ClassicNestedSampler.h"
#include "LNS.h"
#include "Models/Rosenbrock.h"
#include "DNest4/code/DNest4.h"

int main(int argc, char** argv)
{
    // Choose the problem
    typedef Rosenbrock ModelType;

    // Clear previous output files
    LiNeS::clear_all_output_files();

    // Run DNest4
	DNest4::CommandLineOptions options(argc, argv);
	DNest4::Sampler<ModelType> sampler = DNest4::setup<ModelType>(options);
	sampler.run();

    // Get a copy of the first RNG from the DNest4 sampler
    DNest4::RNG rng = sampler.get_rngs()[0];

    // Count MCMC steps
    unsigned int count_mcmc_steps = 0;

    for(int i=0; i<100; ++i)
    {
        // Create a Linked Nested Sampler
        LiNeS::LNS<ModelType> lns(i+1, "levels.txt", rng);

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


#ifndef LiNeS_SimpleExample
#define LiNeS_SimpleExample

#include "DNest4/code/DNest4.h"
#include <vector>
#include <ostream>

class SimpleExample
{
    private:
        static const size_t num_scalars = 2;

        std::vector<double> params;

    public:
        // Constructor only gives size of params
        SimpleExample();

        // Generate the point from the prior
        void from_prior(DNest4::RNG& rng);

        // Metropolis-Hastings proposals
        double perturb(DNest4::RNG& rng);

        // Objective functions
        std::vector<double> get_scalars() const;

        // Print to stream
        void print(std::ostream& out) const;

        // Return string with column information
        std::string description() const;

        // Return number of parameters
        static size_t get_num_scalars()
        { return num_scalars; }
};

#endif

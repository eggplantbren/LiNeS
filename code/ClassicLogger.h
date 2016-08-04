#ifndef LiNeS_ClassicLogger
#define LiNeS_ClassicLogger

#include <vector>
#include <stddef.h>
#include <fstream>

namespace LiNeS
{

class ClassicLogger
{
    private:
        // Number of particles in the run
        size_t num_particles, num_scalars;

        // Log likelihoods and tiebreakers
        std::vector< std::vector<double> > scalars;
        std::vector< std::vector<double> > tiebreakers;

        // Output file stream
        std::fstream fout;

    public:
        /*
        * Constructor arguments:
        *       size_t num_particles - number of particles in the run
        */
        ClassicLogger(size_t num_particles, size_t num_scalars);

        /*
        * Destructor (closes fstream)
        */
        ~ClassicLogger();

        /*
        * log a particle
        * arguments:
        *       double s    = scalar
        *       double tb   = tiebreaker
        *       size_t which_scalar = which scalar
        */
        void log_particle(double s, double tb, size_t which_scalar);

        /*
        * Calculate log(Z)
        */
        double calculate_logZ(double temperature=1.0) const;

        /*
        * Getters (these return copies)
        */
//        size_t get_num_particles() const
//        { return num_particles; }
//        std::vector<double> get_log_likelihoods() const
//        { return log_likelihoods; }
//        std::vector<double> get_tiebreakers() const
//        { return tiebreakers; }

        // Clear output files
        static void clear_output_files();
};

} // namespace LiNeS

#endif


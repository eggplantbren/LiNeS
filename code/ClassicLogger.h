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
        size_t num_particles;

        // Log likelihoods and tiebreakers
        std::vector<double> log_likelihoods;
        std::vector<double> tiebreakers;

        // Output file stream
        std::fstream fout;

    public:
        /*
        * Constructor arguments:
        *       size_t num_particles - number of particles in the run
        */
        ClassicLogger(size_t num_particles);

        /*
        * Destructor (closes fstream)
        */
        ~ClassicLogger();

        /*
        * log a particle
        * arguments:
        *       double logl = log likelihood
        *       double tb   = tiebreaker
        */
        void log_particle(double logl, double tb);

        /*
        * Calculate log(Z)
        */
        double calculate_logZ(double temperature=1.0) const;

        /*
        * Getters (these return copies)
        */
        size_t get_num_particles() const
        { return num_particles; }
        std::vector<double> get_log_likelihoods() const
        { return log_likelihoods; }
        std::vector<double> get_tiebreakers() const
        { return tiebreakers; }

        // Clear output files
        static void clear_output_files();
};

} // namespace LiNeS

#endif


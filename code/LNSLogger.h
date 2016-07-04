#ifndef LiNeS_LNSLogger
#define LiNeS_LNSLogger

#include <vector>
#include <stdlib.h>

namespace LiNeS
{

/*
* Logger for the results of a *single* LNS run.
*/
class LNSLogger
{
    private:
        // Info about levels
        std::vector<double> levels_logX;
        std::vector<double> levels_logL;

        // Info about particles
        std::vector<size_t> particles_level_id;
        std::vector<double> particles_logL;

    public:
        // Constructor (all the vectors start with size 0)
        LNSLogger();

        // Add to the log
        void log_level(double logX, double logL);
        void log_particle(size_t level_id, double logL);

        // Save the log
        void save(bool append=false) const;
};

} // namespace LiNeS

#endif


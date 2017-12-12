#ifndef LiNeS_LNSLogger
#define LiNeS_LNSLogger

#include <cstdlib>
#include <fstream>
#include <mutex>
#include <vector>

namespace LiNeS
{

static std::mutex mutex;

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
        std::vector<unsigned int> particles_run_id;
        std::vector<size_t> particles_level_id;
        std::vector<double> particles_logL;

    public:
        // Constructor (all the vectors start with size 0)
        LNSLogger();

        // Destructor (saves logX to file)
        ~LNSLogger();

        // Add to the log
        void log_level(double logX, double logL);

        // Log particle info
        void log_particle_info(unsigned int run_id, size_t level_id,
                                                                  double logL);

        // Save a particle to disk
        template<class ModelType>
        void save_particle(const ModelType& particle) const;

        // Clear output files
        static void clear_output_files();
};


/* Implement the template function */
template<class ModelType>
void LNSLogger::save_particle(const ModelType& particle) const
{
    mutex.lock();
    {
        std::fstream fout("particles.txt", std::ios::out | std::ios::app);
        particle.print(fout);
        fout<<std::endl;    
        fout.close();
    }
    mutex.unlock();
}

} // namespace LiNeS

#endif


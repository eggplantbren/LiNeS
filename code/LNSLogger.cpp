#include "LNSLogger.h"
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

namespace LiNeS
{

LNSLogger::LNSLogger()
{

}

void LNSLogger::log_level(double logX, double logL)
{
    levels_logX.push_back(logX);
    levels_logL.push_back(logL);

    std::fstream fout;

    fout.open("levels_logL.txt", std::ios::out | std::ios::app);
 
    fout<<std::setprecision(12);
    fout<<logL<<std::endl;
    fout.close();

    fout.open("levels_logX.txt", std::ios::out | std::ios::app);

    fout<<std::setprecision(12);
    fout<<logX<<std::endl;
    fout.close();
}

void LNSLogger::log_particle(unsigned int run_id, size_t level_id, double logL)
{
    particles_run_id.push_back(run_id);
    particles_level_id.push_back(level_id);
    particles_logL.push_back(logL);

    std::fstream fout;

    fout.open("particles_info.txt", std::ios::out | std::ios::app);
    fout<<std::setprecision(12);
    fout<<run_id<<' '<<level_id<<' '<<logL<<std::endl;
    fout.close();
}

void LNSLogger::clear_output_files()
{
    std::vector<std::string> filenames{"levels_logL.txt",
                                       "levels_logX.txt",
                                       "particles.txt",
                                       "particles_info.txt"};

    for(const auto& filename: filenames)
    {
        std::fstream fout(filename, std::ios::out);
        fout.close();
    }
}

} // namespace LiNeS


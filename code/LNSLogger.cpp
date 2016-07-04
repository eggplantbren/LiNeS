#include "LNSLogger.h"
#include <fstream>
#include <iomanip>

namespace LiNeS
{

LNSLogger::LNSLogger()
{

}

void LNSLogger::log_level(double logX, double logL)
{
    levels_logX.push_back(logX);
    levels_logL.push_back(logL);
}

void LNSLogger::log_particle(unsigned int run_id, size_t level_id, double logL)
{
    particles_run_id.push_back(run_id);
    particles_level_id.push_back(level_id);
    particles_logL.push_back(logL);
}

void LNSLogger::save(bool append) const
{
    std::fstream fout;

    // Save the levels' log-likelihoods
    if(!append)
    {
        fout.open("levels_logL.txt", std::ios::out);
        fout<<std::setprecision(12);
        for(size_t i=0; i<levels_logL.size(); ++i)
           fout<<levels_logL[i]<<' ';
        fout<<std::endl;
        fout.close();
    }

    // Save the logX estimates
    if(append)
        fout.open("levels_logX.txt", std::ios::out | std::ios::app);
    else
        fout.open("levels_logX.txt", std::ios::out);
    for(size_t i=0; i<levels_logX.size(); ++i)
        fout<<levels_logX[i]<<' ';
    fout<<std::endl;
    fout.close();

    // Save the particles
    if(append)
        fout.open("particles_info.txt", std::ios::out | std::ios::app);
    else
        fout.open("particles_info.txt", std::ios::out);
    fout<<std::setprecision(12);
    for(size_t i=0; i<particles_level_id.size(); ++i)
    {
        fout<<particles_run_id[i]<<' '<<particles_level_id[i]<<' ';
        fout<<particles_logL[i]<<'\n';
    }
    fout.close();
}

} // namespace LiNeS


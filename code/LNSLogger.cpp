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

void LNSLogger::save(const char* filename, bool append) const
{
    std::fstream fout;

    if(append)
        fout.open(filename, std::ios::out | std::ios::app);
    else
        fout.open(filename, std::ios::out);

    fout<<std::setprecision(12);
    if(!append)
    for(size_t i=0; i<levels_logL.size(); ++i)
        fout<<levels_logL[i]<<' ';
    fout<<std::endl;

    for(size_t i=0; i<levels_logX.size(); ++i)
        fout<<levels_logX[i]<<' ';
    fout<<std::endl;
    fout.close();
}

} // namespace LiNeS


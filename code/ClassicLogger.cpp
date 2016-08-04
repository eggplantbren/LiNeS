#include "ClassicLogger.h"
#include <stddef.h>
#include <stdexcept>
#include <limits>
#include <iomanip>
#include "DNest4/code/Utils.h"

namespace LiNeS
{

ClassicLogger::ClassicLogger(size_t num_particles, size_t num_scalars)
:num_particles(num_particles)
,num_scalars(num_scalars)
,fout("classic_log.txt", std::ios::out)
{
    if(num_particles == 0)
        throw std::domain_error
            ("ERROR constructing ClassicLogger: num_particles can't be zero.");
    fout<<"# num_particles = "<<num_particles<<'\n';
    fout<<"# iteration, logX (deterministic estimate), logL, tiebreaker";
    fout<<std::endl;
    fout<<std::setprecision(12);
}

ClassicLogger::~ClassicLogger()
{
    fout.close();
}

void ClassicLogger::log_particle(double s, double tb, size_t which_scalar)
{
    if(scalars.size() == 0)
    {
        scalars.push_back(std::vector<double>(num_scalars, -1E300));
        tiebreakers.push_back(std::vector<double>(num_scalars, 0.0));
    }

    scalars.push_back(scalars.back());
    tiebreakers.push_back(tiebreakers.back());
    scalars.back()[which_scalar] = s;
    tiebreakers.back()[which_scalar] = tb;

    fout<<(scalars.size()-1)<<' ';
    for(size_t i=0; i<num_scalars; ++i)
        fout<<scalars.back()[i]<<' ';
    for(size_t i=0; i<num_scalars; ++i)
        fout<<tiebreakers.back()[i]<<' ';
    fout<<std::endl;

//    fout<<log_likelihoods.size()<<' ';
//    fout<<(-static_cast<double>(log_likelihoods.size())/num_particles)<<' ';
//    fout<<logl<<' '<<tb<<std::endl;
}

//double ClassicLogger::calculate_logZ(double temperature) const
//{
//    std::vector<double> logX(log_likelihoods.size());
//    std::vector<double> logp(log_likelihoods.size());

//    for(size_t i=0; i<log_likelihoods.size(); ++i)
//    {
//        // Assign log(X) deterministically
//        logX[i] = -static_cast<double>(i+1)/num_particles;

//        // Un-normalised posterior weight
//        logp[i] = logX[i] + log_likelihoods[i]/temperature;
//    }
//    return DNest4::logsumexp(logp) - DNest4::logsumexp(logX);
//}

void ClassicLogger::clear_output_files()
{
    std::vector<std::string> filenames{"classic_log.txt"};

    for(const auto& filename: filenames)
    {
        std::fstream fout(filename, std::ios::out);
        fout.close();
    }
}

} // namespace LiNeS


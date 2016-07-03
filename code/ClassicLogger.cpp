#include "ClassicLogger.h"
#include <stddef.h>
#include <stdexcept>
#include <limits>
#include <iomanip>
#include "DNest4/code/Utils.h"

namespace LiNeS
{

ClassicLogger::ClassicLogger(size_t num_particles)
:num_particles(num_particles)
,fout("classic_log.csv", std::ios::out)
{
    if(num_particles == 0)
        throw std::domain_error
            ("ERROR constructing ClassicLogger: num_particles can't be zero.");
    fout<<"logL, tiebreaker"<<std::endl;
    fout<<std::setprecision(12);
}

ClassicLogger::~ClassicLogger()
{
    fout.close();
}

void ClassicLogger::log_particle(double logl, double tb)
{
    log_likelihoods.push_back(logl);
    tiebreakers.push_back(tb);
    fout<<logl<<','<<tb<<std::endl;
}

double ClassicLogger::calculate_logZ(double temperature) const
{
    std::vector<double> logX(log_likelihoods.size());
    std::vector<double> logp(log_likelihoods.size());

    for(size_t i=0; i<log_likelihoods.size(); ++i)
    {
        // Assign log(X) deterministically
        logX[i] = -static_cast<double>(i+1)/num_particles;

        // Un-normalised posterior weight
        logp[i] = logX[i] + log_likelihoods[i]/temperature;
    }
    return DNest4::logsumexp(logp) - DNest4::logsumexp(logX);
}

} // namespace LiNeS


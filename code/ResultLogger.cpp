#include "ResultLogger.h"
#include <stddef.h>
#include <stdexcept>

namespace LiNeS
{

ResultLogger::ResultLogger(size_t num_particles)
:num_particles(num_particles)
{
    if(num_particles == 0)
        throw std::domain_error
            ("ERROR constructing ResultLogger: num_particles can't be zero.");
}

} // namespace LiNeS


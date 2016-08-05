#ifndef LiNeS_LiNeS
#define LiNeS_LiNeS

#include "ClassicLogger.h"
#include "LNS.h"
#include "LNSLogger.h"

namespace LiNeS
{

void clear_all_output_files();

bool is_okay(const std::vector<double>& s, const std::vector<double>& tb,
                const std::vector<double>& s_threshold,
                const std::vector<double>& tb_threshold);

} // namespace LiNeS

#endif


#include "LiNeS.h"
#include "ClassicLogger.h"
#include "LNSLogger.h"

namespace LiNeS
{

void clear_all_output_files()
{
    ClassicLogger::clear_output_files();
    LNSLogger::clear_output_files();
}

bool is_okay(const std::vector<double>& s, const std::vector<double>& tb,
                const std::vector<double>& s_threshold,
                const std::vector<double>& tb_threshold)
{
    for(size_t i=0; i<s.size(); ++i)
    {
        if(s[i] < s_threshold[i])
            return false;
        if(s[i] == s_threshold[i] && (tb[i] < tb_threshold[i]))
            return false;
    }

    return true;
}

} // namespace LiNeS


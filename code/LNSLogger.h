#ifndef LiNeS_LNSLogger
#define LiNeS_LNSLogger

#include <vector>

namespace LiNeS
{

class LNSLogger
{
    private:
        std::vector<double> levels_logX;

    public:
        LNSLogger();

        void log_level(double logX);
};

} // namespace LiNeS

#endif


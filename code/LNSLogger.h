#ifndef LiNeS_LNSLogger
#define LiNeS_LNSLogger

#include <vector>

namespace LiNeS
{

class LNSLogger
{
    private:
        std::vector<double> levels_logX;
        std::vector<double> levels_logL;

    public:
        LNSLogger();
        void log_level(double logX, double logL);
        void save(const char* filename, bool append=false) const;
};

} // namespace LiNeS

#endif


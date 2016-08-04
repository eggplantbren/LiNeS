#include "LiNeS.h"
#include "ClassicLogger.h"
#include "LNSLogger.h"

namespace LiNeS
{

void clear_all_output_files()
{
    ClassicLogger::clear_output_files();
//    LNSLogger::clear_output_files();
}

} // namespace LiNeS


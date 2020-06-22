#include "TTUtils.h"

#include <boost/filesystem.hpp>

namespace tt
{

std::string defaultResourceFolder()
{
    namespace fs = boost::filesystem;
#ifdef _WINDOWS
    auto cwd = fs::current_path();
    cwd /= "resources";
    return cwd.string();
#elif defined(__APPLE__)    
    return "macos";
#elif defined(__linux__)
    return "linux";
#else
    return "unknown"
#endif
}

} // namespace tt
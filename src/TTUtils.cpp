#include "TTUtils.h"

namespace tt
{

std::filesystem::path defaultResourceFolder()
{
    namespace fs = std::filesystem;
#ifdef _WINDOWS
    auto cwd = fs::current_path();
    cwd /= "resources";
    return cwd;
#elif defined(__APPLE__)    
    return "macos";
#elif defined(__linux__)
    return "linux";
#else
    return "unknown"
#endif
}

} // namespace tt
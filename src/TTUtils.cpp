#include "TTUtils.h"

#include <boost/filesystem.hpp>

namespace sf
{

void from_json(const nl::json& j, Vector2f& v)
{
   if (j.contains("x"))
   {
       j.at("x").get_to(v.x);
   }

   if (j.contains("y"))
   {
       j.at("y").get_to(v.y);
   }
}

}

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
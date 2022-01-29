#ifdef _WINDOWS
#   include <windows.h>
#   include <shellapi.h>
#   include <Shlobj.h>
#else
#   include <unistd.h>
#   include <sys/types.h>
#   include <pwd.h>
#   include <boost/process.hpp>
#endif

#ifdef __APPLE__
#   include <CoreFoundation/CFBundle.h>
#   include <ApplicationServices/ApplicationServices.h>
#   include <boost/dll.hpp>
#endif

#include "TTUtils.h"

#include <boost/filesystem.hpp>

#include <fmt/core.h>

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

void from_json(const nl::json& j, Vector2i& v)
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

void from_json(const nl::json& j, Vector2u& v)
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
    return "resources";
#elif defined(__APPLE__)
    auto exepath = boost::dll::program_location().parent_path();
    return fmt::format("{}/../Resources", exepath.string());
#elif defined(__linux__)
    return "linux";
#else
    return "unknown"
#endif
}

void openBrowser(const std::string& url_str)
{
    if (url_str.empty()) return;

#ifdef _WINDOWS
    ShellExecute(0, 0, url_str.c_str(), 0, 0, SW_SHOWNORMAL);
#elif defined(__APPLE__)
    // only works with `http://` prepended
    CFURLRef url = CFURLCreateWithBytes(
        // allocator
        nullptr,

        // URLBytes
        (UInt8*)url_str.c_str(),     // URLBytes

        // length
        static_cast<std::int32_t>(url_str.length()),

        // encoding
        kCFStringEncodingASCII,

        // baseURL
        NULL
    );

    LSOpenCFURLRef(url, nullptr);
    CFRelease(url);
#elif defined(__linux__)
    boost::process::system("/usr/bin/xdg-open", url_str,
        boost::process::std_err > boost::process::null,
        boost::process::std_out > boost::process::null);
#else
    throw NotImplementedException("openBrowser");
#endif
}

std::string getOsString()
{
#ifdef _WINDOWS
    return "windows";
#elif defined(__APPLE__)    
    return "macos";
#elif defined(__linux__)
    return "linux";
#else
    return "unknown"
#endif
}

std::string getUserFolder()
{
    std::string retval;

#ifdef _WINDOWS
    WCHAR path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, path)))
    {
        std::wstring temp(path);
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
        retval = convert.to_bytes(temp);
    }
    else
    {
        throw std::runtime_error("could not retrieve user folder");
    }
#else
struct passwd *pw = getpwuid(getuid());
retval = pw->pw_dir;
#endif

    return retval;
}

} // namespace tt

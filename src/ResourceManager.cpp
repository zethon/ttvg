#include "ResourceManager.h"

namespace tt
{

ResourceManager::ResourceManager(const boost::filesystem::path& path)
    : _resourceFolder{ path }
{}

std::string ResourceManager::getFilename(const std::string& name)
{
    auto filepath = _resourceFolder / name;
    return filepath.string();
}


} // namespace tt
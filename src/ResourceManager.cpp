#include "ResourceManager.h"

namespace tt
{

ResourceManager::ResourceManager(const boost::filesystem::path& path)
    : _resourceFolder{ path }
{}

} // namespace tt
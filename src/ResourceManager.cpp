#include "ResourceManager.h"

namespace tt
{

ResourceManager::ResourceManager(const std::filesystem::path& path)
    : _resourceFolder{ path }
{}

} // namespace tt
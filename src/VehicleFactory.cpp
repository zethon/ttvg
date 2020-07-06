#include <boost/filesystem.hpp>

#include <fmt/core.h>

#include "VehicleFactory.h"

namespace tt
{

VehicleFactory::VehicleFactory(ResourceManager& resmgr, const std::string& mapname)
    : _resources { resmgr }
{
    std::string jsonfile =
        _resources.getFilename(fmt::format("maps/{}.json", mapname));

    if (!boost::filesystem::exists(jsonfile))
    {
        auto error = fmt::format("file '{}' not found", jsonfile);
        throw std::runtime_error(error);
    }

    std::ifstream file(jsonfile.c_str());
    file >> _json;
}

VehiclePtr VehicleFactory::createVehicle()
{
    return VehiclePtr();
}

} // namespace tt
#include <boost/filesystem.hpp>

#include <nlohmann/json.hpp>
#include <fmt/core.h>

#include "Background.h"
#include "Vehicle.h"
#include "VehicleFactory.h"
#include "PathFactory.h"

namespace tt
{

VehicleFactory::VehicleFactory(ResourceManager& resmgr, BackgroundSharedPtr bg)
    : _background{ bg },
      _resources { resmgr }
{
    std::string jsonfile =
        _resources.getFilename(fmt::format("maps/{}.json", _background->mapname()));

    if (!boost::filesystem::exists(jsonfile))
    {
        auto error = fmt::format("file '{}' not found", jsonfile);
        throw std::runtime_error(error);
    }

    std::ifstream file(jsonfile.c_str());
    if (file.is_open()) file >> _json;
}

VehiclePtr VehicleFactory::createVehicle()
{
    assert(_pathFactory);

    auto temptext = *(_resources.load<sf::Texture>("textures/car1.png"));
    auto vehicle = std::make_shared<Vehicle>(temptext, sf::Vector2i{ 77, 41 }, _background);

    auto path = _pathFactory->makeRiboPath();
    path.setRepeating(false);
    
    vehicle->setPath(path);

    return vehicle;
}

} // namespace tt
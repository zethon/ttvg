#include <boost/filesystem.hpp>

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
    if (file.is_open())
    {
        nl::json json;
        file >> json;
        loadVehicles(json);
    }
}

void VehicleFactory::loadVehicles(const nl::json& json)
{
    if (!json.at("zones").is_array()) return;

    for (const auto& item : json["vehicles"]["assets"].items())
    {

    }
}

VehiclePtr VehicleFactory::createVehicle()
{
    assert(_pathFactory);

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(8.0f, 20.0f);

    auto temptext = *(_resources.load<sf::Texture>("textures/car1.png"));
    auto vehicle = std::make_shared<Vehicle>(temptext, sf::Vector2i{ 77, 41 }, _background);

    auto path = _pathFactory->makeRiboPath();
    path.setRepeating(false);
    
    vehicle->setPath(path);
    vehicle->setSpeed(dis(gen));
    vehicle->setScale(1.4f, 1.4f);

    return vehicle;
}

} // namespace tt
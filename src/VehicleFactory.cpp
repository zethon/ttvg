#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/spirit/home/x3.hpp>

#include <fmt/core.h>

#include "Background.h"
#include "Vehicle.h"
#include "VehicleFactory.h"
#include "PathFactory.h"

namespace x3 = boost::spirit::x3;

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
    _resources.cacheSound("sounds/carhorn1.wav");
    _resources.cacheSound("sounds/carhorn2.wav");

    for (const auto& item : json["vehicles"]["assets"].items())
    {
        VehicleInfo info{ item.value().get<ItemInfo>() };

        info.id = info.name;
        info.instinfo = item.value().get<ItemInstanceInfo>();
        info.speed = item.value().at("speed").get<sf::Vector2f>();

        if (item.value().contains("damage"))
        {
            item.value().at("damage").get_to(info.damage);
        }

        const auto soundname = fmt::format("sounds/{}.wav",
            item.value().at("horn").get<std::string>());

        info.soundid = soundname;
        info.texture = _resources.cacheTexture(info.texturefile);
        info.defaultState = "right";

        if (info.texture)
        {
            _vehicles.push_back(info);
        }
    }
}

VehiclePtr VehicleFactory::createVehicle()
{
    assert(_pathFactory);

    static std::random_device rd;
    static std::mt19937 gen(rd());

    auto vinfo = tt::select_randomly(_vehicles);
    auto vehicle = std::make_shared<Vehicle>(*vinfo, _background);

    vehicle->setHornSound(vinfo->soundid);

    std::uniform_real_distribution<float> dis(vinfo->speed.x, vinfo->speed.y);
    vehicle->setSpeed(dis(gen));

    auto path = _pathFactory->makeRiboPath();
    path.setRepeating(false);
    vehicle->setPath(path);

    vehicle->setHighlighted(_highlighted);
    vehicle->setDamage(vinfo->damage);

    vehicle->setScale(*(vinfo->scale));
    
    return vehicle;
}

} // namespace tt

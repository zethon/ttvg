#pragma once

#include <nlohmann/json.hpp>

#include "../Screen.h"

namespace nl = nlohmann;

namespace tt
{

enum class Scenes : std::size_t
{
    TUCSON = 0,
    EUCLIDHOUSE
};

struct Transition
{
    enum class Type
    {
        MANUAL,
        AUTO
    };

    std::string     id;
    std::string     description;
    sf::Vector2f    position;
    bool            enabled;
    Type            type;

    void to_json(nl::json& j, const Transition& t)
    {
        j = nl::json
        {
            {"id", t.id},
            {"description", t.description},
            {"x", t.position.x},
            {"y", t.position.y},
            {"enabled", t.enabled},
            {"type", t.type == Type::MANUAL ? "manual" : "auto"}
        };
    }

    void from_json(const nl::json& j, Transition& t)
    {
        j.at("id").get_to(t.id);
        j.at("description").get_to(t.description);
        j.at("enabled").get_to(t.enabled);
        if (j.at("type").get<std::string>() == "manual")
        {
            t.type = Type::MANUAL;
        }
        else
        {
            t.type = Type::AUTO;
        }

        t.position = sf::Vector2f{ j.at("x").get<float>(), j.at("y").get<float>() };
    }
};

class Scene;
using ScenePtr = std::unique_ptr<Scene>;
using SceneSharedPtr = std::shared_ptr<Scene>;

class Scene : public Screen
{

public:
    Scene(ResourceManager& res, sf::RenderTarget& target);

};

} // namespace tt
#include "TransitionPoint.h"

namespace nl = nlohmann;

namespace tt
{

//void to_json(nl::json& j, const Transition& t)
//{
//    j = nl::json
//    {
//        {"id", t.id},
//        {"description", t.description},
//        {"x", t.position.x},
//        {"y", t.position.y},
//        {"enabled", t.enabled},
//        {"type", t.type == Transition::Type::MANUAL ? "manual" : "auto"},
//        {"newscene", t.newscene}
//    };
//}

void from_json(const nl::json& j, Transition& t)
{
    //j.at("id").get_to(t.id);
    //j.at("description").get_to(t.description);
    j.at("enabled").get_to(t.enabled);
    if (j.at("type").get<std::string>() == "manual")
    {
        t.type = Transition::Type::MANUAL;
    }
    else
    {
        t.type = Transition::Type::AUTO;
    }

    //t.position = sf::Vector2f{ j.at("x").get<float>(), j.at("y").get<float>() };
    j.at("newscene").get_to(t.newscene);
}

} // namesapce
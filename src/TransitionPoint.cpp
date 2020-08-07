#include "TransitionPoint.h"

namespace nl = nlohmann;

namespace tt
{

void from_json(const nl::json& j, Transition& t)
{
    j.at("enabled").get_to(t.enabled);
    if (j.at("type").get<std::string>() == "manual")
    {
        t.type = Transition::Type::MANUAL;
    }
    else
    {
        t.type = Transition::Type::AUTO;
    }

    j.at("newscene").get_to(t.newscene);
}

} // namesapce
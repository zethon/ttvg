#include "TransitionPoint.h"

namespace nl = nlohmann;

namespace tt
{

void from_json(const nl::json& j, Transition& t)
{
    j.at("enabled").get_to(t.enabled);
    j.at("scene").get_to(t.newscene);
    t.selectEvent = j.value("onSelect", {});
}

} // namesapce
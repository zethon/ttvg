#pragma once
#include <vector>
#include <variant>
#include <optional>

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

#include "GameObject.h"

namespace nl = nlohmann;

namespace tt
{

class Item;
using GameObjectPtr = std::shared_ptr<GameObject>;

class Item : public GameObject
{

public:


    using GameObject::GameObject;

private:

};

} // namespace tt

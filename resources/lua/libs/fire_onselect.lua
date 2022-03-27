function fire_onSelect(scene, item)
    Utils.showModal(scene, "Why would you try to pick up a fire you fucking idiot")
    local player = scene:getPlayer()
    player:setHealth(0);
end
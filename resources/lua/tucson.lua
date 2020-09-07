local gDT = nil

function onInit(scene)
    print("onInit call for '" .. scene:name() .. "'")
    gDT = scene:getDescriptionWindow()
end

function treasureChest_onPickup(scene, item)
    local player = scene:getPlayer()
    if player:hasItemByName("key") then
        local vagina = ItemFactory.createItem("magic-space-vagina")
        player:addItem(vagina)
        player:removeItemByName("key")
        scene:removeItem(item)
        gDT:setText("You opened the chest!")
    else
        gDT:setText("You can't open this.")
    end
end
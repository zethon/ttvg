local gDT = nil
local gPlayer = nil

function onInit(scene)
    print("onInit call for '" .. scene:name() .. "'")
    gDT = scene:getDescriptionWindow()
end

function onEnter(scene)
    gPlayer = scene:getPlayer()
end 

function treasureChest_onPickup(scene, item)
    if gPlayer:hasItemByName("key") then
        local vagina = ItemFactory.createItem("magic-space-vagina")
        gPlayer:addItem(vagina)
        gPlayer:removeItemByName("key")
        scene:removeItem(item)
        gDT:setText("You opened the chest!")
    else
        -- gDT:setText("You can't open this.")
        local w = scene:createModal(ModalType.Default)
        w:setText("You cannot open this chest! Find the key!")
        w:exec()
    end
end

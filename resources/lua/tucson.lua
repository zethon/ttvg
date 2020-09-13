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
    local w = scene:createModal(ModalType.Default)
    
    if gPlayer:hasItemByName("key") then
        local vagina = ItemFactory.createItem("magic-space-vagina")
        gPlayer:addItem(vagina)
        gPlayer:removeItemByName("key")
        scene:removeItem(item)
        w:setText("You opened the chest!\nNow you have a Magic Space Vagina!")
    else
        w:setText("You cannot open this right now")
    end

    w:exec()
end

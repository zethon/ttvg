local gDT = nil
local gPlayer = nil

function onInit(scene)
    print("onInit call for '" .. scene:name() .. "'")
    gDT = scene:getDescriptionWindow()
end

function onEnter(scene)
    gPlayer = scene:getPlayer()
end 

-- function onclub_enter_callback(scene)
-- end

-- function onclub_enter(scene)
--     local modal = WindowFactory.create(OPTIONS)
--     modal:addOption("yes")
--     modal:addOption("no")
-- end
-- -- function onclub_enter(scene)
-- --     local modal = scene:createModal()
-- --     modal:doDisplay("This will show up in the first screen")
-- --     modal:doDisplay("This is the second screen that will show up")
-- --     local result = modal:doYesOrNo("Do you like it?")
-- --     if result then
-- --         modal:doDisplay("You selected Yes")
-- --     else
-- --         modal:doDisplay('nooooooooooooooooo!!')
-- --     end
-- -- end

function on_rpg_speak(scene, item)
    local mw = scene:createModal(ModalWindow.Options)
    mw::setText("This is a big mistake")
    mw::show()
end

function treasureChest_onPickup(scene, item)
    if gPlayer:hasItemByName("key") then
        local vagina = ItemFactory.createItem("magic-space-vagina")
        gPlayer:addItem(vagina)
        gPlayer:removeItemByName("key")
        scene:removeItem(item)
        gDT:setText("You opened the chest!")
    else
        gDT:setText("You can't open this.")
    end
end

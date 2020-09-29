
function pole_onSelect(scene, zone)

    local player = scene:getPlayer()

    Utils.showModal(scene, "You are too fat to climb the pole.")

end

function locker_onSelect(scene, zone)

    local player    = scene:getPlayer()
    local m         = scene:createModal(ModalType.Default)

    if player:hasItemByName("locker-key") then
        m:setText(  "You opened the locker.\n" .. 
                    "It is empty.")
    else
        m:setText(  "You cannot open this locker.\n" .. 
                    "It is locked.\n" ..
                    "That's why it's called a locker.")
    end

    m:exec()

end




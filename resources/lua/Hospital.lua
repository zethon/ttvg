function desk_onSelect(scene, zone)
    local player = scene:getPlayer()
    local health = player:getHealth()
    if health == 100 then
        Modal.showDefault(scene, "You appear to be as healthy as a 38\nyear old woman.")
        return
    end

    local balance = player:getBalance()
    if balance < 25 then
        Modal.showDefault(scene, "No welfare here. No money. No healthcare!")
        return
    end

    local mw = scene:createModal(ModalType.Options)
    mw:setText("It will cost $25 to restore your health")
    mw:addOption("Yes! I need to be healthy")
    mw:addOption("No thanks, I'll take my chances")
    mw:exec()
    local result = mw:getSelection()
    if result == nil then return end
    if result == 0 then
        player:setHealth(100)
        player:setBalance(balance - 25)
        Modal.showDefault(scene, "You're all set! Be careful out there!")
    else
        Modal.showDefault(scene, "LOL! Good luck out there hobo!")
    end
end
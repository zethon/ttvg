
function order_onSelect(scene, zone)

    local player    = scene:getPlayer()
    local modal     = scene:createModal(ModalType.Options)

    modal:addOption("You order #5")
    modal:addOption("No order #5")
    modal:exec()

    local result = modal:getSelection()

    if result == nil then return end

    if result == 0 then
        Utils.showModal(scene, "You no afford a #5! I carr a cops!")
    end

    if result == 1 then
        Utils.showModal(scene, "You no pay customer! You go now.")
    end

end



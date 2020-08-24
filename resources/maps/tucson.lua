function stripclub_onEnter()
    Modal.notify("There is a $25 cover fee")
    local response = Modal.yesno("Do you wish to pay $25 to enter?")
    if (response == true) then
        local balance = Player.getBalance()
        if (balance < 25) then
            return false
        else
            Player.setBalance(balance - 25)
            return true
        end
    end

    return true
end

function scene_onEnter()
    HUD.setDescription("Welcome back")
end
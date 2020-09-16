_player = nil

function onEnter(scene)
    _player = scene:getPlayer()
end

function computer_onSelect(scene, zone)
    local w = scene:createModal(ModalType.Options)
    w:setText("Where would you like to go on the Internet today?")
    w:addOption("AMB")
    w:addOption("KiwiFarms")
    w:addOption("Young girls in comprimising positions")
    w:addOption("File a LOLsuit")
    w:addOption("Tommy's favorite porn")
    w:exec()
    local result = w:getSelection()
    if result == nil then return end
    if result == 0 then
        Utils.openUrl("http://www.amb.la")
    elseif result == 1 then 
        Utils.openUrl("http://www.kiwifarms.net")
    elseif result == 2 then 
        Utils.openUrl("https://www.netflix.com/title/81111198")
    elseif result == 3 then
        Utils.openUrl("https://www.jp.pima.gov/Info/CaseTypes/SmallClaims.html")
    elseif result == 4 then
        if Utils.showYesNo(scene, "Are you sure?")
            and Utils.showYesNo(scene, "Are you REALLY REALLY sure?")
            and Utils.showYesNo(scene, "Think about this, are you REALLY\nSUPER POSITIVELY SURE?") 
            and Utils.showYesNo(scene, "It's gonna cost $5.95, do you STILL WANT TO GO?!?")
            then
            _player:setBalance(_player:getBalance() - 5.95)
            Utils.openUrl("https://www.bing.com/images/search?q=homeless+porn&form=HDRSC3&first=1&scenario=ImageBasicHover")        
        end
    end
end


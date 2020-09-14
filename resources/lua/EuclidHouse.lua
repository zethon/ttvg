function onInit(scene)
    print("onInit call for '" .. scene:name() .. "'")
end

function onEnter(scene)
    print("onEnter call for '" .. scene:name() .. "'")
end

function onExit(scene)
    print("onExit call for '" .. scene:name() .. "'")
end

function computer_onSelect(scene, zone)
    local w = scene:createModal(ModalType.Options)
    w:setText("Where would you like to go on the Internet today?")
    w:addOption("AMB")
    w:addOption("KiwiFarms")
    w:addOption("Young girls in comprimising positions")
    w:addOption("File a LOLsuit")
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
    end
end


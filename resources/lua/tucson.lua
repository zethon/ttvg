function onInit(scene)
    print("onInit call for '" .. scene:name() .. "'")
end

function onEnter(scene)
    print("onEnter call for '" .. scene:name() .. "'")
end

function onExit(scene)
    print("onExit call for '" .. scene:name() .. "'")
    local player = scene:getPlayer()
    local health = player:getBalance() - 2
    player:setBalance(health)
end
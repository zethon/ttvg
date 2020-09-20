local _player = nil     -- shortcut to player object, refereshed in every `onEnter` call

function onEnter(scene)
    _player = scene:getPlayer()
end 

function onTileUpdate(scene, x, y)
    Log.debug("I'm on " .. x .. "," .. y)
end

function treasureChest_onPickup(scene, item)
    local w = scene:createModal(ModalType.Default)
    
    if _player:hasItemByName("key") then
        local vagina = ItemFactory.createItem("magic-space-vagina")
        _player:addItem(vagina)
        _player:removeItemByName("key")
        scene:removeItem(item)
        w:setText("You opened the chest!\nNow you have a Magic Space Vagina!")
    else
        w:setText("You cannot open this right now")
    end

    w:exec()
end

local firstTimeSevan = true
function sevan_select(scene, item)
    if firstTimeSevan then
        local w = scene:createModal(ModalType.Messages)
        w:pushMessage("Oh hi Tommy! How does your hobohole feel?")
        w:pushMessage("I know I was little rough on you\n that night\nLOL")
        w:pushMessage("Maybe you can help me. Do you know where I can\nfind some weed?")
        w:pushMessage("If you bring me a bag weed I'll pay you $20")
        w:pushMessage("And if you're feeling horny, let me know!\nI miss that dirty hole!")
        w:exec()
        firstTimeSevan = false
    else
        if not (_player:hasItemByName("bag-of-weed")) then
            Utils.showModal(scene, "Come back when you have some weed.")
        else
            Utils.showModal(scene, "Hey thanks man! Here's another $20")
            _player:removeItemByName('bag-of-weed')
            _player:setBalance(_player:getBalance() + 20)
        end
    end
end

function courtHouse_onEnter(scene)
    if _player:hasItemByName('bag-of-weed') or  _player:hasItemByName('bag-of-crack') then
        local res = Utils.showYesNo(scene, "You might be carrying some things that don't\nbelong in a courthouse!\nDo you still want to enter?")
        if res == true then
            while _player:hasItemByName('bag-of-weed') do
                _player:removeItemByName('bag-of-weed')
                _player:setBalance(_player:getBalance() - 50)
            end
            while _player:hasItemByName('bag-of-crack') do
                _player:removeItemByName('bag-of-crack')
                _player:setBalance(_player:getBalance() - 50)
            end
            Utils.showModal(scene, "bringing drugs into a courthouse is illegal,\nyou fucking idiot")
        end
    end
end

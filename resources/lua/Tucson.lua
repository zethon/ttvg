require "libs/fire_onselect.lua"

local _player = nil     -- shortcut to player object, refereshed in every `onEnter` call

function onEnter(scene)
    _player = scene:getPlayer()
end 

function treasureChest_onSelect(scene, item)
    local w = scene:createModal(ModalType.Default)
    
    if _player:hasItemByName("key") then
        -- create the 'magic-space-vagina' and manually remove
        -- the key from the player's inventory. returning 
        -- true will tell the engine to remove 'treasure-chest'
        -- (aka `item`) from the scene
        local vagina = ItemFactory.createItem("magic-space-vagina")
        _player:addItem(vagina)
        _player:removeItemByName("key")
        Utils.showModal(scene, "You opened the chest!\nNow you have a Magic Space Vagina!")
        return true
    else
        Utils.showModal(scene, "You cannot open this right now")
    end

    return false
end

local firstTimeSevan = true
function sevan_select(scene, item)
    if firstTimeSevan then
        local w = scene:createModal(ModalType.Messages)
        w:pushMessage("Oh hi Tommy! How does your hobohole feel?")
        w:pushMessage("I know I was little rough on you\n that night\nLOL")
        w:pushMessage("Maybe you can help me. Do you know where I can\nfind some weed?")
        w:pushMessage("If you bring me a bag of weed I'll pay you $20")
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
            item:interruptState("flip")
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


--
-- Fill an empty gas can at a gas station.
--
function gasStation_onSelect(scene)

    if _player:hasItemByName("empty-gas-can") then
        local gasCan = ItemFactory.createItem("gas-can")
        _player:addItem(gasCan)
        _player:removeItemByName("empty-gas-can")
        Utils.showModal(scene, "You've got gas!")
    else
        Utils.showModal(scene, "You need something to put the gas in.")
    end
end


--
-- Talk to Chief Covfefe
--
local truckFixed = false
function covfefe_select(scene, item)

    if truckFixed then
        
        Utils.showModal(
            scene, 
            "The Chief has put you on ignore.")

        return
    end

    if not (_player:hasItemByName("empty-gas-can")) then
        Utils.showModal(
            scene, 
            "Wetin dey Mzungu.\n" ..
            "Why you do fucking up my trucks?\n" ..
            "You take this empty gas can.\n" ..
            "I say you can borrow, is not a gift!")

            local emptyGasCan = ItemFactory.createItem("empty-gas-can")
            _player:addItem(emptyGasCan)

    else

        Utils.showModal(
            scene, 
            "Hey Mzungu hobo guy man.\n" ..
            "You fatass go fix a trucks.\n" ..
            "I say you can borrow, is not a gift!")

    end

end


--
-- Fix Covfefe's truck
--
local truckState = 0
local truckStateMap = {
    [0] = "This truck is fucked up.",
    [1] = "Probably needs more gas.",
    [2] = "I'm sure more gas will fix it.",
    [3] = "You really fucked this thing up."
}
function covfefeTruck_select(scene, item)

    --
    -- This if branch completes the quest.
    --
    if (_player:hasItemByName("zyklon-b") and truckState == 3) then

        local w = scene:createModal(ModalType.Messages)

        w:pushMessage(
            "You put in some gift gas.")

        w:pushMessage(
            "A voice rings out\n" ..
            "from beyond the ether.")

        w:pushMessage(
            "Wetin dey m'hobo!\n" ..
            "You murtherforker do fuck my truck\n" ..
            "for a last time!\n" ..
            "May this curse be upon you 1000 times!" )

        w:exec()

        _player:removeItemByName("zyklon-b")
        local zuluCurse = ItemFactory.createItem("curse-of-the-zulu")
        _player:addItem(zuluCurse)
        
        truckFixed = true;

        -- return true to remove the item (truck) from the scene

        return true
    end

    --
    -- Add more gas.
    --
    if (_player:hasItemByName("gas-can")) then

        Utils.showModal(scene, "You put in some gas.")
        _player:removeItemByName("gas-can")

        if (truckState < 3) then
            truckState = truckState + 1; 
        end

    end

    local msg = truckStateMap[truckState]
    Utils.showModal(scene, msg)

end

--
-- Talk to John Belushi
--
function belushi_select(scene, item)

    local w = scene:createModal(ModalType.Messages)

    w:pushMessage(
        "I've been doing \n" ..
        "a lot of cocaine lately. \n")

    w:pushMessage(
        "I could sure use a fish tank.\n" )

    w:exec()

end




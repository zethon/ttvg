local gDT = nil
local gPlayer = nil

function onInit(scene)
    print("onInit call for '" .. scene:name() .. "'")
    gDT = scene:getDescriptionWindow()
end

function onEnter(scene)
    gPlayer = scene:getPlayer()
end 

function treasureChest_onPickup(scene, item)
    local w = scene:createModal(ModalType.Default)
    
    if gPlayer:hasItemByName("key") then
        local vagina = ItemFactory.createItem("magic-space-vagina")
        gPlayer:addItem(vagina)
        gPlayer:removeItemByName("key")
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
        if not (gPlayer:hasItemByName("bag-of-weed")) then
            Utils.showModal(scene, "Come back when you have some weed.")
        else
            Utils.showModal(scene, "Hey thanks man! Here's another $20")
            gPlayer:removeItemByName('bag-of-weed')
            gPlayer:setBalance(gPlayer:getBalance() + 20)
        end
    end
end

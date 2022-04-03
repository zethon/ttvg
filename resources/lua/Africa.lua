-- require "libs/fire_onselect.lua"

--
-- shortcut to player object, refereshed in every `onEnter` call
--
local _player = nil     

function onEnter(scene)
    _player = scene:getPlayer()
end 

--
-- Talk with starving African child
--
function starvingChild_select(scene, item)

    local w = scene:createModal(ModalType.Messages)
    w:pushMessage("Sir, I am so hungry.")
    w:pushMessage("I'M NOT A SIR I'M A MA'AM!")
    w:exec() 

end


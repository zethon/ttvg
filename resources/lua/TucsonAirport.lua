--
-- shortcut to player object, refereshed in every `onEnter` call
--
local _player = nil     

function onEnter(scene)
    _player = scene:getPlayer()
end 

--
-- Airport
--
function airport_onSelect(scene, item)

    if not (_player:hasItemByName("ticket-to-africa")) then

        Utils.showModal(
            scene, 
            "You can't fly without a ticket.")
        
        return;

    end
 
    Utils.showModal(
            scene, 
            "Let's go to Africa!")
     

end



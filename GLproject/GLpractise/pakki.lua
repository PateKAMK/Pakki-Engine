configure = {
    ScreenX = 1200,
    ScreenY = 800,
    WorldX = 0,
    WorldY = 0,
    WorldWidht = 500,
    WorldHeight = 500,
}


function initPakki()
     print("pakki inited from lua side")
     local x = 10;
end

currentTime = 0
function updatePakki(dt)
    currentTime = currentTime + dt
    print("updating pakki", currentTime)
    local x = 10;
    return 1
end
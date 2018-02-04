configure = {
    ScreenX = 1200,
    ScreenY = 800
}


function initPakki()
     print("pakki inited from lua side")
     local x = 10;
end

currentTime = 0
function updatePakki(dt)
    currentTime = currentTime + dt
    print("updating pakki", currentTime)
    print(Pakki.MouseX)
    print(Pakki.MouseY)
    local x = 10;
    return Pakki.success
end
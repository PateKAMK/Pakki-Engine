ffi.cdef[[
    void set_camera(float x,float y,float scale);
    struct Color
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };
   
    struct vec4
	{
		float	x;
		float	y;
		float	z;
		float	w;
    };
     
    struct vec2
	{
		float	x;
		float	y;
    };
    
    struct drawdata
    {
        vec4		uv;
        //Color		color;
        uint32_t	spriteid;
        int			level;
    };
    /*
    struct object
	{
		vec2		pos;
		vec2		dim;
		uint32_t	s;
		uint32_t	i;
		drawdata*	drawPtr;
    };
    object* get_object();*/
]]
print("FFI done correctly")

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
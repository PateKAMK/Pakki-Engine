local ffi = require("ffi")
ffi.cdef[[
    void set_camera(float x,float y,float scale);
    typedef struct
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    } Color;
   
    typedef struct 
	{
		float	x;
		float	y;
		float	z;
		float	w;
    } vec4;
     
    typedef struct 
	{
		float	x;
		float	y;
    } vec2;
    
    typedef struct 
    {
        vec4		uv;
        Color		color;
        uint32_t	spriteid;
        int			level;
    } drawdata;
    typedef struct 
	{
		vec2		pos;
		vec2		dim;
		uint32_t	s;
		uint32_t	i;
		drawdata*	drawPtr;
    } object;
    
    object* get_object();
    void dispose_object(object* obj);
    void dispose_sprite(drawdata* spr);
    void set_camera(float x,float y,float scale);
    bool is_key_down(const char key[]);
    bool is_key_activated(const char key[]);
    void hello_world();
    void draw_box(float x,float y,float w, float h,float angle);
    void update_objects();
    drawdata* load_picture(const char* path);
]]
print("FFI done correctly")




configure = {
    ScreenX = 1200,
    ScreenY = 800,
    WorldX = 0,
    WorldY = 0,
    WorldWidht = 500,
    WorldHeight = 350,
}


function initPakki()
    print("pakki inited from lua side")
    local x = 10;
end

currentTime = 0
newX = 0
function updatePakki(dt)
    --print("here")
    -- ffi.C.hello_world()
    -- ffi.C.is_key_down('a');
    currentTime = currentTime + dt
    if(ffi.C.is_key_activated("w"))then
        local nobj = ffi.C.get_object();
        print("ree")
        nobj[0].pos.x = newX
        newX = newX + 40
        nobj[0].pos.y = 0
        nobj[0].dim.x = 40
        nobj[0].dim.y = 40
        nobj[0].s = 4 -- moving
        nobj[0].drawPtr = ffi.C.load_picture("laatikko.jpg")
    end
    if(ffi.C.is_key_down("w")) then
        ffi.C.draw_box(0,0,50,50,0.5) 
    end
    if(ffi.C.is_key_activated("a"))then
        print("a pressed" ,currentTime)
    end
    if(ffi.C.is_key_activated("s"))then
        print("s pressed" ,currentTime)
    end
    if(ffi.C.is_key_activated("d"))then
        print("d pressed" ,currentTime)
    end
    --print("updating pakki", currentTime)
    local x = 10;
    ffi.C.update_objects();
    return 1
end
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
        int32_t	    level;
    } drawdata;
    typedef struct 
	{
		vec2		pos;
		vec2		dim;
		uint32_t	s;
		uint32_t	i;
		drawdata*	drawPtr;
    } object;
    
    object* get_object_moving(float x,float y,float w, float h);
    object* get_object_static(float x,float y,float w, float h);
    void dispose_object(object* obj);
    void dispose_sprite(drawdata* spr);
    void set_camera(float x,float y,float scale);
    bool is_key_down(const char key[]);
    bool is_key_activated(const char key[]);
    void hello_world();
    void draw_box(float x,float y,float w, float h,float angle);
    void update_objects();
    drawdata* load_picture(const char* path);
    void draw_line(float x1,float y1,float x2,float y2);
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
numCalls = 0
objects = {}
function updatePakki(dt)
    --print("here")
    -- ffi.C.hello_world()
    -- ffi.C.is_key_down('a');
    ffi.C.draw_line(0,0,400,400)
    currentTime = currentTime + dt
    if(ffi.C.is_key_activated("w"))then
        numCalls = numCalls + 1
        objects[numCalls] = ffi.C.get_object_static(newX,0,40,40) 
        print("REEEEEEEEEE")
        objects[numCalls][0].i = numCalls
        newX = newX + 40
        objects[numCalls][0].drawPtr = ffi.C.load_picture("laatikko.jpg")
        objects[numCalls][0].drawPtr[0].level = numCalls
        print(numCalls)
    end
    if(ffi.C.is_key_down("w")) then
    --    ffi.C.draw_box(0,0,50,50,0.5) 
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
    print(#objects)
    for i = 1,#objects do
        ffi.C.draw_box(objects[i][0].pos.x,objects[i][0].pos.y,objects[i][0].dim.x,objects[i][0].dim.y,0)
    end
    return 1
end
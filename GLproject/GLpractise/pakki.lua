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
    vec2 getMousePos();
]]
print("FFI done correctly")




configure = {
    ScreenX = 1200,
    ScreenY = 800,
    WorldX = 100,
    WorldY = 100,
    WorldWidht = 500,
    WorldHeight = 350,
}

objects = {}
player = 0;
numObjs = 0
camposX = 0;
camposY = 0;
function initPakki()
    print("pakki inited from lua side")

    objects[numObjs] = {}
    objects[numObjs].cdata = ffi.C.get_object_moving(0,0,40,40)
    objects[numObjs].cdata[0].i = numObjs
    objects[numObjs].cdata[0].drawPtr = ffi.C.load_picture("laatikko.jpg")
    player = numObjs;
    numObjs = numObjs + 1

    print("here")
    print("pakki inited from lua side")
    for i = 1,10 do
    objects[numObjs] = {}
    objects[numObjs].cdata = ffi.C.get_object_static(-100 + i * 80,-80,50,50)
    objects[numObjs].cdata[0].i = numObjs
    objects[numObjs].cdata[0].drawPtr = ffi.C.load_picture("laatikko.jpg")
    numObjs = numObjs + 1
    end
    
    print("pakki inited from lua side")

end

currentTime = 0
newX = 0
function updatePakki(dt)

    local mpos = ffi.C.getMousePos();
   -- print(mpos.x, mpos.y)
    currentTime = currentTime + dt
    if(ffi.C.is_key_down("w"))then
        camposY = camposY + 2;
        ffi.C.set_camera(camposX,camposY,1);
       print("w pressed")
    end

    if(ffi.C.is_key_down("a"))then
        camposX = camposX - 2;
        ffi.C.set_camera(camposX,camposY,1);
        print("a pressed" ,currentTime)
    end
    if(ffi.C.is_key_down("s"))then
        camposY = camposY - 2;
        ffi.C.set_camera(camposX,camposY,1);
        print("s pressed" ,currentTime)
    end
    if(ffi.C.is_key_down("d"))then
        camposX = camposX + 2;
        ffi.C.set_camera(camposX,camposY,1);
        print("d pressed" ,currentTime)
    end
    ffi.C.update_objects();
    for i = 1,#objects do
       ffi.C.draw_box(objects[i].cdata[0].pos.x,objects[i].cdata[0].pos.y,objects[i].cdata[0].dim.x,objects[i].cdata[0].dim.y,0)
    end
    return 1
end



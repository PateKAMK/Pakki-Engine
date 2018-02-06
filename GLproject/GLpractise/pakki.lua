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
    drawdata* get_sprite();
    void dispose_sprite(drawdata* spr);
    void set_camera(float x,float y,float scale);
    bool is_key_down(unsigned char key);
    bool is_key_activated(unsigned char key);
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
    print("here")
    currentTime = currentTime + dt
    if(ffi.C.is_key_down("w"))then
        print("w pressed" ,currentTime)
    end
    --print("updating pakki", currentTime)
    local x = 10;
    return 1
end
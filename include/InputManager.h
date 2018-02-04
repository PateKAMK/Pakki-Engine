#pragma once
#include <glm\glm\glm.hpp>
struct mouse
{
	bool mouse1;
	bool mouse2;
};
enum class mouseButton
{
	RIGHT,
	LEFT
};


struct input
{
	bool*		keys;
	bool*		lastKeys;
	bool		update;
	float		xMouse;
	float		yMouse;
	float		xWorld;
	float		yWorld;
	uint32_t	size;
	mouse		mouseButtons{ 0 };
	mouse		lastbuttons{ 0 };
	bool		disableClicks;
};


// todo ota singleton pois
void init_inputs(input* in);
void update_keys(input* in);
void dispose_inputs(input* in);
bool is_key_pressed(input* in,const uint32_t key);
bool is_key_active(input* in,const uint32_t key);
void set_key(input* in,const uint32_t key,bool state);
void set_mouse_button(input* in,const uint32_t mouse, const uint32_t action);
void set_mouse_position(input* in,const double x, const double y);
void get_mouse_pos(input* in,float* x, float* y);
bool is_mouse_pressed(input* in,mouseButton button);
bool is_mouse_active(input* in,mouseButton button);

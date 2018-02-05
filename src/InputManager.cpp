#include "InputManager.h"
#include <glfw3.h>
#include <iostream>
#include <new>
#include <cstring>



/*
static input* in = NULL;

#define SINGLETON if (!in) in = (input*)malloc(sizeof(input))*/ 
void init_inputs(input* in)
{
	memset(in, 0, sizeof(input));
	in->keys = (bool*)malloc(sizeof(bool) * GLFW_KEY_LAST);
	memset(in->keys, 0, sizeof(bool)*GLFW_KEY_LAST);
	in->lastKeys = (bool*)malloc(sizeof(bool) * GLFW_KEY_LAST);
	memset(in->lastKeys, 0, sizeof(bool)*GLFW_KEY_LAST);
}
void update_keys(input* in)
{
	
	if (in->update)
	{
		in->keys;
		std::memcpy(in->lastKeys, in->keys, sizeof(bool)*GLFW_KEY_LAST);
		in->update = false;
	}
	in->lastbuttons = in->mouseButtons;
	in->disableClicks = false;
}
void dispose_inputs(input* in)
{
	free(in->keys);
	free(in->lastKeys);
	
}
bool is_key_pressed(input* in,const uint32_t key)
{
	return in->keys[key] && !(in->lastKeys[key]);
}
bool is_key_active(input* in,const uint32_t key)
{
	return in->keys[key];
}
void set_key(input* in,const uint32_t key,bool state)
{
	in->keys[key] = state;
	in->update = true;
}
void set_mouse_button(input* in,const uint32_t mouse,const uint32_t action)
{
	if(mouse == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
		{
			in->mouseButtons.mouse1 = true;
		}
		else if(action == GLFW_RELEASE)
		{
			in->mouseButtons.mouse1 = false;
		}
	}
	else if(mouse == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			in->mouseButtons.mouse2 = true;
		}
		else if (action == GLFW_RELEASE)
		{
			in->mouseButtons.mouse2 = false;
		}
	}
}
void set_mouse_position(input* in,const double x, const double y)
{
	in->xMouse = x;
	in->yMouse = y;
}
void get_mouse_pos(input* in,float* x, float* y)
{
	*x = in->xMouse;
	*y = in->yMouse;
}
bool is_mouse_pressed(input* in,mouseButton button)
{
	if(button == mouseButton::RIGHT)
	{
		return in->mouseButtons.mouse2 && !in->lastbuttons.mouse2;
	}
	else if(button == mouseButton::LEFT)
	{
		return in->mouseButtons.mouse1 && !in->lastbuttons.mouse1;
	}
	return false;
}
bool is_mouse_active(input* in,mouseButton button)
{
	if (button == mouseButton::RIGHT)
	{
		return in->mouseButtons.mouse2;
	}
	else if (button == mouseButton::LEFT)
	{
		return in->mouseButtons.mouse1;
	}
	return false;
}
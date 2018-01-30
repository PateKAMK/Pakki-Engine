#include "InputManager.h"
#include <glfw3.h>
#include <iostream>
#include <new>
#include <cstring>
//
//InputManager::InputManager() : keys(nullptr), size(0), _mouseworldpos(0), update(false), updateMouse(false), disableClickForUI(false)
//{
//	keys = new (std::nothrow) bool[GLFW_KEY_LAST];
//	last = new (std::nothrow) bool[GLFW_KEY_LAST];
//
//	assert(keys != nullptr || last != nullptr);
//
//	memset(keys, false, GLFW_KEY_LAST * sizeof(bool));
//	memset(last, false, GLFW_KEY_LAST * sizeof(bool));
//
//	size = GLFW_KEY_LAST;
//
//}
//InputManager::~InputManager()
//{
//	delete[] keys;
//	delete[] last;
//}
//
//bool InputManager::
//keyState(unsigned int glfw_key) const
//{
//	if (glfw_key > size - 1 || size < 0)
//	{
//		std::cout << "invalid key" << std::endl;
//		return false;
//	}
//	return keys[glfw_key];
//}
//void InputManager::setKey(unsigned int glfw_key, bool state)
//{
//	if (glfw_key > size - 1 || size < 0)
//	{
//		std::cout << " INVALID KEYSET" << std::endl;
//		return;
//	}
//	keys[glfw_key] = state;
//	update = true;
//}
//void InputManager::
//setMousePos(const double &mouseX, const double &mouseY)
//{
//	_mouseCoords.x = (float)mouseX;
//	_mouseCoords.y = (float)mouseY;
//	//std::cout << mouseX << " " << mouseY << std::endl;
//}
//void InputManager::
//setMouseButton(unsigned int mousebutton, unsigned int action)
//{
//	if (mousebutton == GLFW_MOUSE_BUTTON_RIGHT)
//	{
//		if (action == GLFW_PRESS)
//		{
//			mousebuttons.mouse2 = true;
//		}
//		else if (action == GLFW_RELEASE)
//		{
//			mousebuttons.mouse2 = false;
//		}
//	}
//	else if (mousebutton == GLFW_MOUSE_BUTTON_LEFT)
//	{
//		if (action == GLFW_PRESS)
//		{
//			mousebuttons.mouse1 = true;
//		}
//		else if (action == GLFW_RELEASE)
//		{
//			mousebuttons.mouse1 = false;
//		}
//	}
//	updateMouse = true;
//}
//void  InputManager::
//updateKeys(const glm::vec2& camPos,float cameraScale,int screenHeight,int screenWeight)
//{
//	disableClickForUI = false;
//	if (update)
//	{
//		std::memcpy(last, keys, sizeof(bool)*size);
//		update = false;
//	}
//	if (updateMouse)
//	{
//		lastFrameMousebuttons.mouse1 = mousebuttons.mouse1;
//		lastFrameMousebuttons.mouse2 = mousebuttons.mouse2;
//	}
//	//calculate worldposition for mouse
//	_mouseworldpos.y = screenHeight - _mouseCoords.y;
//	_mouseworldpos -= glm::vec2(screenWeight /2,screenHeight/2);
//	_mouseworldpos /= cameraScale;
//	_mouseworldpos += camPos;
//}
//bool InputManager::
//KeyPressed(unsigned int glfw_key)const
//{
//	if (glfw_key > size - 1 || size < 0)
//	{
//		std::cout << "invalid key" << std::endl;
//		return false;
//	}
//	return keys[glfw_key] == true && last[glfw_key] == false;
//}
//
//bool InputManager::
//getMouseButtonState(mouseButton button)
//{
//	if (disableClickForUI)return false;
//	switch (button)
//	{
//	case mouseButton::LEFT:
//	{
//		return mousebuttons.mouse1;
//	}
//	case mouseButton::RIGHT:
//	{
//		return mousebuttons.mouse2;
//	}
//	default:
//	{
//		std::cout << "unreconised mouse button" << std::endl;
//		return false;
//	}
//	}
//}
//bool InputManager::
//MouseReleased(mouseButton button)
//{
//	if (disableClickForUI)return false;
//	if (button == mouseButton::LEFT)
//	{
//		return !mousebuttons.mouse1 && lastFrameMousebuttons.mouse1;
//	}
//	if (button == mouseButton::RIGHT)
//	{
//		return !mousebuttons.mouse2 && lastFrameMousebuttons.mouse2;
//	}
//	return false;
//}
//
//bool InputManager::
//MousePressed(mouseButton button)
//{
//	if (disableClickForUI)return false;
//	if (button == mouseButton::LEFT)
//	{
//		return mousebuttons.mouse1 && !lastFrameMousebuttons.mouse1;
//	}
//	if (button == mouseButton::RIGHT)
//	{
//		return mousebuttons.mouse2 && !lastFrameMousebuttons.mouse2;
//	}
//	return false;
//}

struct input
{
	bool*		keys;
	bool*		lastKeys;
	bool		update;
	float		xMouse;
	float		yMouse;
	uint32_t	size;
	mouse		mouseButtons{ 0 };
	mouse		lastbuttons{ 0 };
	bool		disableClicks;
};


static input* in = NULL;

#define SINGLETON if (!::in) ::in = (input*)malloc(sizeof(input)) 
void init_inputs()
{
	SINGLETON;
	memset(::in, 0, sizeof(input));
	::in->keys = (bool*)malloc(sizeof(bool) * GLFW_KEY_LAST);
	memset(::in->keys, 0, sizeof(bool)*GLFW_KEY_LAST);
	::in->lastKeys = (bool*)malloc(sizeof(bool) * GLFW_KEY_LAST);
	memset(::in->lastKeys, 0, sizeof(bool)*GLFW_KEY_LAST);
}
void update_keys()
{
	if (::in->update)
	{
		::in->keys;
		std::memcpy(::in->lastKeys, ::in->keys, sizeof(bool)*GLFW_KEY_LAST);
		in->update = false;
	}
	::in->lastbuttons = ::in->mouseButtons;
	::in->disableClicks = false;
}
void dispose_inputs()
{
	if (!in)return;
	free(::in->keys);
	free(::in->lastKeys);
	free(in);
	in = NULL;
}
bool is_key_pressed(const uint32_t key)
{
	return ::in->keys[key] && !(::in->lastKeys[key]);
}
bool is_key_active(const uint32_t key)
{
	return ::in->keys[key];
}
void set_key(const uint32_t key,bool state)
{
	::in->keys[key] = state;
	::in->update = true;
}
void set_mouse_button(const uint32_t mouse,const uint32_t action)
{
	if(mouse == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
		{
			::in->mouseButtons.mouse1 = true;
		}
		else if(action == GLFW_RELEASE)
		{
			::in->mouseButtons.mouse1 = false;
		}
	}
	else if(mouse == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			::in->mouseButtons.mouse2 = true;
		}
		else if (action == GLFW_RELEASE)
		{
			::in->mouseButtons.mouse2 = false;
		}
	}
}
void set_mouse_position(const double x, const double y)
{
	::in->xMouse = x;
	::in->yMouse = y;
}
void get_mouse_pos(float* x, float* y)
{
	*x = ::in->xMouse;
	*y = ::in->yMouse;
}
bool is_mouse_pressed(mouseButton button)
{
	if(button == mouseButton::RIGHT)
	{
		return ::in->mouseButtons.mouse2 && !::in->lastbuttons.mouse2;
	}
	else if(button == mouseButton::LEFT)
	{
		return ::in->mouseButtons.mouse1 && !::in->lastbuttons.mouse1;
	}
	return false;
}
bool is_mouse_active(mouseButton button)
{
	if (button == mouseButton::RIGHT)
	{
		return ::in->mouseButtons.mouse2;
	}
	else if (button == mouseButton::LEFT)
	{
		return ::in->mouseButtons.mouse1;
	}
	return false;
}
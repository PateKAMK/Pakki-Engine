#include "InputManager.h"
#include <glfw3.h>
#include <iostream>
#include <new>
#include <cstring>

InputManager::InputManager() : keys(nullptr), size(0), _mouseworldpos(0), update(false), updateMouse(false), disableClickForUI(false)
{
	keys = new (std::nothrow) bool[GLFW_KEY_LAST];
	last = new (std::nothrow) bool[GLFW_KEY_LAST];

	assert(keys != nullptr || last != nullptr);

	memset(keys, false, GLFW_KEY_LAST * sizeof(bool));
	memset(last, false, GLFW_KEY_LAST * sizeof(bool));

	size = GLFW_KEY_LAST;

}
InputManager::~InputManager()
{
	delete[] keys;
	delete[] last;
}

bool InputManager::
keyState(unsigned int glfw_key) const
{
	if (glfw_key > size - 1 || size < 0)
	{
		std::cout << "invalid key" << std::endl;
		return false;
	}
	return keys[glfw_key];
}
void InputManager::setKey(unsigned int glfw_key, bool state)
{
	if (glfw_key > size - 1 || size < 0)
	{
		std::cout << " INVALID KEYSET" << std::endl;
		return;
	}
	keys[glfw_key] = state;
	update = true;
}
void InputManager::
setMousePos(const double &mouseX, const double &mouseY)
{
	_mouseCoords.x = (float)mouseX;
	_mouseCoords.y = (float)mouseY;

	//std::cout << mouseX << " " << mouseY << std::endl;
}
void InputManager::
setMouseButton(unsigned int mousebutton, unsigned int action)
{
	if (mousebutton == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
		{
			mousebuttons.mouse2 = true;
		}
		else if (action == GLFW_RELEASE)
		{
			mousebuttons.mouse2 = false;
		}
	}
	else if (mousebutton == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			mousebuttons.mouse1 = true;
		}
		else if (action == GLFW_RELEASE)
		{
			mousebuttons.mouse1 = false;
		}
	}
	updateMouse = true;
}
void  InputManager::
updateKeys(const glm::vec2& camPos,float cameraScale,int screenHeight,int screenWeight)
{
	disableClickForUI = false;
	if (update)
	{
		std::memcpy(last, keys, sizeof(bool)*size);
		update = false;
	}
	if (updateMouse)
	{
		lastFrameMousebuttons.mouse1 = mousebuttons.mouse1;
		lastFrameMousebuttons.mouse2 = mousebuttons.mouse2;
	}
	//calculate worldposition for mouse
	_mouseworldpos.y = screenHeight - _mouseCoords.y;
	_mouseworldpos -= glm::vec2(screenWeight /2,screenHeight/2);
	_mouseworldpos /= cameraScale;
	_mouseworldpos += camPos;
}
bool InputManager::
KeyPressed(unsigned int glfw_key)const
{
	if (glfw_key > size - 1 || size < 0)
	{
		std::cout << "invalid key" << std::endl;
		return false;
	}
	return keys[glfw_key] == true && last[glfw_key] == false;
}

bool InputManager::
getMouseButtonState(mouseButton button)
{
	if (disableClickForUI)return false;
	switch (button)
	{
	case mouseButton::LEFT:
	{
		return mousebuttons.mouse1;
	}
	case mouseButton::RIGHT:
	{
		return mousebuttons.mouse2;
	}
	default:
	{
		std::cout << "unreconised mouse button" << std::endl;
		return false;
	}
	}
}
bool InputManager::
MouseReleased(mouseButton button)
{
	if (disableClickForUI)return false;
	if (button == mouseButton::LEFT)
	{
		return !mousebuttons.mouse1 && lastFrameMousebuttons.mouse1;
	}
	if (button == mouseButton::RIGHT)
	{
		return !mousebuttons.mouse2 && lastFrameMousebuttons.mouse2;
	}
	return false;
}

bool InputManager::
MousePressed(mouseButton button)
{
	if (disableClickForUI)return false;
	if (button == mouseButton::LEFT)
	{
		return mousebuttons.mouse1 && !lastFrameMousebuttons.mouse1;
	}
	if (button == mouseButton::RIGHT)
	{
		return mousebuttons.mouse2 && !lastFrameMousebuttons.mouse2;
	}
	return false;
}


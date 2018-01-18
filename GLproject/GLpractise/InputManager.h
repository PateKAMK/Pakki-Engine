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
class InputManager
{
public:
	InputManager();
	~InputManager();
	bool keyState(unsigned int glfw_key)const;
	glm::vec2 getMousePos() { return _mouseCoords; }
	void setKey(unsigned int glfw_key, bool state);
	void setMousePos(const double &mouseX, const double &mouseY);
	void setMouseButton(unsigned int mousebutton, unsigned int action);
	void disableClicks() { disableClickForUI = true; }
	bool getMouseAble()const { return !disableClickForUI; }
	bool getMouseButtonState(mouseButton button);
	bool KeyPressed(unsigned int glfw_key)const;
	bool MousePressed(mouseButton button);
	bool MouseReleased(mouseButton button);

	void updateKeys(const glm::vec2& camPos, float cameraScale, int screenHeight, int screenWeight);
private:
	bool update;
	bool updateMouse;
	glm::vec2 _mouseCoords;
	glm::vec2 _mouseworldpos;
	bool *keys; // current frame
	bool *last; // last frame
	int size;
	mouse mousebuttons{ false,false };
	mouse lastFrameMousebuttons{ false,false };
	bool disableClickForUI;
};

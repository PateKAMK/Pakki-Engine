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




void init_inputs();
void update_keys();
void dispose_inputs();
bool is_key_pressed(const uint32_t key);
bool is_key_active(const uint32_t key);
void set_key(const uint32_t key,bool state);
void set_mouse_button(const uint32_t mouse, const uint32_t action);
void set_mouse_position(const double x, const double y);
void get_mouse_pos(float* x, float* y);
bool is_mouse_pressed(mouseButton button);
bool is_mouse_active(mouseButton button);


//class InputManager
//{
//public:
//	void operator = (const InputManager&) = delete;
//	bool keyState(unsigned int glfw_key)const;
//	glm::vec2 getMousePos() { return _mouseCoords; }
//	void setKey(unsigned int glfw_key, bool state);
//	void setMousePos(const double &mouseX, const double &mouseY);
//	void setMouseButton(unsigned int mousebutton, unsigned int action);
//	void disableClicks() { disableClickForUI = true; }
//	bool getMouseAble()const { return !disableClickForUI; }
//	bool getMouseButtonState(mouseButton button);
//	bool KeyPressed(unsigned int glfw_key)const;
//	bool MousePressed(mouseButton button);
//	bool MouseReleased(mouseButton button);
//	void updateKeys(const glm::vec2& camPos, float cameraScale, int screenHeight, int screenWeight);
//	static InputManager* single;
//
//private:
//	InputManager();
//	~InputManager();
//	bool update;
//	bool updateMouse;
//	glm::vec2 _mouseCoords;
//	glm::vec2 _mouseworldpos;
//	bool *keys; // current frame
//	bool *last; // last frame
//	int size;
//	mouse mousebuttons{ false,false };
//	mouse lastFrameMousebuttons{ false,false };
//	bool disableClickForUI;
//};
//

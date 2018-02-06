#include "engine.h"
#include "window.h"
#include <glad/glad.h>
#include <glfw3.h>
#include <cstdio>
#include "projectDefinitions.h"
#include "Vertex.h"
#include "freetype.h"
#include <pakki_utilities.h>
#include <soundsystem.h>
#include <pakki.h>
#include <objectManager.h>
#define SOL_CHECK_ARGUMENTS 1
#include <sol.hpp>
#define FILEID 10
static input inputs;
enum ret
{
	pass,
	fail
};
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

static bool esc = false;
static keys* k;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(action == GLFW_PRESS)
	{
		set_key(&inputs,key, true);
	}
	else if(action == GLFW_RELEASE)
	{
		set_key(&inputs,key, false);
	}
}
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	set_mouse_position(&inputs, xpos, ypos);
}
bool button = false;
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			::button = true;
		}
		else if (action == GLFW_RELEASE)
		{
			::button = false;
		}
	}
}

#define EXPORT extern "C" __declspec(dllexport)
static ObjectManager::objects* luaobjs;
static Camera* luacam;
EXPORT void set_camera(float x,float y,float scale)
{
	luacam->position.x = x;
	luacam->position.y = y;
	luacam->scale = scale;
	luacam->update = true;
}
EXPORT ObjectManager::object* get_object()
{
	ObjectManager::object* obj = luaobjs->objectPool.new_item();
	memset(obj, 0, sizeof *obj);
	return obj;
}
EXPORT ObjectManager::drawdata* get_sprite()
{
	ObjectManager::drawdata* spr = luaobjs->drawPool.new_item();
	memset(spr, 0, sizeof *spr);
	return spr;
}
EXPORT void dispose_sprite(ObjectManager::drawdata* spr)
{
	luaobjs->drawPool.delete_obj(spr);
}
EXPORT void dispose_object(ObjectManager::object* obj)
{
	if(obj->drawPtr != NULL)
	{
		luaobjs->drawPool.delete_obj(obj->drawPtr);
	}
	memset(obj, 0, sizeof *obj);
	luaobjs->objectPool.delete_obj(obj);
}
int main(int argc, const char **argv)
{
	sol::state lua;
	lua.open_libraries(sol::lib::base,
		sol::lib::package, sol::lib::jit,sol::lib::ffi);
	sol::table pakki_table = lua.create_named_table("Pakki");
	sol::table meta = lua.create_table_with();

	ObjectManager::objects Objects;

	lua.script_file("pakki.lua");
	auto con = lua["configure"];
	if (!con.valid())
	{
		printf("Please do configure table");
		getchar();
		return fail;
	}
	Pakki::PakkiContext context;

	int w = con["ScreenX"];
	int h = con["ScreenY"];
	float WorldX = con["WorldX"];
	float WorldY = con["WorldY"];
	float worldW = con["WorldWidht"];
	float worldH = con["WorldHeight"];
	printf("World : %.2f : %.2f : %.2f : %.2f\n", WorldX, WorldY, worldW, worldH);
	ObjectManager::init_objects(&Objects, ObjectManager::vec2{ WorldX,WorldY }, ObjectManager::vec2{ worldW, worldH});
	//soundsystem::initsounds();
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // get access to smaller subset of opengl features
	GLFWwindow* window  = glfwCreateWindow(w, h, "Tabula rasa", NULL, NULL);

	if (window == NULL)
	{
		LOGI("failed to initialize window");
		return 0;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // GLAD manages function pointers for opengl(here we init it)
	{
		LOGI("failed to initialize window");
		window = NULL;
		return 0;
	}
	LOGI("*** OpenGL Version: %s\n", glGetString(GL_VERSION));// this prints your current opengl version
	glfwSwapInterval(0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, w, h);

	/******************************************/
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	Camera camera;
	camera.position.x = WorldX;
	camera.position.y = WorldY;
	luacam = &camera;
	camera.scale = 1.f;
	Shader shader{ 0,0 };
	engine engine;
	memset(&engine, 0, sizeof(engine));
	SpriteBatch batch;
	DebugRenderer drenderer;
	engine.batch = &batch;
	engine.drenderer = &drenderer;
//	FontRenderer font;
	//engine.text = &font;
	engine.height = h;
	engine.widht = w;
	::k = &engine.key;
	engine_init(&engine,&camera,&shader,WorldX,WorldY,worldW,worldH);
	int num_draw_calls = 0;
	init_inputs(&inputs);
	init_pakki(&context, &engine,&num_draw_calls);
	double t = 0.0;
	constexpr double dt = 1.0 / 60.0;

	double currentTime = glfwGetTime();
	double accumulator = 0.0;
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float time[10]{0};
	const float fpsUpdateRate = 1.0f;
	float lastTime = 0;
	float updating = 0;
	float currentFps = 0;
	int index = 0;
	bool pause = true;
	bool debuglines = false;
	lua["initPakki"]();
	auto luaUpdate = lua["updatePakki"];
	bool stopgame = false;
	while(!glfwWindowShouldClose(window))
	{
        if (is_key_pressed(&inputs,GLFW_KEY_ESCAPE) || stopgame) break;
		glfwPollEvents();
		glm::vec2 mWpos = point_to_world_position(engine.camera, &glm::vec2(inputs.xMouse, inputs.yMouse));
		inputs.xWorld = mWpos.x;
		inputs.yWorld = mWpos.y;
		static float volume = 0.1f;
		static float pitch = 1.f;
		//soundsystem::ss->update(pause, volume, pitch);
		//LOGI("button %d\n", button);
		start_frame(&context,glm::vec2(inputs.xMouse,inputs.yMouse),button);
		double newTime = glfwGetTime();
		time[index++] = (float)newTime - lastTime;
		if (index >= 10)index = 0;
		updating += newTime - lastTime;
		lastTime = (float)newTime;



		static bool profiler = false;
		static bool audio = false;
		Pakki::window(&context,GEN_ID,Pakki::Window_No_Header);
		if(pause && Pakki::button(&context, "play"))
		{
			pause ^= 1;
		}
		if (!pause && Pakki::button(&context, "pause"))
		{
			pause ^= 1;
		}
		Pakki::checkbox(&context, &profiler,Pakki::SameLine);
		Pakki::text(&context, "profile");
		Pakki::checkbox(&context, &audio, Pakki::SameLine);
		Pakki::text(&context, "audio");
		Pakki::checkbox(&context, &profiler, Pakki::SameLine);
		Pakki::text(&context, "debug");
		Pakki::end_window(&context);
		if(profiler)
		{
			Pakki::window(&context, GEN_ID,Pakki::Window_No_Header);
			Pakki::text(&context, "FPS %d", (int) currentFps);
			Pakki::text(&context, "FrameTime %f", time[index]*1000);
			Pakki::text(&context, "NumDrawcalls %d",num_draw_calls);
			Pakki::text(&context, "objects %d", 0);
			Pakki::end_window(&context);
		}
		num_draw_calls = 0;
		if(audio)
		{
			Pakki::window(&context, GEN_ID, Pakki::Window_No_Header);
			Pakki::text(&context, "Audio Controlls");
			Pakki::sliderf(&context, &volume,"Volume");
			Pakki::sliderf(&context, &pitch, "Pitch");
			Pakki::end_window(&context);

		}
		if(updating > fpsUpdateRate)
		{
			float add = 0;
			for (int i = 0; i < 10; i++)
			{
				add += time[i];
			}
			currentFps = 1.f / (add / 10.f);
			updating = 0;
		}
		double frameTime = newTime - currentTime;
		currentTime = newTime;
		accumulator += frameTime;

		while (accumulator >= dt)//processloop
		{
			accumulator -= dt;
			t += dt;
			if(!pause)
			{
				int suc = luaUpdate(dt);
				if (!suc) stopgame = true;
				engine_events(&engine, dt,currentFps);
			}
			update_keys(&inputs);
		}

		num_draw_calls += engine_draw(&engine);
		Pakki::render(&context);
		glfwSwapBuffers(window);

	}
	ObjectManager::dispose_objects(&Objects);
	engine_clearup(&engine);
	dispose_inputs(&inputs);
	//clear up engine
	glfwTerminate();
	return pass;
}


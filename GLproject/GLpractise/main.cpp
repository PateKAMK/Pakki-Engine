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

#define FILEID 10

enum ret
{
	pass,
	fail
};
static glm::vec2* mpos;
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
		set_key(key, true);
	}
	else if(action == GLFW_RELEASE)
	{
		set_key(key, false);
	}
	/*if (action == GLFW_PRESS)
	{
		if(key == GLFW_KEY_LEFT)
		{
			k->arrowL = true;
		}
		if (key == GLFW_KEY_RIGHT)
		{
			k->arrowR = true;
		}		
		if (key == GLFW_KEY_UP)
		{
			k->arrowU = true;
		}
        if(key == GLFW_KEY_ESCAPE)
        {
            esc = true;
        }
	}
	if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_LEFT)
		{
			k->arrowL = false;
		}
		if (key == GLFW_KEY_RIGHT)
		{
			k->arrowR = false;
		}
		if (key == GLFW_KEY_UP)
		{
			k->arrowU = false;
		}
	}*/
   
}
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	*mpos = glm::vec2(xpos, ypos);
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

int main(int argc,const char **argv)
{
	Pakki::PakkiContext context;
	int w = 1200;
	int h = 800;
	soundsystem::initsounds();
	//ree.loadSound("Bag Raiders - Shooting Stars.mp3", 1);
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // get access to smaller subset of opengl features
	GLFWwindow* window  = glfwCreateWindow(w, h, "Tabula rasa", NULL, NULL);
	//int texture_units;
	//glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
	//printf("Texture units avaivable: %d", texture_units);
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
	camera.scale = 1.f;
	Shader shader{ 0,0 };
	engine engine;
	memset(&engine, 0, sizeof(engine));
	::mpos = &engine.mousePos;
	SpriteBatch batch;
	DebugRenderer drenderer;
	engine.batch = &batch;
	engine.drenderer = &drenderer;
//	FontRenderer font;
	//engine.text = &font;
	engine.height = h;
	engine.widht = w;
	::k = &engine.key;
	engine_init(&engine,&camera,&shader);
	int num_draw_calls = 0;

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
	while(!glfwWindowShouldClose(window))
	{
        if (is_key_pressed(GLFW_KEY_ESCAPE)) break;
		glfwPollEvents();
		static float volume = 0.1f;
		static float pitch = 1.f;
		soundsystem::ss->update(pause, volume, pitch);
		//LOGI("button %d\n", button);
		start_frame(&context, engine.mousePos,button);
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
				engine_events(&engine, dt,currentFps);
			}
			update_keys();
		}

//		glEnable(GL_SCISSOR_TEST);

//		glScissor(300, 300, 300, 300);
		num_draw_calls += engine_draw(&engine);
//		glDisable(GL_SCISSOR_TEST);
		Pakki::render(&context);
		glfwSwapBuffers(window);

	}
	engine_clearup(&engine);
	//clear up engine
	glfwTerminate();
	return pass;
}


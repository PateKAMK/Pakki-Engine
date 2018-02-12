#include "engine.h"
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
#include <iostream>
#define SOL_CHECK_ARGUMENTS 1
#include <sol.hpp>

#include<filesystem.h>
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

static spriteCache* luasprites;

#define EXPORT extern "C" __declspec(dllexport)
static ObjectManager::objects* luaobjs;
static Camera* luacam;
static DebugRenderer* luaDRend;
EXPORT void set_camera(float x,float y,float scale)
{
	luacam->position.x = x;
	luacam->position.y = y;
	luacam->scale = scale;
	luacam->update = true;
}
EXPORT ObjectManager::object* get_object_static(float x,float y,float w, float h)
{
	ObjectManager::object* obj = luaobjs->objectPool.new_item();
	memset(obj, 0, sizeof *obj);
	obj->pos.x = x;
	obj->pos.y = y;
	obj->dim.x = w;
	obj->dim.y = h;
	obj->s = ObjectManager::Static;
	return obj;
}
EXPORT ObjectManager::object* get_object_moving(float x, float y, float w, float h)
{
	ObjectManager::object* obj = luaobjs->objectPool.new_item();
	memset(obj, 0, sizeof *obj);
	obj->pos.x = x;
	obj->pos.y = y;
	obj->dim.x = w;
	obj->dim.y = h;
	obj->s = ObjectManager::Moving;
	return obj;
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

EXPORT void draw_box(float x,float y,float w, float h,float angle)
{
	draw_debug_box(luaDRend, x - w, y - h, w  * 2, h * 2, angle);
}
EXPORT void draw_line(float x1,float y1,float x2,float y2)
{
	draw_debug_line(luaDRend, x1, y1, x2, y2);
}
EXPORT void hello_world()
{
	printf("hello world");
}
EXPORT bool is_key_down(const char key[])
{
	switch (*key)
	{
	case 'w':
		return is_key_active(&inputs, GLFW_KEY_W);
		break;
	case 'a':
		return is_key_active(&inputs, GLFW_KEY_A);
		break;
	case 's':
		return is_key_active(&inputs, GLFW_KEY_S);
		break;
	case 'd':
		return is_key_active(&inputs, GLFW_KEY_D);
		break;
	default:
		return false;
		break;
	}
}
EXPORT bool is_key_activated(const char key[])
{
	switch (*key)
	{
	case 'w':
		return is_key_pressed(&inputs, GLFW_KEY_W);
		break;
	case 'a':
		return is_key_pressed(&inputs, GLFW_KEY_A);
		break;
	case 's':
		return is_key_pressed(&inputs, GLFW_KEY_S);
		break;
	case 'd':
		return is_key_pressed(&inputs, GLFW_KEY_D);
		break;
	default:
		return false;
		break;
	}
}
EXPORT ObjectManager::vec2 getMousePos()
{
	return ObjectManager::vec2{ inputs.xWorld, inputs.yWorld };
}


struct collisionMap 
{
	unsigned char* textureData;
	unsigned char* collisionData;
	bool update;
	int texwidht;
	int texheight;
	int spriteid;
	float posX;
	float posY;
	float widht;
	float height;
};
GLuint TESTID = 100;
int TESTWIDHT = 0;
int TESTHEIGHT = 0;

Texture* load_sprite(spriteCache* cache,const char* path)
{
	Texture* ret = NULL;
	auto tex = cache->sprites.find(path);
	if(tex == cache->sprites.end())
	{
		Texture t = FileSystem::load_sprite_io(path);
		assert(cache->bindedTextures._max < 29);
		/*glActiveTexture(GL_TEXTURE0 + cache->bindedTextures._max);
		glBindTexture(GL_TEXTURE_2D, t.ID);*/
		cache->bindedTextures.ids[cache->bindedTextures._max] = cache->bindedTextures._max;
		cache->bindedTextures.textures[cache->bindedTextures._max] = t;
	/*	TESTID = t.ID;
		TESTHEIGHT = t.height;
		TESTWIDHT = t.widht;*/
		t.ID = cache->bindedTextures._max++;
		printf("cpp id = %d\n", t.ID);
		auto r = cache->sprites.insert(std::make_pair(path,t));
		ret = &r.first->second;
	}
	else
	{
		ret = &tex->second;
	}
	return ret;
}
void reloadCollisionMap(collisionMap* map,spriteCache* cache)
{
	Texture newTexture = FileSystem::reload_texture(cache->bindedTextures.textures[map->spriteid].ID, map->textureData, map->texwidht, map->texheight);
	cache->bindedTextures.textures[map->spriteid].ID = newTexture.ID;
}
void load_collisionmap(collisionMap* map,const char* path, spriteCache* cache)
{
	Texture t = FileSystem::load_sprite_to_buffer(path, &map->textureData);

	assert(cache->bindedTextures._max < 29);

	cache->bindedTextures.ids[cache->bindedTextures._max] = cache->bindedTextures._max;
	cache->bindedTextures.textures[cache->bindedTextures._max] = t;
	t.ID = cache->bindedTextures._max++;
	printf("cpp id = %d\n", t.ID);
	auto r = cache->sprites.insert(std::make_pair(path, t));
	


	map->spriteid = t.ID;
	map->texheight = t.height;
	map->texwidht = t.widht;
	map->collisionData = (unsigned char*)calloc(t.height*t.widht, sizeof(unsigned char));
	int k = 0;
	for(int i = 4; i < t.height*t.widht / 4;i += 4)
	{
		if(map->textureData[i] != 0)
		{ 
			map->collisionData[k] = 1;
		}
		k++;
	}
}
EXPORT  ObjectManager::drawdata* load_picture(const char* path)
{
	ObjectManager::drawdata* data = luaobjs->drawPool.new_item();
	data->color = Color{ 255,255,255,255 };
	data->level = 0;
	data->spriteid = load_sprite(luasprites, path)->ID;
	data->uv = ObjectManager::vec4{ 0,0,1,1 };
	return data;
}
EXPORT void update_objects()
{
	ObjectManager::update_objects(luaobjs);
}



int main(int argc, const char **argv)
{
	sol::state lua;
	lua.open_libraries(sol::lib::base,
		sol::lib::package, sol::lib::jit,sol::lib::ffi);
	sol::table pakki_table = lua.create_named_table("Pakki");
	sol::table meta = lua.create_table_with();





	ObjectManager::objects Objects;
	luaobjs = &Objects;

	sol::load_result lr = lua.load_file("pakki.lua");


	if (!lr.valid()) { // This checks the syntax of your script, but does not execute it
		sol::error err = lr;
		std::string what = err.what();
		std::cout << "call failed, sol::error::what() is " << what << std::endl;
		getchar();
	}
	else
	{
		sol::protected_function_result result1 = lr(); // this causes the script to execute
		if (!result1.valid()) {
			sol::error err = result1;
			std::string what = err.what();
			std::cout << "call failed, sol::error::what() is " << what << std::endl;
			getchar();
		}
		else
			std::cout << "OK" << std::endl;
	}



	//lua.script_file("pakki.lua");


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
	engine.objs = &Objects;
	SpriteBatch batch;
	DebugRenderer drenderer;
	luaDRend = &drenderer;
	engine.batch = &batch;
	engine.drenderer = &drenderer;
//	FontRenderer font;
	//engine.text = &font;
	engine.height = h;
	engine.widht = w;
	engine_init(&engine,&camera,&shader,WorldX,WorldY,worldW,worldH);
	int num_draw_calls = 0;
	init_inputs(&inputs);
	init_pakki(&context,&num_draw_calls);
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
	bool pause = false;
	bool debuglines = false;
	spriteCache Sprites;
	luasprites = &Sprites;
	engine.spritecache = &Sprites;

	try
	{
		lua["initPakki"]();
	}
	catch(const sol::error& err)
	{
		std::cout << "Caught sol error" <<err.what() <<std::endl;
		getchar();
	}
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
				int suc = 0;
				try
				{
					suc = luaUpdate(dt);
				}
				catch (const sol::error& err)
				{
					std::cout << "Caught sol error" << err.what() << std::endl;
					getchar();
				}
				if (!suc) stopgame = true;
				draw_debug_box(engine.drenderer, WorldX - worldW, WorldY - worldH, worldW * 2, worldH *2, 0);
				if (TESTID != 100)
				{
					//unsigned char* tester = (unsigned char*)malloc(sizeof(unsigned char)* TESTWIDHT*TESTHEIGHT);
					/*GLubyte* tester = (GLubyte*)malloc(sizeof(GLubyte) * 100 * 100 * 4);

					glGetTexImage(GL_TEXTURE_2D, 0 ​, GL_RGBA, GL_UNSIGNED_BYTE,  tester)*/;
				}
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
	dispose_pakki(&context);
	//clear up engine
	glfwTerminate();
	return pass;
}



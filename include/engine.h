#pragma once
#include "camera.h"
#include "shader.h"
#include <texture.h>
#include <spritebatch.h>
#include <debugrendering.h>
#include <entities.h>
//#include <freetype.h>
enum class EngineStates:int
{

};
struct keys
{
	bool arrowL;
	bool arrowR;
	bool arrowU;
};
struct engine {
	Camera*			camera;
	Shader*			shader;
	EngineStates	state;
	SpriteBatch*	batch;
	void*			platformState;
	DebugRenderer*	drenderer;
	glm::vec2		mousePos;
	Entities		objects;
	int				widht, height;
	keys			key;
};

extern void engine_init(engine* engine, Camera* camera, Shader* shader);
extern void engine_events(engine* engine,double deltaTime,float fps);
extern int engine_draw(engine* engine);
extern void engine_clearup(engine* engine);
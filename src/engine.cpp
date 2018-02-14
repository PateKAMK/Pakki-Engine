#include "filesystem.h"
#include "engine.h"
#include "projectDefinitions.h"
#include <string>
#include <iostream>
#include <entities.h>
#ifdef P_WINDOWS
#include <glad/glad.h>
#endif // P_WINDOWS
#ifdef P_ANDROID
#include <GLES2/gl2.h>
#include <android/log.h>
#include <engine.h>

#endif //P_ANDROID
//
#include <InstancedRenderer.h>
#include <perlinnoice.h>
static InRender::instaRenderer inrend;
GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
			//case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
			//case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
        fatalerror(false);
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)
void engine_init(engine* engine,Camera* camera,Shader* shader, float WorldX, float WorldY, float worldWidht, float WorldHeight)
{
#ifdef P_ANDROID
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif
	camera->scale = 1.0f;
	init_camera(camera, engine->widht, engine->height);
	char* vs; 
	int size = 0;
	FileSystem::load_file_to_buffer(txt_file_names[VERT_SHA], &vs, &size);
	fatalerror(vs);
	GLuint vert = compile_shader(GL_VERTEX_SHADER, vs);
	free(vs);
	char* fs;
	size = 2;
	FileSystem::load_file_to_buffer(txt_file_names[FRAG_SHA], &fs, &size);
	fatalerror(vert);
	GLuint frag = compile_shader(GL_FRAGMENT_SHADER, fs);
	free(fs);
	fatalerror(frag);
	shader->program = glCreateProgram();
	glAttachShader(shader->program, vert);
	glAttachShader(shader->program, frag);

	add_attribute(shader, "vertexPosition");
	add_attribute(shader,"vertexColor");
	add_attribute(shader,"vertexUV");
	add_attribute(shader, "id");
	fatalerror(link_shader(shader, vert, frag));
	engine->shader = shader;
	engine->camera = camera;
	use_shader(shader);// debug

	init_batch(engine->batch);
	unuse_shader(shader);// debug
	glCheckError();
	init_debug_renderer(engine->drenderer);
	glCheckError();
	//init_fonts(engine->text);
#ifndef OUT_OF_DATE
	init_entities(&engine->objects, engine,engine->drenderer);
#endif // !OUT_OF_DATE


	InRender::init_renderer(&inrend);

	LOGI("engine inited\n");
}
//#ifdef P_WINDOWS
int engine_draw(engine* engine)
{
	int ret = 0;
	glClearColor(0.f, 0.f, 0.082f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

//	use_shader(engine->shader);
////	glActiveTexture(GL_TEXTURE0);// tells u want to use texture unit 0	
//	glCheckError();
//	set_matrix(engine->shader,"P", &(engine->camera->cameraMatrix));
//	GLint textureUniform = get_uniform_location(engine->shader, "mySamples");
//	for(int i = 0; i < engine->spritecache->bindedTextures._max;i++)
//	{
//		glActiveTexture(GL_TEXTURE0 + i);
//		glBindTexture(GL_TEXTURE_2D, engine->spritecache->bindedTextures.textures[i].ID);
//		//printf("i%d  %d\n",i, engine->spritecache->bindedTextures.textures[i].ID);
//	}
//	//int* tes = (int*)calloc(30, sizeof(int));
//	glUniform1iv(textureUniform, 30,engine->spritecache->bindedTextures.ids);
	glCheckError();

	ret += render_batch(engine->batch,engine->shader,engine->spritecache, &engine->camera->cameraMatrix);

	glCheckError();
	//render_debug_lines(engine->drenderer,&engine->camera->cameraMatrix);
	ret += 1;
	InRender::render(&inrend, engine->spritecache->bindedTextures.textures[0].ID, &engine->camera->cameraMatrix);
	glCheckError();

	//free(tes);
	return ret;
//	render_text(engine->text);
}
//
//struct entitity
//{
//	GLuint texid;
//	glm::vec2 pos;
//	glm::vec2 dim;
//};

void engine_events(engine* engine, double deltaTime, float fps)
{
	
	begin(engine->batch);
#ifndef OUT_OF_DATE
	update_objects(&engine->objects,&engine->key);
	push_objects_to_batch(&engine->objects, engine->batch,engine->drenderer);
#else
   /* for(int i = 0; i < 1;i++)
    {
        PakkiPhysics::update_objects(engine->scene, deltaTime / 1.f, &engine->key);
    }
    PakkiPhysics::draw_objects(&engine->scene, engine->batch);*/
	draw_debug_box(engine->drenderer, 0, 0, 30 + 30, 30 + 30,0);
#endif
	//ObjectManager
	ObjectManager::draw_objects(&engine->objs->drawAbleOnes, engine->batch);
	post_batch_process(engine->batch);
	populate_debugrender_buffers(engine->drenderer);
	tileId* mesh = NULL;
	generate_mesh(&mesh);
	for (float h = 0; h < 100; h++)
	{
		for (float w = 0; w < 100; w++)
		{
			if (mesh[(int)((w*h) + w)] == 0) continue;

			float scale = 10;
			float uv = 0;
			float rw = w * scale - 100;
			float rh = h * scale - 100;
			float uv2 = 1;
			InRender::push_to_renderer(&inrend, &rw, &rh, &scale, &uv, &uv, &uv2, &uv2);
		}
	}

		free(mesh);
	/*for(int i = 0 ; i < 4 ;i++)
	{
		float k = 0 - i * 200;
		float k1 = 0;
		float k11 = 1;
		float scale = 100;
		InRender::push_to_renderer(&inrend, &k , &k1, &scale, &k1, &k1, &k11,&k11);
	}*/
	InRender::create_buffers(&inrend);
	update_camera(engine->camera);
}

void engine_clearup(engine* engine)
{
	dispose_batch(engine->batch);
	dispose_shader(engine->shader);
	dispose_debug_renderer(engine->drenderer);
}

#include "filesystem.h"
#include "engine.h"
#include "window.h"
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



GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
        fatalerror(false);
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
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)
void engine_init(engine* engine,Camera* camera,Shader* shader)
{
	FileSystem::init_filesystem();
#ifdef P_ANDROID
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif
	camera->scale = 1.0f;
	init_camera(camera, engine->widht, engine->height);
	char* vs; 
	int size = 0;
	FileSystem::load_file_to_buffer(txt_file_names[VERT_SHA], &vs, &size,engine);
	fatalerror(vs);
	GLuint vert = compile_shader(GL_VERTEX_SHADER, vs);
	free(vs);
	char* fs;
	size = 2;
	FileSystem::load_file_to_buffer(txt_file_names[FRAG_SHA], &fs, &size,engine);
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

	init_entities(&engine->objects, engine,engine->drenderer);
	LOGI("engine inited\n");
}
//#ifdef P_WINDOWS
int engine_draw(engine* engine)
{
	int ret = 0;
	glClearColor(0.f, 0.f, 0.082f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	use_shader(engine->shader);
	glActiveTexture(GL_TEXTURE0);// tells u want to use texture unit 0	
	glCheckError();
	set_matrix(engine->shader,"P", &(engine->camera->cameraMatrix));
	GLint textureUniform = get_uniform_location(engine->shader, "mySampler");
	glUniform1i(textureUniform, 0);
#ifdef P_WINDOWS
	ret += render_batch(engine->batch);
#endif
	glCheckError();
	unuse_shader(engine->shader);
	render_debug_lines(engine->drenderer,&engine->camera->cameraMatrix);
	ret += 1;
	return ret;
//	render_text(engine->text);
}

struct entitity
{
	GLuint texid;
	glm::vec2 pos;
	glm::vec2 dim;
};

void engine_events(engine* engine, double deltaTime, float fps)
{
	



	begin(engine->batch);
	update_objects(&engine->objects,&engine->key);
	push_objects_to_batch(&engine->objects, engine->batch,engine->drenderer);
	post_batch_process(engine->batch);
	populate_debugrender_buffers(engine->drenderer);
	glm::vec2 wpos = point_to_world_position(engine->camera, &engine->mousePos);

	char buf[64];
	char buf2[64];
	sprintf(buf, "fps %.2f", fps);
	sprintf(buf2, "x %.2f y %.2f", wpos.x, wpos.y);

	update_camera(engine->camera);

}

void engine_clearup(engine* engine)
{
	dispose_batch(engine->batch);
	dispose_shader(engine->shader);
	dispose_debug_renderer(engine->drenderer);
	//dispose_fonts(engine->text);
}

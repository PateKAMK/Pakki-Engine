#pragma once
#include <projectDefinitions.h>
#ifdef P_WINDOWS
#include <glad/glad.h>
#endif //P_WINDOWS
#ifdef  P_ANDROID
#include <GLES2/gl2.h>
#endif //P_ANDROID
#include "dybamic_array.h"
#include <glm/glm/common.hpp>


struct Shader
{
	GLuint		program;
	uint32_t	numAttributes;
};

extern GLuint compile_shader(GLenum type,const char* source);
extern bool link_shader(Shader* shader,GLuint vert,GLuint frag);
extern void dispose_shader(Shader* shader);
extern void unuse_shader(Shader* shader);
extern void add_attribute(Shader* shader,const char* name);
extern void use_shader(Shader* shader);
extern void set_matrix(Shader *shader, const char* name, glm::mat4x4* mat);
extern void set_vec3(Shader* shader, const char* name, glm::vec3* vec);
extern GLint get_uniform_location(Shader* shader,const char* name);
extern void set_vec4(Shader* shader, const char* name, const glm::vec4* vec);
extern GLuint get_addribute_location(Shader* shader, const char* location);
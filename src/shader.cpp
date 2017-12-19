#include "shader.h"
#include "projectDefinitions.h"
#ifdef P_WINDOWS
#include <stdio.h>
#endif // P_WINDOWS
#ifdef P_ANDROID
#include <android/log.h>
#endif //P_ANDROID
#include <glm/glm/gtc/type_ptr.hpp>
GLuint compile_shader(GLenum type, const char* source)
{
	GLint compiledcheck;

	GLuint shader = glCreateShader(type);
	if (shader == 0)
		return 0;

	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiledcheck);
	if (!compiledcheck)
	{
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1)
		{
			char* infoLog = (char*)malloc(sizeof(char) * infoLen);
			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			LOGI("Error compiling shader :\n%s\n", infoLog);
			free(infoLog);
		}
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}
bool link_shader(Shader* shader, GLuint vert, GLuint frag)
{
	LOGI("linking program %d\n", shader->program);
	glLinkProgram(shader->program);
	GLint linked;
	glGetProgramiv(shader->program, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		GLint infoLen = 0;
		glGetProgramiv(shader->program, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1)
		{
			char* infoLog = (char*)malloc(sizeof(char) * infoLen);
			glGetProgramInfoLog(shader->program, infoLen, NULL, infoLog);
			LOGI("Error linking program:\n%s\n", infoLog);
			free(infoLog);
		}
		glDeleteProgram(shader->program);
		LOGI("FAILED TO CREATE GL PROGRAM");
		return false;
	}
	glDeleteShader(vert);
	glDeleteShader(frag);
	glClearColor(1.f, 1.f, 1.f, 1.f);
	return true;
}
void dispose_shader(Shader* shader)
{
	if (!shader->program)glDeleteProgram(shader->program);
}
void unuse_shader(Shader* shader)
{
	glUseProgram(0);
	for (int i = 0; i < shader->numAttributes; i++) {
		glDisableVertexAttribArray(i);
	}
}
void add_attribute(Shader* shader, const char* name)
{
	LOGI("adding attribute %s to program %d\n", name,shader->program);
	glBindAttribLocation(shader->program, shader->numAttributes++, name);
}
void use_shader(Shader* shader)
{
	glUseProgram(shader->program);
	for (int i = 0; i < shader->numAttributes; i++) {
		glEnableVertexAttribArray(i);
	}
}
void set_matrix(Shader *shader,const char* name, glm::mat4x4* mat)
{
	glUniformMatrix4fv(glGetUniformLocation(shader->program, name), 1, GL_FALSE, glm::value_ptr(*mat));
}
void set_vec3(Shader* shader, const char* name, glm::vec3* vec)
{
	int vertexLocation = glGetUniformLocation(shader->program, name);
	glUniform3f(vertexLocation, vec->x, vec->y, vec->z);
}
GLint get_uniform_location(Shader* shader,const char* name)
{
	GLint location = glGetUniformLocation(shader->program, name);
#ifdef P_WINDOWS

	if (location == GL_INVALID_INDEX)
	{
		LOGI("failed to get uniform");
		fatalerror(false);
	}
#endif
	return location;
}
void set_vec4(Shader* shader, const char* name, const glm::vec4* vec)
{
	int vertexLocation = glGetUniformLocation(shader->program, name);

	glUniform4f(vertexLocation, vec->x, vec->y, vec->z,vec->w);
}
GLuint get_addribute_location(Shader* shader, const char* location)
{
	return glGetAttribLocation(shader->program, location);
}
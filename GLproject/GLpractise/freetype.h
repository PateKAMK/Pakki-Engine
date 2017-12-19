#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include<glm/glm/gtc/matrix_transform.hpp>
#include<glm/glm/gtc/type_ptr.hpp>
#include <shader.h>

typedef struct
{
	Shader		shader;
	GLuint		texID;
	GLuint		vao;
	GLuint		vbo;
	FT_Face		face;
	FT_Library	library;
	glm::mat4	projection;
}FontContext;




extern void init_fonts();
extern void render_text(char const * text, float x, float y, float sx, float sy);



//extern void init_fonts();
//extern void render_text(char const * text, float x, float y, float sx, float sy);
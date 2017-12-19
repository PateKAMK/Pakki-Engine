#pragma once
#ifdef old
#include <ft2build.h>
#include FT_FREETYPE_H
#include<glm\glm\gtc\matrix_transform.hpp>
#include<glm\glm\gtc\type_ptr.hpp>
#include <shader.h>
struct character_info {
	float ax; // advance.x
	float ay; // advance.y

	float bw; // bitmap.width;
	float bh; // bitmap.rows;

	float bl; // bitmap_left;
	float bt; // bitmap_top;

	float tx; // x offset of glyph in texture coordinates
	float ty;
} /*c[128]*/;	  // character information


struct point {
	GLfloat x;//coord;
	GLfloat y;//coord;
	GLfloat s;//texcoord;
	GLfloat t;//texcoord;
};

typedef struct
{
	Shader txtshader;
	GLuint tex, vao, vbo;
	GLuint uniform_tex;
	GLuint uniform_color;
	GLuint uniform_matrix;
	unsigned int w, h;
	point *dyArrBatch;
	size_t numAttribs;
	character_info c[128];
}FontRenderer;


extern void init_fonts(FontRenderer* font);
extern void begin_text_rendering(FontRenderer* font);
extern void post_process_text_batch(FontRenderer* font);
extern void push_to_text_batch(FontRenderer* font, char const * text, float x, float y,float scale/* float sx, float sy*/);
extern void render_text(FontRenderer* font);
extern void dispose_fonts(FontRenderer* font);

#endif
//extern void init_fonts();
//extern void render_text(char const * text, float x, float y, float sx, float sy);
#ifdef old


#include "freetype.h"
#include <assert.h>
#include <stdio.h>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <dybamic_array.h>
//#include <glm\glm/gtc/matrix_transform.hpp>
//#include <glm\glm\gtc\type_ptr.hpp>

#define MAXWIDTH 1024

namespace
{
	static constexpr char* VERT = R"(
		#version 330 core
		layout (location = 0) in vec4 coord; // <vec2 pos, vec2 tex>
		out vec2 TexCoords;

		uniform mat4 projection;

		void main()
		{
			gl_Position = projection * vec4(coord.xy, 0.0, 1.0);
			TexCoords = coord.zw;
		}  
)";

	static constexpr char* FRAG = R"(
		#version 330 core

		in vec2 TexCoords;
		out vec4 Outcolor;

		uniform sampler2D tex;
		uniform vec3 color;

		void main()
		{    
			vec4 sampled = texture2D(tex, TexCoords);//vec4(1.0, 1.0, 1.0, texture(tex, TexCoords).r);//texture2D(tex, TexCoords);//vec4(1.0, 1.0, 1.0, texture(tex, TexCoords).r);
			Outcolor = vec4(color, 1.0) * sampled;
		}  
)";
}

//static glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);


inline void init_shaders(FontRenderer* font)
{
	GLuint vert = compile_shader(GL_VERTEX_SHADER, VERT);
	fatalerror(vert);
	GLuint frag = compile_shader(GL_FRAGMENT_SHADER, FRAG);
	fatalerror(frag);
	font->txtshader.program = glCreateProgram();
	//add_attribute(&txtshader, "coord");
	glAttachShader(font->txtshader.program, vert);
	glAttachShader(font->txtshader.program, frag);
	fatalerror(link_shader(&font->txtshader, vert, frag));

	//attribute_coord = get_addribute_location(&txtshader, "coord");
	add_attribute(&font->txtshader, "coord");
	font->uniform_tex = get_uniform_location(&font->txtshader, "tex");
	font->uniform_color = get_uniform_location(&font->txtshader, "color");
	font->uniform_matrix = get_uniform_location(&font->txtshader, "projection");

	if (font->uniform_tex == -1 || font->uniform_color == -1 || font->uniform_matrix == -1)
		fatalerror(false);
	if ((glGetError()) != GL_NO_ERROR)
	{
		fatalerror(false);
	}
	glGenVertexArrays(1, &(font->vao));
	fatalerror(font->vao);
	glBindVertexArray(font->vao);
	glGenBuffers(1, &(font->vbo));
	fatalerror(font->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, font->vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)*4, 0);

	glBindVertexArray(0);
	if ((glGetError()) != GL_NO_ERROR)
	{
		fatalerror(false);
	}
}
//katso tämä
//https://gitlab.com/wikibooks-opengl/modern-tutorials/blob/master/text02_atlas/text.cpp


#undef max
void init_fonts(FontRenderer* font)
{
	memset(font, 0, sizeof *font);
	init_shaders(font);
	allocate_new_array<point>(&font->dyArrBatch);
	//memset(context, 0, sizeof(FontContext));
	
	FT_Library  library;
	FT_Face face;
	FT_Error error = FT_Init_FreeType(&library);
	//face->face_index = 0;
	assert(!error);
	error = FT_New_Face(library, "ProggyClean.ttf", 0, &face);
	assert(!error);

	printf("faces loaded %d\n", face->num_faces);
	printf("characters loaded %d\n",face->num_glyphs);


	error = FT_Set_Pixel_Sizes(
		face,   /* handle to face object */
		0,      /* pixel_width           */
		48);   /* pixel_height          */
	assert(!error);

	FT_GlyphSlot g = face->glyph;


	font->w = 0;
	font->h = 0;
	unsigned int roww = 0;
	unsigned int rowh = 0;
	//memset(atlas.characters, 0, sizeof atlas.characters);


	if ((glGetError()) != GL_NO_ERROR)
	{
		fatalerror(false);
	}

	for (int i = 32; i < 128; i++) {
		if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
			fatalerror(false);
		}
		if (roww + g->bitmap.width + 1 >= MAXWIDTH) {
			font->w = std::max(font->w, roww);
			font->h += rowh;
			roww = 0;
			rowh = 0;
		}
		roww += g->bitmap.width + 1;
		rowh = std::max(rowh, g->bitmap.rows);
	}
	font->w = std::max(font->w, roww);
	font->h += rowh;




	/* you might as well save this value as it is needed later on */
#define teset

#ifndef  teset


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &font->tex);
	glBindTexture(GL_TEXTURE_2D, font->tex);
	//glUniform1i(uniform_tex, 0);
	if ((glGetError()) != GL_NO_ERROR)
	{
		fatalerror(false);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font->w, font->h, 0, GL_RED, GL_UNSIGNED_BYTE, 0);// later put the texture in
	if ((glGetError()) != GL_NO_ERROR)
	{
		fatalerror(false);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

#endif //  teset
	/***************/
#ifdef teset

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &font->tex);
	glBindTexture(GL_TEXTURE_2D, font->tex);
	//glUniform1i(uniform_tex, 0);
	if ((glGetError()) != GL_NO_ERROR)
	{
		fatalerror(false);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, font->w, font->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);// later put the texture in
	if ((glGetError()) != GL_NO_ERROR)
	{
		fatalerror(false);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif // DEBUG


	/*******************/


	if ((glGetError()) != GL_NO_ERROR)
	{
		fatalerror(false);
	}
	int ox = 0;
	int oy = 0;
	
	rowh = 0;

	if ((glGetError()) != GL_NO_ERROR)
	{
		fatalerror(false);
	}


	for (int i = 32; i < 128; i++) {
		if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
			fatalerror(false);
		}

		if (ox + g->bitmap.width + 1 >= MAXWIDTH) {
			oy += rowh;
			rowh = 0;
			ox = 0;
		}
		if(g->bitmap.buffer)
		{
			//unsigned char llll = *(g->bitmap.buffer);

#ifndef  teset
			glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
#endif
#ifdef  teset
			struct data
			{
				unsigned char r;
				unsigned char g;
				unsigned char b;
				unsigned char a;
			};

			data* d = (data*)malloc(sizeof(data) * g->bitmap.width* g->bitmap.rows);
			unsigned char* pointerdata = g->bitmap.buffer;
			for(int i = 0; i <  g->bitmap.width* g->bitmap.rows;i++)
			{
				d[i] = data{ 255,255,255, *pointerdata };
				pointerdata++;
			}
			glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_RGBA, GL_UNSIGNED_BYTE, d);
			free(d);
#endif

		}
		if ((glGetError()) != GL_NO_ERROR)
		{
			assert(false);
		}
		font->c[i].ax = g->advance.x >> 6;
		font->c[i].ay = g->advance.y >> 6;

		font->c[i].bw = g->bitmap.width;
		font->c[i].bh = g->bitmap.rows;

		font->c[i].bl = g->bitmap_left;
		font->c[i].bt = g->bitmap_top;

		font->c[i].tx = ox / (float)(font->w);
		font->c[i].ty = oy / (float)(font->h);

		rowh = std::max<unsigned int>(rowh, g->bitmap.rows);
		ox += g->bitmap.width + 1;
	}

	fprintf(stderr, "Generated a %d x %d (%d kb) texture atlas\n", font->w, font->h, font->w * font->h / 1024);

	if ((glGetError()) != GL_NO_ERROR)
	{
		fatalerror(false);
	}

	FT_Done_Face(face);
	FT_Done_FreeType(library);
}
void push_to_text_batch(FontRenderer* font, char const * text, float x, float y,float scale)
{
	size_t slenght = strlen(text);
	size_t arrlenght = get_array_size<point>(font->dyArrBatch);
	resize_array<point>(&font->dyArrBatch, (slenght * 6) + arrlenght);

	for (const char* p = text; *p; p++) {
		/* Calculate the vertex and texture coordinates */
		float x2 = x + font->c[*p].bl * scale;
		float y2 = -y - font->c[*p].bt * scale;

		float wi = font->c[*p].bw * scale;
		float he = font->c[*p].bh * scale;

		/* Advance the cursor to the start of the next character */
		x += font->c[*p].ax * scale;
		y += font->c[*p].ay * scale;

		/* Skip quintessences(t: aleksi) that have no pixels */
		if (!wi || !he) 
		{
			//font->negation +=6;
			continue;
		};


		if ((glGetError()) != GL_NO_ERROR)
		{
			fatalerror(false);
		}
		/*
		width: the width (in pixels) of the bitmap accessed via face->glyph->bitmap.width.

		height: the height (in pixels) of the bitmap accessed via face->glyph->bitmap.rows.

		bearingX: the horizontal bearing e.g. the horizontal position (in pixels) of the bitmap relative to the origin accessed via face->glyph->bitmap_left.

		bearingY: the vertical bearing e.g. the vertical position (in pixels) of the bitmap relative to the baseline accessed via face->glyph->bitmap_top.

		advance: the horizontal advance e.g. the horizontal distance (in 1/64th pixels) from the origin to the origin of the next glyph. Accessed via face->glyph->advance.x.
		*/

		font->dyArrBatch[font->numAttribs++] = point{
			x2, -y2,								font->c[*p].tx, font->c[*p].ty
		};
		font->dyArrBatch[font->numAttribs++] = point{
			x2 + wi, -y2,							font->c[*p].tx + font->c[*p].bw / font->w, font->c[*p].ty
		};
		font->dyArrBatch[font->numAttribs++] = point{
			x2, -y2 - he,font->c[*p].tx,			font->c[*p].ty + font->c[*p].bh / font->h
		};
		font->dyArrBatch[font->numAttribs++] = point{
			x2 + wi, -y2,							font->c[*p].tx + font->c[*p].bw / font->w, font->c[*p].ty
		};
		font->dyArrBatch[font->numAttribs++] = point{
			x2, -y2 - he, font->c[*p].tx,			font->c[*p].ty + font->c[*p].bh / font->h
		};
		font->dyArrBatch[font->numAttribs++] = point{
			x2 + wi, -y2 - he,						font->c[*p].tx + font->c[*p].bw / font->w, font->c[*p].ty + font->c[*p].bh / font->h
		};
	}
}
void post_process_text_batch(FontRenderer* font)
{
	if (font->numAttribs <= 0)return;

	glBindBuffer(GL_ARRAY_BUFFER, font->vbo);
	//orphan the buffer
	glBufferData(GL_ARRAY_BUFFER, font->numAttribs * sizeof(point), nullptr, GL_DYNAMIC_DRAW);
	//upload the data
	glBufferSubData(GL_ARRAY_BUFFER, 0, font->numAttribs * sizeof(point), font->dyArrBatch);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
static glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
void render_text(FontRenderer* font) 
{
	int err = 0;
	use_shader(&font->txtshader);
	err = glGetError();
	if (err != GL_NO_ERROR)
	{
		fatalerror(false);
	}
	GLfloat black[3] = { 1, 1, 1};
	glUniform3fv(font->uniform_color, 1, black);
	err = glGetError();
	if (err != GL_NO_ERROR)
	{
		fatalerror(false);
	}
	glUniformMatrix4fv(font->uniform_matrix, 1,GL_FALSE, &projection[0][0]);
	err = glGetError();
	if (err != GL_NO_ERROR)
	{
		fatalerror(false);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, font->tex);
	glUniform1i(font->uniform_tex, 0);
	glBindTexture(GL_TEXTURE_2D, font->tex);

	glBindVertexArray(font->vao);
	glDrawArrays(GL_TRIANGLES, 0, font->numAttribs);
	glBindVertexArray(0);

	unuse_shader(&font->txtshader);//p
	if ((glGetError()) != GL_NO_ERROR)
	{
		fatalerror(false);
	}
	//font->numAttribs = 0;
}
void dispose_fonts(FontRenderer* font)
{
	free_dyn_array<point>(font->dyArrBatch);
	if(font->vao != 0) glDeleteVertexArrays(1, &font->vao);
	if(font->vbo != 0)glDeleteBuffers(1, &font->vbo);
}
void begin_text_rendering(FontRenderer* font)
{
	font->numAttribs = 0;
	clear_array<point>(font->dyArrBatch);
}
#endif // old
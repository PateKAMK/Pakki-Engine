#include "freetype.h"
#include <cassert>
#include <cstdio>
#include <algorithm>
#include <iostream>

#define MAXWIDTH 1024


struct character_info {
	float ax; // advance.x
	float ay; // advance.y

	float bw; // bitmap.width;
	float bh; // bitmap.rows;

	float bl; // bitmap_left;
	float bt; // bitmap_top;

	float tx; // x offset of glyph in texture coordinates
	float ty;
} c[128];	  // character information

namespace
{
	static constexpr char* VERT = R"(
		#version 330 core

		in vec4 coord;
		out vec2 texcoord;

		void main() 
		{
		gl_Position = vec4(coord.xy, 0, 1);
		texcoord = coord.zw;
		}
)";

	static constexpr char* FRAG = R"(
		#version 330 core

		in vec2 texcoord;
		uniform sampler2D tex;
		uniform vec4 color;

		void main(void) 
		{
		  gl_FragColor = vec4(1, 1, 1, texture2D(tex, texcoord).r) * color;
		}
)";
}





//std::map<GLchar, Character> Characters;
Shader txtshader;
static GLuint tex,vao,vbo;
static int widht, height;
GLuint attribute_coord;
GLuint uniform_tex;
GLuint uniform_color;
int atlas_width;
void init_shaders()
{
	GLuint vert = compile_shader(GL_VERTEX_SHADER, VERT);
	fatalerror(vert);
	GLuint frag = compile_shader(GL_FRAGMENT_SHADER, FRAG);
	fatalerror(frag);
	txtshader.program = glCreateProgram();
	//add_attribute(&txtshader, "coord");
	glAttachShader(txtshader.program, vert);
	glAttachShader(txtshader.program, frag);
	fatalerror(link_shader(&txtshader, vert, frag));

	attribute_coord = get_addribute_location(&txtshader, "coord");
	uniform_tex = get_uniform_location(&txtshader, "tex");
	uniform_color = get_uniform_location(&txtshader, "color");

	if (attribute_coord == -1 || uniform_tex == -1 || uniform_color == -1)
		fatalerror(false);

	glGenVertexArrays(1, &vao);
	fatalerror(vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	fatalerror(vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)*4, 0);

	glBindVertexArray(0);
}
//katso t�m�
//https://gitlab.com/wikibooks-opengl/modern-tutorials/blob/master/text02_atlas/text.cpp


#undef max

void init_fonts()
{
	init_shaders();
	//memset(context, 0, sizeof(FontContext));
	memset(c, 0, sizeof c);
	
	FT_Library  library;
	FT_Face face;
	FT_Error error = FT_Init_FreeType(&library);
	//face->face_index = 0;
	assert(!error);
	error = FT_New_Face(library, "OpenSans-Bold.ttf", 0, &face);
	assert(!error);

	printf("faces loaded %d", face->num_faces);
	printf("characters loaded %d",face->num_glyphs);


	error = FT_Set_Pixel_Sizes(
		face,   /* handle to face object */
		0,      /* pixel_width           */
		48);   /* pixel_height          */
	assert(!error);

	FT_GlyphSlot g = face->glyph;
	unsigned int w = 0;
	unsigned int h = 0;
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
			w = std::max(w, roww);
			h += rowh;
			roww = 0;
			rowh = 0;
		}

		/*
		#####################################
		#####################################
		#####################################
		*/
		roww += g->bitmap.width + 1;
		rowh = std::max(h, g->bitmap.rows);
	}
	w = std::max(w, roww);
	h += rowh;




	/* you might as well save this value as it is needed later on */
	atlas_width = w;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(uniform_tex, 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, 0);// later put the texture in
	if ((glGetError()) != GL_NO_ERROR)
	{
		fatalerror(false);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	if ((glGetError()) != GL_NO_ERROR)
	{
		std::cout << "REEEEEEEE" << std::endl; //ERRRRROR
	}
	int ox = 0;
	int oy = 0;
	
	rowh = 0;

	if ((glGetError()) != GL_NO_ERROR)
	{
		assert(false);
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

		glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
		if ((glGetError()) != GL_NO_ERROR)
		{
			assert(false);
		}
		c[i].ax = g->advance.x >> 6;
		c[i].ay = g->advance.y >> 6;

		c[i].bw = g->bitmap.width;
		c[i].bh = g->bitmap.rows;

		c[i].bl = g->bitmap_left;
		c[i].bt = g->bitmap_top;

		c[i].tx = ox / (float)w;
		c[i].ty = oy / (float)h;

		rowh = std::max<unsigned int>(rowh, g->bitmap.rows);
		ox += g->bitmap.width + 1;
	}

	fprintf(stderr, "Generated a %d x %d (%d kb) texture atlas\n", w, h, w * h / 1024);

	if ((glGetError()) != GL_NO_ERROR)
	{
		assert(false);
	}

	//height = h;
	//widht = w;
	//FT_Done_Face(face);
	//FT_Done_FreeType(library);
	//atlas.textureID = tex;
	//atlas.widht = w;
	//atlas.height = h;
}

	struct point {
		GLfloat x;//coord;
		GLfloat y;//coord;
		GLfloat s;//texcoord;
		GLfloat t;//texcoord;
	};
void render_text(char const * text, float x, float y, float sx, float sy) {


	GLfloat black[4] = { 0, 0, 0, 1 };
	glUniform4fv(uniform_color, 1, black);


	/* Use the texture containing the atlas */
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(uniform_tex, 0);

	/* Set up the VBO for our vertex data */

	size_t lenght = strlen(text);
	point* coords = (point*)malloc(sizeof(point)* 6 * lenght);

	glEnableVertexAttribArray(attribute_coord);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);


	int cind = 0;

	/* Loop through all characters */
	for (const char* p = text; *p; p++) {
		/* Calculate the vertex and texture coordinates */
		float x2 = x + c[*p].bl * sx;
		float y2 = -y - c[*p].bt * sy;
		float w = c[*p].bw * sx;
		float h = c[*p].bh * sy;

		/* Advance the cursor to the start of the next character */
		x += c[*p].ax * sx;
		y += c[*p].ay * sy;

		/* Skip glyphs that have no pixels */
		if (!w || !h)
			continue;

		coords[cind++] = point{
			x2, -y2, c[*p].tx, c[*p].ty
		};
		coords[cind++] = point{
			x2 + w, -y2, c[*p].tx + c[*p].bw / w, c[*p].ty
		};
		coords[cind++] = point{
			x2, -y2 - h, c[*p].tx, c[*p].ty + c[*p].bh / h
		};
		coords[cind++] = point{
			x2 + w, -y2, c[*p].tx + c[*p].bw / w, c[*p].ty
		};
		coords[cind++] = point{
			x2, -y2 - h, c[*p].tx, c[*p].ty + c[*p].bh / h
		};
		coords[cind++] = point {
			x2 + w, -y2 - h, c[*p].tx + c[*p].bw / w, c[*p].ty + c[*p].bh / h
		};
	}

	/* Draw all the character on the screen in one go */
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(point) * 6 * lenght, coords, GL_DYNAMIC_DRAW);//njee
	glBindBuffer(GL_ARRAY_BUFFER,0);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, cind);

	glDisableVertexAttribArray(attribute_coord);
	glBindVertexArray(0);
	free(coords);
	

//	glClearColor(0.5f, 1, 0.2f, 1.f);
//	glClear(GL_COLOR_BUFFER_BIT);
//
//	size_t lenght = strlen(text);
//	size_t llllttttt = sizeof(point)*lenght;//48
//	point* coords = (point*)malloc(sizeof(point)*lenght);
//	use_shader(&txtshader);
//
//
//	glm::vec4 ccccc(255, 255, 255, 255);
//	set_vec4(&txtshader, "color", &ccccc);	// set uniforms
//	glActiveTexture(GL_TEXTURE0);
//	GLint textureUniform = get_uniform_location(&txtshader, "tex");
//	glUniform1i(textureUniform, 0);
//	glBindTexture(GL_TEXTURE_2D, tex);		// set uniforms
//	if ((glGetError()) != GL_NO_ERROR)
//	{
//		std::cout << "REEEEEEEE" << std::endl;
//	}
//
//	int n = 0;
//	int jaa = 0;
//	for (const char *p = text; *p; p++) {
//		float x2 = x + c[*p].bl * sx;
//		float y2 = -y - c[*p].bt * sy;
//		float w = c[*p].bw * sx;
//		float h = c[*p].bh * sy;
//
//		/* Advance the cursor to the start of the next character */
//		x += c[*p].ax * sx;
//		y += c[*p].ay * sy;
//
//		/* Skip glyphs that have no pixels */
//		if (w && h)
//		{
//			jaa++;
//			//continue;
//		//coord;
//		//texcoord;
//		coords[n++] = point{ x2, -y2, c[*p].tx, 0 };
//		coords[n++] = point{ x2 + w, -y2, c[*p].tx + c[*p].bw / widht, 0 };
//		coords[n++] = point{ x2, -y2 - h, c[*p].tx, c[*p].bh / height };
//		//remember: each glyph occupies a different amount of vertical space
//		coords[n++] = point{ x2 + w, -y2, c[*p].tx + c[*p].bw / widht, 0 };
//		coords[n++] = point{ x2, -y2 - h, c[*p].tx, c[*p].bh / height };
//		coords[n++] = point{ x2 + w, -y2 - h, c[*p].tx + c[*p].bw / widht, c[*p].bh / height };
//		}
//	}
//	//int s = sizeof coords;
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(point)*lenght , coords, GL_DYNAMIC_DRAW);//njee
////	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(vao);//njee
//	//glDrawArrays(GL_TRIANGLES, 0, n);
//	glBindVertexArray(0);
//	if ((glGetError()) != GL_NO_ERROR)
//	{
//		std::cout << "REEEEEEEE" << glGetError() << std::endl;
//	}
//
//	free(coords);
//	unuse_shader(&txtshader);
}
//
//
//
//
//
//
//
#ifdef test

void compile_text_shaders(FontContext *context)
{
	GLuint vert = compile_shader(GL_VERTEX_SHADER, VERT);
	fatalerror(vert);
	GLuint frag = compile_shader(GL_FRAGMENT_SHADER, FRAG);
	fatalerror(frag);
	context->shader.program = glCreateProgram();
	add_attribute(&context->shader, "vertex");
	glAttachShader(context->shader.program, vert);
	glAttachShader(context->shader.program, frag);
	fatalerror(link_shader(&context->shader, vert, frag));


}

void init_fonts(FontContext* context)
{
	memset(context, 0, sizeof(FontContext));
	memset(characters, 0, sizeof characters);
	compile_text_shaders(context);


	FT_Library  library;
	FT_Error error = FT_Init_FreeType(&library);
	//face->face_index = 0;
	assert(!error);
	error = FT_New_Face(library, "OpenSans-Bold.ttf", 0, &context->face);
	if (error == FT_Err_Unknown_File_Format)
	{
		assert(false);
	}
	else if (error)
	{
		assert(false);
	}
	printf("faces loaded %d", context->face->num_faces);
	printf("characters loaded %d", context->face->num_glyphs);

	error = FT_Set_Pixel_Sizes(
		context->face,   /* handle to face object */
		0,      /* pixel_width           */
		48);   /* pixel_height          */
	assert(!error);
	//error = FT_Set_Char_Size(
	//	face,    /* handle to face object           */
	//	0,       /* char_width in 1/64th of points  */
	//	16 * 64,   /* char_height in 1/64th of points */
	//	300,     /* horizontal device resolution    */
	//	300);   /* vertical device resolution      */
	//assert(!error);
	error = FT_Load_Char(context->face, 'X', FT_LOAD_RENDER);
	//FT_Load_
	fatalerror(!error);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

	for ( unsigned char c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(context->face, c, FT_LOAD_RENDER))
		{
			LOGI("ERROR::FREETYTPE: Failed to load Glyph\n");
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			context->face->glyph->bitmap.width,
			context->face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			context->face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(context->face->glyph->bitmap.width, context->face->glyph->bitmap.rows),
			glm::ivec2(context->face->glyph->bitmap_left, context->face->glyph->bitmap_top),
			context->face->glyph->advance.x
		};
		characters[c] = character;
		//Characters.insert(std::pair<GLchar, Character>(c, character));
	}

	FT_Done_Face(context->face);
	FT_Done_FreeType(library);

	glm::mat4 projection = glm::ortho(0.0f, 400.f, 0.0f, 300.0f);



	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	context->vao = VAO;
	context->vbo = VBO;
	context->projection = projection;
}
void render_text(const std::string& text, float x, float y, float scale,FontContext* context)
{
	glm::vec3 color(255, 255, 255);
	use_shader(&context->shader);
	set_vec3(&context->shader, "textColor", &color);
	set_matrix(&context->shader, "projection", &context->projection);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(context->vao);

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = characters[*c];
		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, context->vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	unuse_shader(&context->shader);
}
#endif
//todo blending?
//
//
//
//struct FontCharacter
//{
//	float advanceX;
//	float advanceY;
//
//	float bitmapWidth;
//	float bitmapHeight;
//
//	float bitmapLeft;
//	float bitmapTop;
//
//	float uvOffsetX;
//	float uvOffsetY;
//};
//
//struct FontTextureAtlas
//{
//	GLuint texture;
//	GLuint textureUniform;
//
//	int width;
//	int height;
//
//	FontCharacter characters[128];
//
//	FontTextureAtlas(FT_Face face, int h, GLuint tUniform)
//	{
//		FT_Set_Pixel_Sizes(face, 0, h);
//		FT_GlyphSlot glyphSlot = face->glyph;
//
//		int roww = 0;
//		int rowh = 0;
//		width = 0;
//		height = 0;
//
//		memset(characters, 0, sizeof(FontCharacter));
//
//		for (int i = 32; i < 128; i++)
//		{
//			if (FT_Load_Char(face, i, FT_LOAD_RENDER))
//			{
//				//std::cout << "Loading character %c failed\n", i;
//				continue;
//			}
//
//			if (roww + glyphSlot->bitmap.width + 1 >= MAX_WIDTH)
//			{
//				width = std::fmax(width, roww);
//				height += rowh;
//				roww = 0;
//				rowh = 0;
//			}
//
//			roww += glyphSlot->bitmap.width + 1;
//			rowh = std::fmax(rowh, glyphSlot->bitmap.rows);
//		}
//
//		width = std::fmax(width, roww);
//		height += rowh;
//
//		glGenTextures(1, &texture);
//
//		if (glGetError() != GL_NO_ERROR)
//		{
//			//std::cout << "glGenTextures failed\n";
//		}
//
//		glActiveTexture(GL_TEXTURE0 + texture);
//
//		if (glGetError() != GL_NO_ERROR)
//		{
//			//std::cout << "glActiveTexture failed\n";
//		}
//
//		glBindTexture(GL_TEXTURE_2D, texture);
//
//		if (glGetError() != GL_NO_ERROR)
//		{
//			//std::cout << "glBindTexture failed\n";
//		}
//
//		glUniform1i(tUniform, 0);
//		textureUniform = tUniform;
//
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
//
//		if (glGetError() != GL_NO_ERROR)
//		{
//			//std::cout << "glTexImage2D failed\n";
//		}
//
//		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//
//		if (glGetError() != GL_NO_ERROR)
//		{
//			//std::cout << "glPixelStorei failed\n";
//		}
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//
//		if (glGetError() != GL_NO_ERROR)
//		{
//			//std::cout << "glTexParameteri failed\n";
//		}
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//
//		if (glGetError() != GL_NO_ERROR)
//		{
//			//std::cout << "glTexParameteri failed\n";
//		}
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//
//		if (glGetError() != GL_NO_ERROR)
//		{
//			//std::cout << "glTexParameteri failed\n";
//		}
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//		if (glGetError() != GL_NO_ERROR)
//		{
//			//std::cout << "glTexParameteri failed\n";
//		}
//
//
//		int ox = 0;
//		int oy = 0;
//
//		rowh = 0;
//
//		for (int i = 32; i < 128; i++)
//		{
//			if (FT_Load_Char(face, i, FT_LOAD_RENDER))
//			{
//				std::cout << "Loading character %c failed\n", i;
//				continue;
//			}
//
//			if (ox + glyphSlot->bitmap.width + 1 >= MAX_WIDTH)
//			{
//				oy += rowh;
//				rowh = 0;
//				ox = 0;
//			}
//
//			glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, glyphSlot->bitmap.width, glyphSlot->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, glyphSlot->bitmap.buffer);
//
//			if (glGetError() != GL_NO_ERROR)
//			{
//				LOGI("vorged again");
//			}
//
//			characters[i].advanceX = glyphSlot->advance.x >> 6;
//			characters[i].advanceY = glyphSlot->advance.y >> 6;
//
//			characters[i].bitmapWidth = glyphSlot->bitmap.width;
//			characters[i].bitmapHeight = glyphSlot->bitmap.rows;
//
//			characters[i].bitmapLeft = glyphSlot->bitmap_left;
//			characters[i].bitmapTop = glyphSlot->bitmap_top;
//
//			characters[i].uvOffsetX = ox / (float)width;
//			characters[i].uvOffsetY = oy / (float)height;
//
//			rowh = std::fmax(rowh, glyphSlot->bitmap.rows);
//			ox += glyphSlot->bitmap.width + 1;
//		}
//
//		std::cout << "Generated a " << width << "x " << height << " (" << width * height / 1024 << " kb) texture atlas.\n";
//	}
//
//	~FontTextureAtlas()
//	{
//		glDeleteTextures(1, &texture);
//	}
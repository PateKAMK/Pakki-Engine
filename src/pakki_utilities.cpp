#include <pakki_utilities.h>
#ifndef P_ANDROID
#include <shader.h>
#include <glad/glad.h>
#include <filesystem.h>
#include <algorithm>
#include <ft2build.h>
#include FT_FREETYPE_H


//static Pakki::PakkiContext context;
static Shader pakkiShader;
static GLuint uniform_tex, uniform_matrix/*, uniform_color*/,vao,vbo;
static glm::mat4 projection = glm::ortho(0.0f, 1200.0f, 0.0f, 800.0f);
namespace{
	static constexpr char* pakkivertsha = R"(
	#version 330 core

	layout (location = 0) in vec4 coord; // <vec2 pos, vec2 tex>
	layout (location = 1) in vec4 vertexColor;

	out vec2 TexCoords;
	out vec4 fragmentColor;

	uniform mat4 projection;

	void main()
	{
		gl_Position = projection * vec4(coord.xy, 0.0, 1.0);
		TexCoords = vec2(coord.z, 1.0 - coord.w);
		fragmentColor = vertexColor;

	}  
)";
	static constexpr char* pakkifragsha = R"(
	#version 330 core

	in vec4 fragmentColor;
	in vec2 TexCoords;
	out vec4 Outcolor;


	uniform sampler2D tex;
	//uniform vec3 color;

	void main()
	{    
		vec4 textureColor = texture2D(tex, TexCoords);
		Outcolor = fragmentColor * (textureColor); /*color*/ /*vec4(0.7,0.7,0.7,0.9)*/
	}  

)";

}
void load_characters(Pakki::Font *font);

inline glm::vec2 screen_to_gui(glm::vec2 mpos)
{
	return  glm::vec2(mpos.x, 800-mpos.y);
	
}
void end_pakki(Pakki::Vertex* vertbuffer, Pakki::Renderbatch* batchBuffer, int BatchSize, int vertexSize);
static int* draw_calls = nullptr;
void init_pakki(Pakki::PakkiContext* context, int* numdraw)
{
	draw_calls = numdraw;
	Pakki::init_pakki(context, FileSystem::load_sprite_io("gwen.png").ID,0);
	memset(&pakkiShader, 0, sizeof(Shader));
	GLuint vert = compile_shader(GL_VERTEX_SHADER, pakkivertsha);
	GLuint frag = compile_shader(GL_FRAGMENT_SHADER, pakkifragsha);
	fatalerror(vert && frag);
	pakkiShader.program = glCreateProgram();
	glAttachShader(pakkiShader.program, vert);
	glAttachShader(pakkiShader.program, frag);
	fatalerror(link_shader(&pakkiShader, vert, frag));

	add_attribute(&pakkiShader, "coord");
	add_attribute(&pakkiShader, "vertexColor");
	uniform_tex = get_uniform_location(&pakkiShader, "tex");
	//uniform_color = get_uniform_location(&pakkiShader, "color");
	uniform_matrix = get_uniform_location(&pakkiShader, "projection");

	glGenVertexArrays(1, &vao);
	fatalerror(vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	fatalerror(vbo);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//position and uv
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Pakki::Vertex), (void*)offsetof(Pakki::Vertex, position));
	//color
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Pakki::Vertex), (void*)offsetof(Pakki::Vertex, color));

	glBindVertexArray(0);

	context->rendercallback = &end_pakki;

	load_characters(&context->font);
}

void start_frame(Pakki::PakkiContext* context,glm::vec2 scrpos,bool mouseActive)
{
	glm::vec2 mpos = screen_to_gui(scrpos);
	Pakki::start_frame(context,mpos,mouseActive ? Pakki::CurrentlyActive : Pakki::InActive);
}
//void do_window()
//{
//	Pakki::window(&context, GEN_ID);
//	static bool show = false;
//	static bool show1 = false;
//	//if (Pakki::button(&context,Pakki::NewLine))show1 ^= 1;
//	Pakki::checkbox(&context, &show);
//
//
//
//	Pakki::text(&context, "Run %d", 3);
//	//Pakki::checkbox(&context, &show);
//
//	Pakki::text(&context, "Am i o?");
//
//
//	//if (Pakki::button(&context, Pakki::NewLine))show1 ^= 1;
//	Pakki::end_window(&context);
//
//
//	if(show)
//	{
//		Pakki::window(&context, GEN_ID);
//		Pakki::end_window(&context);
//	}/*
//	if(show)
//	{
//		Pakki::window(&context, GEN_ID,Pakki::Window_No_Header);
//		Pakki::end_window(&context);
//	}*/
//}

//void render_pakki()
//{
//	Pakki::render(&context);
//}

void end_pakki(Pakki::Vertex* vertbuffer, Pakki::Renderbatch* batchBuffer, int BatchSize, int vertexSize)
{
	/*int BatchSize = 0;
	int vertexSize = 0;
	Pakki::end_and_get_batches_frame(&context,&BatchSize,&vertexSize);
	Pakki::Vertex* vertbuffer = context.vertexDrawBuffer;
	Pakki::Renderbatch* batchBuffer = context.batchDrawBuffer;
*/


	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, vertexSize * sizeof(Pakki::Vertex), nullptr, GL_DYNAMIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexSize * sizeof(Pakki::Vertex), vertbuffer);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	use_shader(&pakkiShader);
	//GLfloat white [3] = { 255, 255, 255 };

	//glUniform3fv(uniform_color, 1, white);
	glUniformMatrix4fv(uniform_matrix, 1, GL_FALSE, &projection[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Pakki::fet_id());
	glUniform1i(uniform_tex, 0);
	glBindTexture(GL_TEXTURE_2D, Pakki::fet_id());

	glBindVertexArray(vao);

	for (int i = 0; i < BatchSize; i++) 
	{
		glBindTexture(GL_TEXTURE_2D, batchBuffer[i].texture);
		glDrawArrays(GL_TRIANGLES, batchBuffer[i].offset, batchBuffer[i].numVertices);
		*draw_calls+=1;
	}

	glBindVertexArray(0);
}
void dispose_pakki(Pakki::PakkiContext* context)
{
	Pakki::disposepakki(context);
	if (vao != 0) {
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
	if (vbo != 0) {
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}
}

#undef max
#define MAXWIDTH 1024

void load_characters(Pakki::Font *font)
{
	FT_Library  library;
	FT_Face face;
	FT_Error error = FT_Init_FreeType(&library);
	fatalerror(!error);
	error = FT_New_Face(library, "ProggyClean.ttf", 0, &face);
	assert(!error);

	error = FT_Set_Pixel_Sizes(
		face,   /* handle to face object */
		0,      /* pixel_width           */
		15);   /* pixel_height          */
	assert(!error);


	FT_GlyphSlot g = face->glyph;


	font->w = 0;
	font->h = 0;
	unsigned int roww = 0;
	unsigned int rowh = 0;


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


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &font->txtid);
	glBindTexture(GL_TEXTURE_2D, font->txtid);
	//font->txtid = 1;
	//glUniform1i(uniform_tex, 0);
	if ((glGetError()) != GL_NO_ERROR)
	{
		fatalerror(false);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, font->w, font->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);// later put the texture in
	if ((glGetError()) != GL_NO_ERROR)
	{
		fatalerror(false);
	}

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);


	int ox = 0;
	int oy = 0;

	rowh = 0;


	for (uint32_t i = 32; i < 128; i++) {
		if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
			fatalerror(false);
		}

		if (ox + g->bitmap.width + 1 >= MAXWIDTH) {
			oy += rowh;
			rowh = 0;
			ox = 0;
		}
		if (g->bitmap.buffer)
		{
			struct data
			{
				unsigned char r;
				unsigned char g;
				unsigned char b;
				unsigned char a;
			};

			data* d = (data*)malloc(sizeof(data) * g->bitmap.width* g->bitmap.rows);
			unsigned char* pointerdata = g->bitmap.buffer;
			for (uint32_t i = 0; i < g->bitmap.width* g->bitmap.rows; i++)
			{ 
				d[i] = data{ 255,255,255, *pointerdata/* > (unsigned char)50? (unsigned char)255:(unsigned char)0*/ };
				pointerdata++;
			}
			glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_RGBA, GL_UNSIGNED_BYTE, d);
			//glGenerateMipmap(GL_TEXTURE_2D);
			free(d);
		}

		font->character[i].ax = g->advance.x >> 6;
		font->character[i].ay = g->advance.y >> 6;

		font->character[i].bw = g->bitmap.width;
		font->character[i].bh = g->bitmap.rows;

		font->character[i].bl = g->bitmap_left;
		font->character[i].bt = g->bitmap_top;

		font->character[i].tx = ox / (float)(font->w);
		font->character[i].ty = oy / (float)(font->h);

		rowh = std::max<unsigned int>(rowh, g->bitmap.rows);
		ox += g->bitmap.width + 1;
	}
	FT_Done_Face(face);
	FT_Done_FreeType(library);
	//font->txtid = FileSystem::load_sprite(linux_pingu, engine, false).ID;
	glBindTexture(GL_TEXTURE_2D, last_texture);
}
#undef MAXWIDTH 
#endif
//todo sprite id

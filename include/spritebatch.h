#pragma once
#include <projectDefinitions.h>
#include <Vertex.h>
//#include <dybamic_array.h>
#ifdef P_WINDOWS
#include <glad/glad.h>
#endif // P_WINDOWS
#ifdef P_ANDROID
#include <GLES2/gl2.h>
#endif //P_ANDROID
//#include <dybamic_array.h>
#include <arrayD.h>
#include <texture.h>
#include <memsebug.h>
#include <shader.h>
#include<spritecache.h>

struct Glyph
{
	GLuint texture;
	float depth;
	Vertex topLeft;
	Vertex bottomLeft;
	Vertex topRight;
	Vertex bottomRight;
};
struct RenderBatch
{
	GLuint offset;
	GLuint numVertices;
	GLuint texture;
};



struct SpriteBatch
{
	GLuint						vao;
	GLuint						vbo;
	GLuint						ebo;
	dynamicArray<Glyph*>		sortingGlyphs;
	dynamicArray<Glyph>			glyphs;
	//dynamicArray<RenderBatch>	renderbatches;
	dynamicArray<Vertex>		VertexBuffer;
	int							numVerts;
	int							bindableTextures[30]{ 0 };
	int							bindtextures[30]{ 0 };
	int							numTexturesToBind;
};

void init_batch(SpriteBatch* batch);
void begin(SpriteBatch* batch);  //clear vectors and get ready for drawing
void push_to_batch(SpriteBatch* batch, const glm::vec4* destinationRectangle, const glm::vec4* uvRectangle, GLuint texture, const Color* color, float depth,float angle = 0);
void post_batch_process(SpriteBatch* batch);
#ifdef P_WINDOWS
int render_batch(SpriteBatch *batch, Shader* shader, spriteCache* cache, glm::mat4x4* camMatrix);
#endif // P_WINDOWS
#ifdef P_ANDROID
extern void render_batch(SpriteBatch *batch);
#endif // P_WINDOWS
void dispose_batch(SpriteBatch *batch);

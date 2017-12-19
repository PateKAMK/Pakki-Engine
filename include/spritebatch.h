#pragma once
#include <projectDefinitions.h>
#include <Vertex.h>
#include <dybamic_array.h>
#ifdef P_WINDOWS
#include <glad/glad.h>
#endif // P_WINDOWS
#ifdef P_ANDROID
#include <GLES2/gl2.h>
#endif //P_ANDROID
#include <dybamic_array.h>
typedef struct
{
	GLuint texture;
	float depth;
	Vertex topLeft;
	Vertex bottomLeft;
	Vertex topRight;
	Vertex bottomRight;
}Glyph;
typedef struct
{
	GLuint offset;
	GLuint numVertices;
	GLuint texture;
}RenderBatch;

typedef struct
{
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	Glyph** dynArrSortingGlyphs;
	Glyph* dynArrGlyphs;
	RenderBatch* dynArrRenderBatches;
}SpriteBatch;

extern void init_batch(SpriteBatch* batch);
extern void begin(SpriteBatch* batch);  //clear vectors and get ready for drawing
extern void push_to_batch(SpriteBatch* batch, const glm::vec4* destinationRectangle, const glm::vec4* uvRectangle, GLuint texture, const Color* color, float depth,float angle = 0);
extern void post_batch_process(SpriteBatch* batch);
#ifdef P_WINDOWS
extern int render_batch(SpriteBatch *batch);
#endif // P_WINDOWS
#ifdef P_ANDROID
extern void render_batch(SpriteBatch *batch);
#endif // P_WINDOWS
extern void dispose_batch(SpriteBatch *batch);
// todo p��t� miten handlaat v�rin
// check ett� sorting toimii ja tee eri sorttaus mahdollisuuksia
//todo k�yt� ebo ja v�henn� vertexsi�

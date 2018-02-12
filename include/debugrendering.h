#pragma once
#include <projectDefinitions.h>
#ifdef P_WINDOWS
#include <glad/glad.h>
#endif // P_WINDOWS
#include <shader.h>
#include <Vertex.h>
#include <memsebug.h>

typedef struct
{
	glm::vec2 pos;
}DebugVertex;

typedef struct
{
	GLuint vao;
	GLuint vbo;
	GLuint ibo;
	Shader m_prog;
	int _numElements;
	DebugVertex* dyArrDeVert;
	GLuint* dyArrIndi;
}DebugRenderer;

extern void init_debug_renderer(DebugRenderer* drenderer);
extern void draw_debug_line(DebugRenderer* drenderer, const float x, const float y, const float x2, const float y2);
extern void render_debug_lines(DebugRenderer* drenderer, glm::mat4x4* cameramatrix);
extern void dispose_debug_renderer(DebugRenderer* drenderer);
extern void populate_debugrender_buffers(DebugRenderer* drenderer);
extern void draw_debug_box(DebugRenderer* drenderer, const float x, const float y, const float w, const float h, float angle);
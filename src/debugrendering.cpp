#include "debugrendering.h"
#include <glm/glm/common.hpp>
#include <dybamic_array.h>
#include <cstring>
#ifdef P_WINDOWS
namespace {
	const char* VERT_SRC = R"(#version 130
	//The vertex shader operates on each vertex
	//input data from the VBO. Each vertex is 2 floats
	in vec2 vertexPosition;
	in vec4 vertexColor;
	out vec2 fragmentPosition;
	out vec4 fragmentColor;
	uniform mat4 P;
	void main() {
		//Set the x,y position on the screen
		gl_Position.xy = (P * vec4(vertexPosition, 0.0, 1.0)).xy;
		//the z position is zero since we are in 2D
		gl_Position.z = 0.0;
    
		//Indicate that the coordinates are normalized
		gl_Position.w = 1.0;
    
		fragmentPosition = vertexPosition;
    
		fragmentColor = vertexColor;
	})";
	const char* FRAG_SRC = R"(#version 130
	//The fragment shader operates on each pixel in a given polygon
	in vec2 fragmentPosition;
	in vec4 fragmentColor;
	//This is the 3 component float vector that gets outputted to the screen
	//for each pixel.
	out vec4 color;
	void main() {
		color = fragmentColor;
	})";
}
#endif
#ifdef P_ANDROID
namespace {
    const char* VERT_SRC = R"(#version 300 es
	//The vertex shader operates on each vertex
	//input data from the VBO. Each vertex is 2 floats
	in vec2 vertexPosition;
	in vec4 vertexColor;
	out vec2 fragmentPosition;
	out vec4 fragmentColor;
	uniform mat4 P;
	void main() {
		//Set the x,y position on the screen
		gl_Position.xy = (P * vec4(vertexPosition, 0.0, 1.0)).xy;
		//the z position is zero since we are in 2D
		gl_Position.z = 0.0;

		//Indicate that the coordinates are normalized
		gl_Position.w = 1.0;

		fragmentPosition = vertexPosition;

		fragmentColor = vertexColor;
	})";
    const char* FRAG_SRC = R"(#version 300 es
	//The fragment shader operates on each pixel in a given polygon
    precision mediump float;
	in vec2 fragmentPosition;
	in vec4 fragmentColor;
	//This is the 3 component float vector that gets outputted to the screen
	//for each pixel.
	out vec4 color;
	void main() {
		color = fragmentColor;
	})";
}
#endif
inline glm::vec2 rotatePoint(const glm::vec2& point, float angle)
{
	if (!angle)return point;
	glm::vec2 newV;
	newV.x = point.x * cos(angle) - point.y *sin(angle);
	newV.y = point.x * sin(angle) + point.y *cos(angle);
	return newV;
}
#ifdef P_WINDOWS
void init_debug_renderer(DebugRenderer* drenderer)
{
	memset(drenderer, 0, sizeof(DebugRenderer));
	GLuint vert = compile_shader(GL_VERTEX_SHADER, VERT_SRC);
	fatalerror(vert);
	GLuint frag = compile_shader(GL_FRAGMENT_SHADER, FRAG_SRC);
	fatalerror(frag);
	drenderer->m_prog.program = glCreateProgram();
	add_attribute(&drenderer->m_prog, "vertexPosition");
	add_attribute(&drenderer->m_prog, "vertexColor");
	glAttachShader(drenderer->m_prog.program, vert);
	glAttachShader(drenderer->m_prog.program, frag);
	fatalerror(link_shader(&drenderer->m_prog, vert, frag));

	glGenVertexArrays(1, &(drenderer->vao));
	glGenBuffers(1, &(drenderer->vbo));
	glGenBuffers(1, &(drenderer->ibo));

	fatalerror(drenderer->vao > 0 || drenderer->vbo > 0 || drenderer->ibo > 0);


	glBindVertexArray(drenderer->vao);
	glBindBuffer(GL_ARRAY_BUFFER, drenderer->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drenderer->ibo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, pos));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, color));
	glBindVertexArray(0);


	allocate_new_array<DebugVertex>(&(drenderer->dyArrDeVert));
	allocate_new_array<GLuint>(&(drenderer->dyArrIndi));

}
#endif //win
#ifdef P_ANDROID
void init_debug_renderer(DebugRenderer* drenderer)
{
	memset(drenderer, 0, sizeof(DebugRenderer));
	GLuint vert = compile_shader(GL_VERTEX_SHADER, VERT_SRC);
	fatalerror(vert);
	GLuint frag = compile_shader(GL_FRAGMENT_SHADER, FRAG_SRC);
	fatalerror(frag);
	drenderer->m_prog.program = glCreateProgram();
	add_attribute(&drenderer->m_prog, "vertexPosition");
	add_attribute(&drenderer->m_prog, "vertexColor");
	glAttachShader(drenderer->m_prog.program, vert);
	glAttachShader(drenderer->m_prog.program, frag);
	fatalerror(link_shader(&drenderer->m_prog, vert, frag));

	//glGenVertexArrays(1, &(drenderer->vao));
	glGenBuffers(1, &(drenderer->vbo));
	glGenBuffers(1, &(drenderer->ibo));

	fatalerror(drenderer->vbo > 0 || drenderer->ibo > 0);

	/*glBindBuffer(GL_ARRAY_BUFFER, drenderer->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drenderer->ibo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, pos));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, color));


	glBindVertexArray(0);*/
	allocate_new_array<DebugVertex>(&(drenderer->dyArrDeVert));
	allocate_new_array<GLuint>(&(drenderer->dyArrIndi));

}
#endif // P_ANDROID

void draw_debug_line(DebugRenderer* drenderer, const glm::vec2* a, const glm::vec2* b, const Color* color)
{
	GLuint size = get_array_size<DebugVertex>(drenderer->dyArrDeVert);
	resize_array<DebugVertex>(&(drenderer->dyArrDeVert),size + 2);
	drenderer->dyArrDeVert[size].pos = *a;
	drenderer->dyArrDeVert[size].color = *color;
	drenderer->dyArrDeVert[size +1].pos = *b;
	drenderer->dyArrDeVert[size + 1].color = *color;
	push_back_dyn_array<GLuint>(&(drenderer->dyArrIndi), &size);
	size++;
	push_back_dyn_array<GLuint>(&(drenderer->dyArrIndi), &size);

}
void draw_debug_box(DebugRenderer* drenderer, const glm::vec4* destRect,const Color* color,float angle)
{
	GLuint size = get_array_size<DebugVertex>(drenderer->dyArrDeVert);
	resize_array<DebugVertex>(&drenderer->dyArrDeVert, size + 4);

	glm::vec2 halfDims(destRect->z / 2.f, destRect->w / 2.f);

	glm::vec2 t1(-halfDims.x, halfDims.y);
	glm::vec2 t2(-halfDims.x, -halfDims.y);
	glm::vec2 t3(halfDims.x, -halfDims.y);
	glm::vec2 t4(halfDims.x, halfDims.y);

	glm::vec2 positionOffSet(destRect->x, destRect->y);

	//if(angle == 0)
	//{
		drenderer->dyArrDeVert[size].color = *color;
		drenderer->dyArrDeVert[size].pos = rotatePoint(t1,angle) + halfDims + positionOffSet;
		drenderer->dyArrDeVert[size + 1].color = *color;
		drenderer->dyArrDeVert[size + 1].pos = rotatePoint(t2, angle) + halfDims + positionOffSet;
		drenderer->dyArrDeVert[size + 2].color = *color;
		drenderer->dyArrDeVert[size + 2].pos = rotatePoint(t3, angle) + halfDims + positionOffSet;
		drenderer->dyArrDeVert[size + 3].color = *color;
		drenderer->dyArrDeVert[size + 3].pos = rotatePoint(t4, angle) + halfDims + positionOffSet;
	//}
	push_back_dyn_array<GLuint>(&drenderer->dyArrIndi, &size);
	size++;
	push_back_dyn_array<GLuint>(&drenderer->dyArrIndi, &size);
	push_back_dyn_array<GLuint>(&drenderer->dyArrIndi, &size);
	size++;
	push_back_dyn_array<GLuint>(&drenderer->dyArrIndi, &size);
	push_back_dyn_array<GLuint>(&drenderer->dyArrIndi, &size);
	size++;
	push_back_dyn_array<GLuint>(&drenderer->dyArrIndi, &size);
	push_back_dyn_array<GLuint>(&drenderer->dyArrIndi, &size); 
	size -= 3;
	push_back_dyn_array<GLuint>(&drenderer->dyArrIndi, &size);

}

void populate_debugrender_buffers(DebugRenderer* drenderer)
{
	GLuint vertSize = get_array_size<DebugVertex>(drenderer->dyArrDeVert);
	glBindBuffer(GL_ARRAY_BUFFER, drenderer->vbo);
	glBufferData(GL_ARRAY_BUFFER, vertSize * sizeof(DebugVertex), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize * sizeof(DebugVertex), drenderer->dyArrDeVert);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint indiciesSize = get_array_size<GLuint>(drenderer->dyArrIndi);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drenderer->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indiciesSize * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indiciesSize * sizeof(GLuint), drenderer->dyArrIndi);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	drenderer->_numElements = indiciesSize;
	clear_array<DebugVertex>(drenderer->dyArrDeVert);
	clear_array<GLuint>(drenderer->dyArrIndi);
}
#ifdef P_WINDOWS
void dispose_debug_renderer(DebugRenderer* drenderer)
{
	if (drenderer->vao)
	{
		glDeleteVertexArrays(1, &(drenderer->vao));
	}
	if (drenderer->vbo)
	{
		glDeleteBuffers(1, &(drenderer->vao));
	}
	if (drenderer->ibo)
	{
		glDeleteBuffers(1, &(drenderer->ibo));
	}
	dispose_shader(&(drenderer->m_prog));
	free_dyn_array<DebugVertex>(drenderer->dyArrDeVert);
	free_dyn_array<GLuint>(drenderer->dyArrIndi);
}
void render_debug_lines(DebugRenderer* drenderer,glm::mat4x4* cameramatrix)
{
	use_shader(&drenderer->m_prog);
	set_matrix(&drenderer->m_prog, "P", cameramatrix);
	glLineWidth(0.5);
	glBindVertexArray(drenderer->vao);
	glDrawElements(GL_LINES, drenderer->_numElements, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	unuse_shader(&drenderer->m_prog);
}
#endif // P_WINDOWS
#ifdef P_ANDROID
void dispose_debug_renderer(DebugRenderer* drenderer)
{

	if (drenderer->vbo)
	{
		glDeleteBuffers(1, &(drenderer->vao));
	}
	if (drenderer->ibo)
	{
		glDeleteBuffers(1, &(drenderer->ibo));
	}
	dispose_shader(&(drenderer->m_prog));
	free_dyn_array<DebugVertex>(drenderer->dyArrDeVert);
	free_dyn_array<GLuint>(drenderer->dyArrIndi);
}
void render_debug_lines(DebugRenderer* drenderer, glm::mat4x4* cameramatrix)
{
	use_shader(&drenderer->m_prog);
	set_matrix(&drenderer->m_prog, "P", cameramatrix);
	glLineWidth(1);

	glBindBuffer(GL_ARRAY_BUFFER, drenderer->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drenderer->ibo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, pos));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(DebugVertex), (void*)offsetof(DebugVertex, color));


	//glBindVertexArray(0); */
	glDrawElements(GL_LINES, drenderer->_numElements, GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);
	unuse_shader(&drenderer->m_prog);
}

#endif // P_ANDROID
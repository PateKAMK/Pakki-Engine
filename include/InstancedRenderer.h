#pragma once
#include <shader.h>
#include <glad/glad.h>
#include <arrayD.h>
namespace InRender
{
	struct vec2
	{
		float x;
		float y;
	};
	
	struct inVertex
	{
		vec2 pos;
		float scale;
		//vec2 uv;
	};


	struct instaRenderer
	{
		Shader					instaShader;
		GLuint					vao;
		GLuint					position;
		GLuint					vertex;
		GLuint					uv;
		dynamicArray<inVertex>	positionbuffer;
		dynamicArray<vec2>		uvBuffer;
		int						num_items;
	};

	void init_renderer(instaRenderer *in);
	void render(instaRenderer* in, GLuint tex, glm::mat4x4* camMatrix);
	void create_buffers(instaRenderer* in);
	void push_to_renderer(instaRenderer* in, const float* x, const float* y,const float* scale, const float* uv1, const float* uv2, const float* uv3, const float* uv4);
}
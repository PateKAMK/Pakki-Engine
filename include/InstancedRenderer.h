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
		vec2 uv;
	};


	struct instaRenderer
	{
		Shader					instaShader;
		GLuint					vao;
		GLuint					vbo;
		dynamicArray<inVertex>	vertexbuffer;
	};

	void init_renderer(instaRenderer *in);
}
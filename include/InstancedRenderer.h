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
	};

	void init_renderer(instaRenderer *in);
}
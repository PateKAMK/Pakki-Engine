#pragma once
#include "projectDefinitions.h"
#ifdef P_WINDOWS
#include <glad/glad.h>
#endif // P_WINDOWS
#include "glm/glm/common.hpp"
#ifdef P_ANDROID
#include <GLES2/gl2.h>
#endif
#include <ctype.h>
struct Color
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

struct Vertex
{
	glm::vec2 position;
	glm::vec2 uv; //soon tm TODO
	Color color;
};

/*struct UV
{
	float u;
	float v;
};

struct Vertex
{
	Position position;
	Color color;
	//uv texture coordinates
	UV uv;

	void setPosition(float x, float y)
	{
		position.x = x;
		position.y = y;
	}
	void setPosition(const glm::vec2& pos)
	{
		this->setPosition(pos.x,pos.y);
	}
	void setColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
	{
		color.r = r;
		color.g = g;
		color.b = b;
		color.a = a;
	}
	void setColor(const Color& color)
	{
		this->setColor(color.r,color.g,color.b,color.a);
	}
	void setUv(float u, float v)
	{
		uv.v = v;
		uv.u = u;
	}
	};*/
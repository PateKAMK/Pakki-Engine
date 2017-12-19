#pragma once
#include <projectDefinitions.h>
#ifdef P_WINDOWS
#include <glad/glad.h>
#endif // P_WINDOWS

typedef struct
{
	GLuint ID;
	int widht;
	int height;
	int channels;
} Texture;

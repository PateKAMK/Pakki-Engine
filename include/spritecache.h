#pragma once

#include<map>
#include<string>
#include <texture.h>

struct BindedTextures
{
	BindedTextures() : _max(0)
	{
		ids = (int32_t*)calloc(32, sizeof(uint32_t));
		textures = (Texture*)calloc(32, sizeof(Texture));
	};
	~BindedTextures()
	{
		free(ids);
		free(textures);
	}

	int32_t* ids;
	Texture* textures;
	uint32_t _max;
};


struct spriteCache
{
	spriteCache() = default;
	std::map<std::string, Texture> sprites;
	BindedTextures bindedTextures;
};



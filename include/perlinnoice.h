#pragma once
#include <InstancedRenderer.h>
#include <arrayD.h>
#include <debugrendering.h>
typedef unsigned char tileId;


struct v2
{
	float x;
	float y;
};

#define DIMENSION 300//120

struct terrain
{
	v2					pos;
	dynamicArray<v2>	destroyedTerrain;
	int					xP;
	int					yP;
};

struct terrainHandler
{
	terrain*				currentTerrainsHandles[3 + 3 + 3];
	uint8_t*				meshes[3 + 3 + 3];
	uint8_t*				meshAllocator;
	float					scale;
	dynamicArray<terrain*>	allTerrains;
	pool<terrain, 200>		terrainAllocator;
	bool					update;
};
void init_terrains(terrainHandler* ter, float worldX, float worldY);
void dispose_terrain(terrainHandler* ter);
void updateTerrain(terrainHandler* ter, InRender::instaRenderer* rend, v2 worldPos,DebugRenderer* deb);

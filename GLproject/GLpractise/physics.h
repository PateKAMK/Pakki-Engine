#pragma once
#include "../Header.h"

struct object;
struct tree;




struct vec2
{
    float	x;
    float	y;
};


struct worldScene
{

    pool<object>			objectAllocator;
    dynamicArray<object*>	objects;
    tree*					treeAllocator;
    uint32_t				treeAllocatorSize;
    dynamicArray<object*>	updateobjects;
    vec2					gravity;
    vec2					worldCentrePos;
    vec2					worldWidht;
};

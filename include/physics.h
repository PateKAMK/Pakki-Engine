#pragma once
#include "arrayD.h"
#include <spritebatch.h>

    struct keys;
namespace PakkiPhysics
{
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


    void init_scene(worldScene* scene, vec2 worldCentrePos, vec2 worldDimensions, uint32_t texID);
    void update_objects(worldScene& scene, float dt,keys* k); // keys temp
    void draw_objects(worldScene* scene, SpriteBatch* batch);
    void dispose_scene(worldScene* scene);
}

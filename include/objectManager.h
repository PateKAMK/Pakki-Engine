#pragma once
#include <ctype.h>
#include <spritebatch.h>
#include <arrayD.h>

namespace ObjectManager
{
	typedef uint32_t	state;
	typedef uint32_t	userID;
	struct vec2
	{
		float	x;
		float	y;
	};
	struct vec4
	{
		float	x;
		float	y;
		float	z;
		float	w;
	};
	struct drawdata
	{
		vec4		uv;
		Color		color;
		uint32_t	spriteid;
		int			level;
	};
	struct object
	{
		vec2		pos;
		vec2		dim;
		state		s;
		userID		i;
		drawdata*	drawPtr;
	};
	struct tree
	{
		unsigned int				level;
		dynamicArray<object*>		objects;
		tree*						treebuffer;
		vec2						pos;
		vec2						dim;
	};
#define POOLSIZE 255
	struct collisiontable
	{
		object* obj1;
		object* obj2;
	};
	struct objects
	{
		pool<object, POOLSIZE>			objectPool;
		pool<drawdata, POOLSIZE>		drawPool;
		tree							objTree;
		vec2							worldMid;
		vec2							worldDims;
		tree*							treeAllocator;
		uint32_t						allocatorSize;
		dynamicArray<collisiontable>	hardCollisionBuffer;//init
		dynamicArray<collisiontable>	softCollisionBuffer;
		dynamicArray<object*>			updateBuffer;
		dynamicArray<object*>			generalcollisionBuffer;
		dynamicArray<object*>			drawAbleOnes;
	};

	void init_objects(objects* objs, vec2 worldMid, vec2 dimensions);
	void get_collisions(tree* node, dynamicArray<object*>* buffer, object* obj);
	void dispose_objects(objects* objs);
}

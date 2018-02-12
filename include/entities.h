#pragma once
#define OUT_OF_DATE
#ifndef OUT_OF_DATE


#include <spritebatch.h>
#include <debugrendering.h>
//#include <filesystem.h>
#include <memsebug.h>

typedef int objID;
typedef struct
{
	float	x;
	float	y;
} vec2;

typedef struct
{
	float	x;
	float	y;
	float	z;
	float	w;
} vec4;

typedef struct
{
	objID			id;
	unsigned int	textureID;
	vec2			position;
	vec2			dimensions;
	vec4			uv;
	float			angle;
} Header;

typedef struct
{
	float	alpha;
	vec2	velocity;
	int		hp;//10
} Alien;

typedef struct
{
	vec2	velocity;
	int		hp; // 100
	float	lastSpawnSmoke;
} Player;

typedef struct
{
	vec2			velocity;
	unsigned int	id;
} Projectile;

typedef struct
{
	vec2			velocity;
	bool			spawned;
} Starfield;

typedef struct
{
	float		alpha; // same offset as meteor hack
	vec2		velocity;
	int			hp;
	int			level;
	float		lastspawnedRock;
} Meteor;


typedef struct
{
	Header header;
	bool alive;
	union
	{
		Alien		alien;
		Player		player;
		Projectile	projectile;
		Starfield	starfield;
		Meteor		meteor;
	};
} Entitity;

struct tree
{

	unsigned int	level;
	Entitity**		objects;
	unsigned int	numObjects;
	tree*			nodes[4];
	vec2			position;
	vec2			dimensions;
};


struct engine;
struct keys;
typedef struct
{
	vec2			pos;
	vec2			dim;
	vec2			velocity;
	vec4			uv;
	float			life;
	float			startlife;
	unsigned int	textureid;
} Particle;


struct Entities
{
	unsigned int	numEntities;
	tree			data;
	bool			debuglines;
	Entitity		objects[100];
	Particle		particles[1000];
};

void init_entities(Entities* objects, engine* eng, DebugRenderer* debuggerR); // ota debugpois
void push_objects_to_batch(Entities* objects,SpriteBatch* batch,DebugRenderer* dbug);
void update_objects(Entities* objects, keys* key);
#endif // !OUT_OF_DATE

//void tester();
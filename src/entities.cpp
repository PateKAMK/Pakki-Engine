#include "entities.h"
#ifndef OUT_OF_DATE

#include<string.h>
#include<dybamic_array.h>
#include <filesystem.h>
#include <engine.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#ifndef P_ANDROID
#include <soundsystem.h>
#endif
enum objectids
{
	player = 0,
	alien,
	projectile,
	starfieldid,
	meteor,
	maxObjIDs
};

static int allo = 0;
static DebugRenderer* debuggerR = NULL;
static bool* dbug = NULL;
void create_new_node(tree* node, unsigned int level, vec2 pos,vec2 dims)
{
	memset(node, 0, sizeof *node);
	node->level = level;
	node->position = pos;
	node->level = level;
	node->dimensions = dims;
	allocate_new_array<Entitity*>(&node->objects,10);
	allo++;
	if(*dbug)
	{	
		glm::vec4 rec(pos.x - dims.x, pos.y - dims.y,dims.x * 2,dims.y * 2);
		Color c{ 200,100,255,255 };
		draw_debug_box(debuggerR, &rec, &c, 0);
	}

	//printf("new level %d , position is %f , %f \n", level,pos.x,pos.y);
}
static engine* engptr;
static const float worldXpos = 1000;
static const float worldWidht = 500;

void add_player(Entities* objects)
{
	Entitity* player = NULL;
	for(int i = 0; i < 100;i++)
	{
		if(!objects->objects[i].alive)
		{
			player = &objects->objects[i];
			break;
		}
	}
	//Entitity* player = &objects->objects[objects->numEntities++];
	memset(player, 0, sizeof(Entitity));
	player->header.id = objectids::player;
	Texture tex = FileSystem::load_sprite(spaceShips_009, engptr);
	player->header.textureID = tex.ID;
	player->header.dimensions = vec2{ (float)(tex.widht * 0.5), (float)(tex.height * 0.5) };
	player->header.uv = vec4{ 0,0,1,1};
	player->header.position = vec2{ worldXpos ,worldXpos - 300 };
	player->player.hp = 100;
	player->alive = true;
	objects->numEntities++;
}
void add_starfield(Entities* objects,vec2 velocity, vec2 position,vec2 dimensions,unsigned int picture)
{
	Entitity* field = NULL;
	for (int i = 0; i < 100; i++)
	{
		if (!objects->objects[i].alive)
		{
			field = &objects->objects[i];
			break;
		}
	}
	//Texture tex = FileSystem::load_sprite(picture, engptr);
	field->header.dimensions = dimensions;
	field->header.uv = vec4{ 0.f,0.f,1.f,1.f};
	field->header.position = position;
	field->header.textureID = picture;
	field->header.id = starfieldid;
	field->starfield.velocity = velocity;
	field->starfield.spawned = false;
	field->alive = true;
	objects->numEntities++;
}
void init_entities(Entities* objects, engine* eng, DebugRenderer* debuggerR)
{
    engptr = eng;
	dbug = &objects->debuglines;
	int starid = FileSystem::load_sprite(starfield, engptr).ID;
	int temp = FileSystem::load_sprite(spaceEffects_009, engptr).ID;
    temp = 0;
	memset(objects, 0, sizeof *objects);
	//create_new_node(&objects->data, 0, vec2{ 100.f , 100.f}, vec2{ 50.f , 50.f });
	add_player(objects);
	add_starfield(objects, vec2{ 0.f,-3.f }, vec2{ 1000.f,1000.f }, vec2{800,800}, starid);
	add_starfield(objects, vec2{ 0.f,-1.7f }, vec2{ 1000.f,1000.f }, vec2{ 600,600 }, starid);
	add_starfield(objects, vec2{ 0.f,-1.f }, vec2{ 1000.f,1000.f }, vec2{ 500,500 }, starid);
	::debuggerR = debuggerR;
	create_new_node(&objects->data, 0, vec2{ worldXpos ,worldXpos }, vec2{ worldWidht ,worldWidht });

	time_t t;
	srand((unsigned)time(&t));
}

#define maxLevels 5

#define maxObjects 10

// indexing 
/*
[1][0]
[2][3]
*/


void clear_tree(tree* node)
{
	for(int i = 0;i < 4;i++)
	{
		if(node->nodes[i] != NULL)
		{
			clear_tree(node->nodes[i]);
			free(node->nodes[i]);
			node->nodes[i] = NULL;
		}
	}
	free_dyn_array<Entitity*>(node->objects);
	node->objects = NULL;
	allo--;
}

void split_tree(tree* node)
{
	float subWidht = node->dimensions.x / 2;
	float subHeight= node->dimensions.y / 2;

	node->nodes[0] = (tree*)malloc(sizeof(tree));
	create_new_node(node->nodes[0], node->level + 1, 
		vec2{ node->position.x + subWidht,node->position.y + subHeight },
		vec2{subWidht,subHeight});
	
	node->nodes[1] = (tree*)malloc(sizeof(tree));
	create_new_node(node->nodes[1], node->level + 1,
		vec2{ node->position.x - subWidht,node->position.y + subHeight },
		vec2{ subWidht,subHeight });

	node->nodes[2] = (tree*)malloc(sizeof(tree));
	create_new_node(node->nodes[2],node->level + 1,
		vec2{ node->position.x - subWidht,node->position.y - subHeight },
		vec2{ subWidht,subHeight });

	node->nodes[3] = (tree*)malloc(sizeof(tree));
	create_new_node(node->nodes[3], node->level + 1,
		vec2{ node->position.x + subWidht,node->position.y - subHeight },
		vec2{ subWidht,subHeight });

}

int get_index(tree* node,vec2 pos, vec2 dim)
{
	int index = -1;

	//if true it can fit to bottom place
	bool bot =  pos.y - dim.y  > node->position.y - node->dimensions.y 
		&& pos.y + dim.y <  node->position.y;
	//if true it can fit to top place
	bool top = pos.y - dim.y  > node->position.y 
		&& pos.y + dim.y <  node->position.y + node->dimensions.y;


	if(pos.x - dim.x  > node->position.x - node->dimensions.x
		&& pos.x + dim.x <  node->position.x)
	{	
		if(top)
		{
			index = 1;
		}
		else if(bot)
		{
			index = 2;
		}
	}
	else if(pos.x - dim.x  > node->position.x
		&& pos.x + dim.x <  node->position.x + node->dimensions.x)
	{
		if(top)
		{
			index = 0;
		}
		else if(bot)
		{
			index = 3;
		}
	}
	return index;
}
void insert_to_tree(tree* node,Entitity* object)
 {
	if(node->nodes[0] != NULL)
	{
		int index = get_index(node, object->header.position, object->header.dimensions);
		if (index != -1)
		{
			insert_to_tree(node->nodes[index], object);
			return;
		}
	}
	Entitity** emblaced = emblace_back<Entitity*>(&node->objects);
	(*emblaced) = object;
	node->numObjects++;


	if(node->numObjects >= maxObjects && node->level < maxLevels)
	{
		if(node->nodes[0] == NULL)
		{
 			split_tree(node);
		}
		int i = 0;
		while(i < node->numObjects)
		{
			int index = get_index(node, node->objects[i]->header.position, node->objects[i]->header.dimensions);
			if(index != -1)
			{
				insert_to_tree(node->nodes[index], node->objects[i]);
				node->objects[i] = 0;
				node->numObjects--;
				fast_remove_element_array<Entitity*>(node->objects, i);
			}
			else
			{
				i++;
			}
		}
	}
}
// create dyarray as buffer
Entitity** get_collisions(tree* node,Entitity** buffer,Entitity* object)
{
	int index = get_index(node, object->header.position, object->header.dimensions);
	if(index != -1 && node->nodes[0] != NULL)
	{
		get_collisions(node->nodes[index],buffer, object);
	}
	//int sizeoftree = get_array_size<Entitity*>(node->objects); android error
	for(int i = 0; i < node->numObjects; i++)
	{
		Entitity** emblacer = emblace_back<Entitity*>(&buffer);
		(*emblacer) = node->objects[i];
		
	}
	return buffer;
}

void push_objects_to_batch(Entities* objects, SpriteBatch* batch, DebugRenderer* dbug)
{
	
	for(Entitity* iterator = objects->objects;iterator != &objects->objects[99];iterator++)
	{
		if (!iterator->alive)continue;
		glm::vec4 destRec;
		destRec.x = iterator->header.position.x - iterator->header.dimensions.x;
		destRec.y = iterator->header.position.y - iterator->header.dimensions.y;
		destRec.z = iterator->header.dimensions.x * 2;
		destRec.w = iterator->header.dimensions.y * 2;
		glm::vec4 uv(iterator->header.uv.x, iterator->header.uv.y, iterator->header.uv.z, iterator->header.uv.w);
		Color color{ (GLubyte)255, (GLubyte)255, (GLubyte)255, iterator->header.id == alien || iterator->header.id == meteor ? (GLubyte)(255 * iterator->alien.alpha) : (GLubyte)255 };

		push_to_batch(batch, &destRec, &uv, iterator->header.textureID, &color, 0, iterator->header.angle);
		if(*(::dbug))
		{
			draw_debug_box(dbug, &destRec, &color, 0);		
		}

	}
	int pp = 0;
	for(Particle* iterator = objects->particles; iterator != &objects->particles[999]; iterator++)
	{
		if (iterator->life <= 0)continue;
		pp++;
		glm::vec4 destRec;
		destRec.x = iterator->pos.x - iterator->dim.x;
		destRec.y = iterator->pos.y - iterator->dim.y;
		destRec.z = iterator->dim.x * 2;
		destRec.w = iterator->dim.y * 2;
		glm::vec4 uv(iterator->uv.x, iterator->uv.y, iterator->uv.z, iterator->uv.w);
		Color color{ (GLubyte)255, (GLubyte)255, (GLubyte)255, (GLubyte)(255 * (iterator->life / iterator->startlife)) };

		push_to_batch(batch, &destRec, &uv, iterator->textureid, &color, 0,0);
	}
	//printf("particles %d\n", pp);
}

bool update_player(Header* base,Player* pl, keys* key, Particle* partis);
bool update_enemy(Header* base, Alien* pl);
bool update_projectile(Header* base, Projectile* pl, tree* data);
bool update_starfield(Header* base, Starfield* pl, Entities *objects);
bool update_meteor(Header* base, Meteor* pl, Entities *objects, Particle* partis);
void createprojectile(Entities* objects,vec2 pos,objectids id)
{
	Entitity* pro = NULL;
	for (int i = 0; i < 999; i++)
	{
		if (!objects->objects[i].alive)
		{
			pro = &objects->objects[i];
			break;
		}
	}
	if (!pro)return;
	objects->numEntities++;
	memset(pro, 0, sizeof(Entitity));
	pro->header.id = objectids::projectile;
	Texture tex = FileSystem::load_sprite(spaceEffects_006, engptr);
	pro->header.textureID = tex.ID;
	pro->header.dimensions = vec2{ (float)(tex.widht * 0.3), (float)(tex.height *  0.3) };
	pro->header.uv = vec4{0,0,1,1 /*248.f / 512.f, 325.f / 512.f + 0.085f, 124.f / 512.f, 90.f / 512.f + 0.12f*/ };
	pro->header.position = vec2{ pos.x , pos.y + (float)(tex.height *  0.05)*2 };
	pro->projectile.velocity = vec2{0.f,6.f};
	pro->projectile.id = id;
	pro->alive = true;
}
#define FIRERATE 0.2f
constexpr float uprate = 1.f / 60.f;
static float shoottimer = 0.0f;
static bool shoot = true;
static float spawntimer = 0.0f;
static float spawncycle = 5.f;
bool collide(const vec2* pos1, const vec2* dim1, const vec2* pos2, const vec2* dim2)
{
	float xDist = std::abs(pos1->x - pos2->x);
	float yDist = std::abs(pos1->y - pos2->y);

	return xDist < dim1->x + dim2->x && yDist < dim1->y + dim2->y;
}
void create_meteor(Entities* objects,vec2 pos,vec2 velocity ,int level)
{
	Entitity* pro = NULL;
	for (int i = 0; i < 100; i++)
	{
		if (!objects->objects[i].alive)
		{
			pro = &objects->objects[i];
			break;
		}
	}
	pro->header.position = pos;
	pro->meteor.velocity = velocity;
	pro->meteor.lastspawnedRock = 0.f;
	pro->header.uv = vec4{ 0,0,1,1 };
	pro->header.id = meteor;
	pro->meteor.level = level;
	if(level == 0)
	{
		pro->meteor.hp = 5;
	}
	if (level == 1)
	{
		pro->meteor.hp = 2;
	}
	pro->meteor.alpha = 1.f;
	pro->alive = true;
	Texture tex = FileSystem::load_sprite(spaceMeteors_004, engptr);
	pro->header.textureID = tex.ID;
	pro->header.dimensions = vec2{ (tex.widht * 0.35f) / (level + 1) ,(tex.height * 0.35f) / (level + 1) };
	objects->numEntities++;
}
void update_objects(Entities* objects, keys* key)
{
	spawntimer += uprate;
	if(spawntimer > spawncycle)
	{
		spawntimer = 0;
		if(spawncycle > 1.f)
		{
			spawncycle *= 0.90;
		}
		int rX = rand() % 400;

		Entitity* pro = NULL;

		for(int i = 0;i < 100;i++)
		{
			if(!objects->objects[i].alive)
			{
				pro = &objects->objects[i];
				break;
			}
		}
		int m = rand() % 20;
		if(m != 9)
		{
			memset(pro, 0, sizeof(Entitity));
			pro->header.id = objectids::alien;
			Texture tex = FileSystem::load_sprite(spaceStation_028, engptr);
			pro->header.textureID = tex.ID;
			pro->header.dimensions = vec2{ (float)(tex.widht * 0.3), (float)(tex.height *  0.3) };
			pro->header.uv = vec4{0.f,0.f,1.f,1.f/* 248.f / 512.f, 325.f / 512.f + 0.085f, 124.f / 512.f, 90.f / 512.f + 0.12f*/ };
			pro->header.position = vec2{ worldXpos  - 200 + rX , worldXpos + 500};
			pro->alien.velocity = vec2{ 0.f, - 2.f };
			pro->alien.hp = 2;
			pro->alive = true;
			pro->alien.alpha = 1.f;
			objects->numEntities++;
		}
		else
		{
			create_meteor(objects, vec2{ worldXpos - 200 + rX , worldXpos + 500 }, vec2{ 0.f,-1.8f }, 0);
		}
		//LOGI("spawning\n");
	}





	clear_tree(&objects->data);
	create_new_node(&objects->data, 0, vec2{ worldXpos ,worldXpos }, vec2{ worldWidht ,worldWidht });
	//Entitity** deleteBuffer;
	//allocate_new_array<Entitity*>(&deleteBuffer,5);
	//int insertamount = 0; //android error
	for (int i = 0; i < 100; i++)
	{
		if (!objects->objects[i].alive || objects->objects[i].header.id == starfieldid)continue;
		insert_to_tree(&objects->data, &objects->objects[i]);
	}



	Entitity** collisionbuffer = NULL;
	allocate_new_array<Entitity*>(&collisionbuffer);
	
	struct collisionBuffer
	{
		Entitity* a;
		Entitity* b;
	};
	collisionBuffer collisiontable[100];//{0}; android error
	unsigned int tableindex = 0;
	for (int i = 0; i < 100; i++)
	{
		Entitity* current = &objects->objects[i];
		if (!current->alive || current->header.id == starfieldid)continue;

		assert(current->header.id != -1 );
		assert(current->alive);
		collisionbuffer = get_collisions(&objects->data, collisionbuffer, current);

		unsigned int collsize = get_array_size<Entitity*>(collisionbuffer);

		for(int j = 0; j < collsize;j++)
		{

			Entitity* collider = collisionbuffer[j];
			if(!collider->alive)
			{
				assert(false);
			}
			if (current == collider) continue;

				if (collide(&current->header.position, &current->header.dimensions, &collider->header.position, &collider->header.dimensions))
				{
					// check table after confirming collision

					bool ishit = true;
					for (int k = 0; k < tableindex; k++) // collision is alredy in table
					{
						if(collisiontable[k].a == current && collisiontable[k].b == collider)
						{
							ishit = false;
							break;
						}
					}
					if(ishit)
					{				
						// if true inset to table and do collisions
						if(tableindex < 100) // todo ota index pois
						{
							
							collisiontable[tableindex].a = collider;
							collisiontable[tableindex++].b = current;
						}
						else
						{
							assert(false);
						}
					}
				}
		}

		clear_array<Entitity*>(collisionbuffer);
	}
	free_dyn_array<Entitity*>(collisionbuffer);
	//printf("collisions %d\n", tableindex);
	int rndpitch = rand() % 10;
	rndpitch = rndpitch > 6 ? rndpitch : 6;

	for(int i = 0; i < tableindex;i++) // do collisions
	{
		if(collisiontable[i].a->header.id == projectile && collisiontable[i].b->header.id == alien)
		{
			if(collisiontable[i].b->alien.alpha == 1.f)
			{
				collisiontable[i].b->alien.hp--;
				collisiontable[i].a->alive = false;
				objects->numEntities--;

			}
		}
		else if(collisiontable[i].a->header.id == alien && collisiontable[i].b->header.id == projectile)
		{
			if (collisiontable[i].a->alien.alpha == 1.f)
			{
				collisiontable[i].a->alien.hp--;
				collisiontable[i].b->alive = false;
				objects->numEntities--;

			}
		}
		/*meteor stuff*/
		else if (collisiontable[i].a->header.id == meteor && collisiontable[i].b->header.id == projectile)
		{
			if (collisiontable[i].a->meteor.alpha == 1.f)
			{
				collisiontable[i].a->meteor.hp--;
				//collisiontable[i].a->meteor.alpha = 0.99f;
				collisiontable[i].b->alive = false;
				objects->numEntities--;

			}

		}
		else if (collisiontable[i].a->header.id == projectile && collisiontable[i].b->header.id == meteor)
		{
			if (collisiontable[i].b->meteor.alpha == 1.f)
			{
				collisiontable[i].b->meteor.hp--;
				//collisiontable[i].b->meteor.alpha = 0.99f;
				collisiontable[i].a->alive = false;
				objects->numEntities--;

			}
		}


		else if (collisiontable[i].a->header.id == alien && collisiontable[i].b->header.id == meteor)
		{
			if (collisiontable[i].a->alien.alpha == 1.f && collisiontable[i].b->meteor.alpha == 1.f)
			{
				float xdist = std::abs(collisiontable[i].a->header.position.x - collisiontable[i].b->header.position.x);
				float ydist = std::abs(collisiontable[i].a->header.position.y - collisiontable[i].b->header.position.y);
				float dist = sqrtf(xdist*xdist + ydist*ydist);
				float colldist = collisiontable[i].a->header.dimensions.x + collisiontable[i].b->header.dimensions.x;

				if(dist < colldist)
				{
					if(collisiontable[i].a->header.position.x < collisiontable[i].b->header.position.x)
					{
						collisiontable[i].a->header.position.x -= 0.1f;
					}
					else
					{
						collisiontable[i].a->header.position.x += 0.1f;
					}
					if (collisiontable[i].a->header.position.y > collisiontable[i].b->header.position.y)
					{
						collisiontable[i].a->header.position.y -= 0.1f;
					}
					else
					{
						collisiontable[i].a->header.position.y += 0.1f;
					}
				}
			}
		}
		else if (collisiontable[i].a->header.id == meteor && collisiontable[i].b->header.id == alien)
		{
			if (collisiontable[i].a->meteor.alpha == 1.f && collisiontable[i].b->alien.alpha == 1.f)
			{
				float xdist = std::abs(collisiontable[i].a->header.position.x - collisiontable[i].b->header.position.x);
				float ydist = std::abs(collisiontable[i].a->header.position.y - collisiontable[i].b->header.position.y);
				float dist = sqrtf(xdist*xdist + ydist*ydist);
				float colldist = collisiontable[i].a->header.dimensions.x + collisiontable[i].b->header.dimensions.x;

				if (dist < colldist)
				{
					if (collisiontable[i].b->header.position.x < collisiontable[i].a->header.position.x)
					{
						collisiontable[i].b->header.position.x -= 0.4f;
					}
					else
					{
						collisiontable[i].b->header.position.x += 0.4f;
					}
					if (collisiontable[i].b->header.position.y > collisiontable[i].a->header.position.y)
					{
						collisiontable[i].b->header.position.y -= 0.4f;
					}
					else
					{
						collisiontable[i].b->header.position.y += 0.4f;
					}
				}
			}
		}
		else if (collisiontable[i].a->header.id == meteor && collisiontable[i].b->header.id == meteor)
		{
			if (collisiontable[i].a->meteor.alpha == 1.f && collisiontable[i].b->meteor.alpha == 1.f)
			{
				float xdist = std::abs(collisiontable[i].a->header.position.x - collisiontable[i].b->header.position.x);
				float ydist = std::abs(collisiontable[i].a->header.position.y - collisiontable[i].b->header.position.y);
				float dist = sqrtf(xdist*xdist + ydist*ydist);
				float colldist = collisiontable[i].a->header.dimensions.x + collisiontable[i].b->header.dimensions.x;

				if (dist < colldist)
				{
					if (collisiontable[i].b->header.position.x < collisiontable[i].a->header.position.x)
					{
						collisiontable[i].b->header.position.x -= 0.4f;
						collisiontable[i].a->header.position.x += 0.4f;

					}
					else
					{
						collisiontable[i].b->header.position.x += 0.4f;
						collisiontable[i].a->header.position.x -= 0.4f;
					}
					if (collisiontable[i].b->header.position.y > collisiontable[i].a->header.position.y)
					{
						collisiontable[i].b->header.position.y -= 0.4f;
						collisiontable[i].a->header.position.y += 0.4f;

					}
					else
					{
						collisiontable[i].b->header.position.y += 0.4f;
						collisiontable[i].a->header.position.y -= 0.4f;
					}
				}
			}
		}
		else if (collisiontable[i].a->header.id == alien && collisiontable[i].b->header.id == alien)
		{
			if (collisiontable[i].a->alien.alpha == 1.f && collisiontable[i].b->alien.alpha == 1.f)
			{
				float xdist = std::abs(collisiontable[i].a->header.position.x - collisiontable[i].b->header.position.x);
				float ydist = std::abs(collisiontable[i].a->header.position.y - collisiontable[i].b->header.position.y);
				float dist = sqrtf(xdist*xdist + ydist*ydist);
				float colldist = collisiontable[i].a->header.dimensions.x + collisiontable[i].b->header.dimensions.x;

				if (dist < colldist)
				{
					if (collisiontable[i].b->header.position.x < collisiontable[i].a->header.position.x)
					{
						collisiontable[i].b->header.position.x -= 0.4f;
						collisiontable[i].a->header.position.x += 0.4f;

					}
					else
					{
						collisiontable[i].b->header.position.x += 0.4f;
						collisiontable[i].a->header.position.x -= 0.4f;
					}
					if (collisiontable[i].b->header.position.y > collisiontable[i].a->header.position.y)
					{
						collisiontable[i].b->header.position.y -= 0.4f;
						collisiontable[i].a->header.position.y += 0.4f;

					}
					else
					{
						collisiontable[i].b->header.position.y += 0.4f;
						collisiontable[i].a->header.position.y -= 0.4f;
					}
				}
			}
		}
	}




	if(!shoot)
	{
		shoottimer += uprate;
		if(shoottimer > FIRERATE)
		{
			shoottimer = 0.0f;
			shoot = true;
		}
	}

	if (key->arrowU && shoot)
	{
		createprojectile(objects,objects->objects->header.position,player);
		shoot = false;
	}

	for (Entitity* iterator = objects->objects; iterator != &objects->objects[99]; iterator++)
	{
		if (!iterator->alive)continue;

		if((iterator->header.position.y > 1000 + 600 || iterator->header.position.y < 1000 - 600) && iterator->header.id != starfieldid)
		{
			iterator->alive = false;
			objects->numEntities--;
			continue;
		}

		switch (iterator->header.id)
		{
		case player:
			if(update_player(&iterator->header, &iterator->player,key,objects->particles))
			{
				iterator->alive = false;
				objects->numEntities--;

			}
			break;
		case alien:
			if(update_enemy(&iterator->header, &iterator->alien))
			{
				iterator->alive = false;
				objects->numEntities--;

			//	printf("ALIEN KILLED");
			}
			break; 
		case projectile:
			if(update_projectile(&iterator->header, &iterator->projectile,&objects->data))
			{
				iterator->alive = false;
				objects->numEntities--;

			}
			break;
		case starfieldid:
			if(update_starfield(&iterator->header, &iterator->starfield,objects))
			{
				iterator->alive = false;
				objects->numEntities--;

			}
			break;
		case meteor:
			if (update_meteor(&iterator->header, &iterator->meteor, objects, objects->particles))
			{
				iterator->alive = false;
				objects->numEntities--;

			}
			break;
		default:
			fatalerror(false);
			break;
		}

	}
	for (Particle* iterator = objects->particles; iterator != &objects->particles[999]; iterator++)
	{
		if (iterator->life <= 0)continue;

		iterator->life -= uprate;
		iterator->pos.x += iterator->velocity.x;
		iterator->pos.y += iterator->velocity.y;
	}


	
}

#define MAX_VEL 7
bool update_player(Header* base, Player* pl, keys* key,Particle* partis)
{
	//input and velocity calculations
	//LOGI("updating player");
	if (pl->hp < 0)
	{
		return true;
	}

	if(key->arrowL)
	{
		pl->velocity.x -= 0.4;
	}
	if (key->arrowR)
	{
		pl->velocity.x += 0.4;
	}

	pl->velocity.x *= 0.95;

	if(std::abs(pl->velocity.x) > MAX_VEL)
	{
		pl->velocity.x = pl->velocity.x < 0 ? -MAX_VEL : MAX_VEL;
	}

	base->position.x += pl->velocity.x;
	/*spawn particles*/
	pl->lastSpawnSmoke += uprate;
	Particle* curpar = NULL;
	if(pl->lastSpawnSmoke > 0.01f)
	{
		for(int k = 0;k < 3;k++)
		{
			for(int i = 0;i < 1000;i++)
			{
				if(partis[i].life <= 0)
				{
					curpar = &partis[i];
				}
			}
			curpar->startlife = 0.5f;
			curpar->pos = base->position;
			curpar->pos.y -= 22;
			curpar->pos.x -= 20;
			int rX = rand() % 40;
			curpar->pos.x += rX;
			float xvel = pl->velocity.x * 0.5;
			curpar->velocity = vec2{ xvel,-2.f };
			curpar->textureid = FileSystem::load_sprite(spaceEffects_009, engptr).ID;
			curpar->uv = vec4{ 0.f,0.f,1.f,1.f };
			rX = rand() % 7;
			curpar->dim.x = rX;
			curpar->dim.y = rX;
			curpar->life = 0.5f;
			
			pl->lastSpawnSmoke = 0.f;
		}
	}

	return false;
}
bool update_meteor(Header* base, Meteor* pl, Entities *objects, Particle* partis)
{

	base->position.x += pl->velocity.x;
	base->position.y += pl->velocity.y;
	if(pl->hp == 0)
	{
		pl->hp = -1;
		pl->alpha = 0.99f;
	}
	if(pl->hp < 0)
	{
		
		if(pl->alpha == 0.99f)
		{
#ifndef P_ANDROID
			soundsystem::ss->playSound();
#endif
			if(pl->level == 0)
			{
				create_meteor(objects, vec2{ base->position.x + 20,base->position.y }, vec2{ 0.5f, pl->velocity.y *0.5f }, 1);
				create_meteor(objects, vec2{ base->position.x - 20,base->position.y }, vec2{ -0.5f, pl->velocity.y *0.5f }, 1);
			}


		}
		pl->alpha -= 0.05f;

		return pl->alpha < 0.f;
	}

		pl->lastspawnedRock += uprate;
		Particle* curpar = NULL;
		if (pl->lastspawnedRock > 0.015f)
		{

			for (int i = 0; i < 1000; i++)
			{
				if (partis[i].life <= 0)
				{
					curpar = &partis[i];
				}
			}
			int rX = rand() % 15;
			rX = rX < 5 ? 5 : rX;
			curpar->startlife = 0.8f;
			curpar->pos = base->position;
			curpar->pos.y += 30.f;
			curpar->pos.x -= base->dimensions.x;
			rX = rand() % (int)(base->dimensions.x*2);
			curpar->pos.x += rX;
			curpar->velocity = vec2{  0.0f,1.5f };
			curpar->textureid = base->textureID;
			curpar->uv = vec4{ 0.f,0.f,1.f,1.f };
			rX = rand() % 7;
			curpar->dim.x = rX;
			curpar->dim.y = rX;
			curpar->life = 0.5f;

			pl->lastspawnedRock = 0.f;

		}
	return false;
}
bool update_enemy(Header* base, Alien* pl)
{
	if(pl->hp <= 0)
	{
		if(pl->hp == 0)
		{
#ifndef P_ANDROID
			soundsystem::ss->playSound();
#endif
			pl->hp--;
		}
		pl->alpha -= 0.05f;

		return pl->alpha < 0.f;
	}
	base->position.x += pl->velocity.x;
	base->position.y += pl->velocity.y;

	return false;
}


bool update_projectile(Header* base, Projectile* pl,tree* data)
{
	bool ret = false;

	base->position.y += pl->velocity.y;
	return ret;
}
//todo take inputs in
bool update_starfield(Header* base, Starfield* pl,Entities *objects)
{
	base->position.x = 1000.f;
	base->position.y += pl->velocity.y;
	if(!pl->spawned && base->position.y < 1000)
	{
		add_starfield(objects, pl->velocity, vec2{ 1000,base->position.y + (base->dimensions.y * 2) },base->dimensions ,base->textureID);
		pl->spawned = true;
	}
	if(base->position.y + base->dimensions.y < 400)
	{
		return true;
	}
	return false;
}

void tester()
{
	Entitity ties[100];
	memset(ties, 0, sizeof *ties *100);
	//init_entities(&ties);
	for(int i = 0; i < 100 ; i ++)
	{
		ties[i].header.position = vec2{ (float)i,(float)i };
		ties[i].header.dimensions = vec2{ 1.f , 1.f };
		ties[i].header.id = i;
	}

	tree node;
	create_new_node(&node, 0, vec2{ 50 ,50 }, vec2{ 50 ,50 });

	for (int i = 10; i < 90; i++)
	{
		//insert_to_tree(&node, &ties[i]);
	}

	Entitity** buff;
	allocate_new_array<Entitity*>(&buff);
	for (int i = 40; i < 60; i++)
	{
		clear_array<Entitity*>(buff);
		//get_collisions(&node, &buff, &ties[i]);
		int s = get_array_size<Entitity*>(buff);
		printf("possible collisions %d\n", s);
	}
	clear_tree(&node);
}

#endif



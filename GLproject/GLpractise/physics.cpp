#include "../Header.h"
#include <algorithm>
#include <new>
namespace PakkiPhysics
{
	typedef struct
	{
		float	x;
		float	y;
	} vec2;

	enum class type
	{
		simulateObj,
		staticObj
	};
	struct simulated
	{
		vec2 velocity;
		vec2 acceleration;
		float mass;
	};

	struct Static
	{
		float friction;
	};

	struct object
	{
		type	t;
		vec2	pos;
		vec2	dim;
		union
		{
			simulated	m;
			Static		s;
		};
	};
	struct collisionTable
	{
		object* obj1;
		object* obj2;
	};
#define MAX_TREELEVEL 4
#define MAX_OBJECTAMOUNT 10
	struct tree
	{		
		unsigned int				level;
		dynamicArray<object*>		objects;
		tree*						treebuffer;
		vec2						pos;
		vec2						dim;
	};

	void create_new_node(tree* node, uint32_t level,const vec2& pos,const vec2& dim)
	{
		node->level = level;
		node->pos = pos;
		node->pos = pos;
		node->level = level;
		node->dim = dim;
		if(node->objects.data == NULL)
		{
			node->objects.init_array(20);
		}
		node->treebuffer == NULL;
	}
	void clear_tree(tree* node)
	{
		if(node->treebuffer)
		{
			for(uint32_t i = 0;i < 4;i++)
			{
				clear_tree(&node->treebuffer[i]);
			}
		}
		node->objects.clear_array();
		node->treebuffer = NULL;
	}
	void split_tree(tree* node,tree* allocator,uint32_t* allocatorstart)
	{
		float subWidht = node->dim.x / 2;
		float subHeight = node->dim.y / 2;

		node->treebuffer = &allocator[*allocatorstart];
		*allocatorstart += 4;

		create_new_node(&node->treebuffer[0], node->level + 1,
			vec2{ node->pos.x + subWidht,node->pos.y + subHeight },
			vec2{ subWidht,subHeight });
		create_new_node(&node->treebuffer[1], node->level + 1,
			vec2{ node->pos.x - subWidht,node->pos.y + subHeight },
			vec2{ subWidht,subHeight });
		create_new_node(&node->treebuffer[2], node->level + 1,
			vec2{ node->pos.x - subWidht,node->pos.y - subHeight },
			vec2{ subWidht,subHeight });
		create_new_node(&node->treebuffer[3], node->level + 1,
			vec2{ node->pos.x + subWidht,node->pos.y - subHeight },
			vec2{ subWidht,subHeight });
	}

	int get_index(tree* node, vec2 pos, vec2 dim)
	{
		int index = -1;

		//if true it can fit to bottom place
		bool bot = pos.y - dim.y  > node->pos.y - node->dim.y
			&& pos.y + dim.y <  node->pos.y;
		//if true it can fit to top place
		bool top = pos.y - dim.y  > node->pos.y
			&& pos.y + dim.y <  node->pos.y + node->dim.y;


		if (pos.x - dim.x  > node->pos.x - node->dim.x
			&& pos.x + dim.x <  node->pos.x)
		{
			if (top)
			{
				index = 1;
			}
			else if (bot)
			{
				index = 2;
			}
		}
		else if (pos.x - dim.x  > node->pos.x
			&& pos.x + dim.x <  node->pos.x + node->dim.x)
		{
			if (top)
			{
				index = 0;
			}
			else if (bot)
			{
				index = 3;
			}
		}
		return index;
	}

	void insert_to_tree(tree* node, object* obj,tree* allocator,uint32_t* allocatorsize)
	{
		if(node->treebuffer)
		{
			int index = get_index(node, obj->pos, obj->dim);
			if(index != -1)
			{
				insert_to_tree(&node->treebuffer[index], obj, allocator, allocatorsize);
				return;
			}
		}
		object** newObj = node->objects.get_new_item();
		(*newObj) = obj;
		
		if(node->objects.get_size() >= MAX_OBJECTAMOUNT && node->level < MAX_TREELEVEL)
		{
			if(node->treebuffer == NULL)
			{
				split_tree(node, allocator, allocatorsize);
			}
			int i = 0;
			while(i < node->objects.get_size())
			{
				int index = get_index(node, node->objects.data[i]->pos, node->objects.data[i]->dim);
				if(index != 1)
				{
					insert_to_tree(&node->treebuffer[index], obj, allocator, allocatorsize);
					node->objects.data[i] = NULL;
					node->objects.fast_remove(i);
				}
				else
				{
					i++;
				}
			}
		}

	}

	void get_collisions(tree* node,dynamicArray<object*>* buffer,object* obj)
	{
		int index = get_index(node, obj->pos, obj->dim);
		if(index != -1 && node->treebuffer != NULL)
		{
			get_collisions(node, buffer, obj);
		}
		for(uint32_t i = 0; i < node->objects.get_size();i++)
		{
			object** newobj = buffer->get_new_item();
			*newobj = node->objects.data[i];
		}
	}

	/***************************SCENE STUFF****************************/
	struct worldScene
	{
		worldScene(vec2 worldCentrePos,vec2 worldDimensions):objects(100),treeAllocator(nullptr),treeAllocatorSize(0)
		{
			constexpr uint32_t poolamount = 1 + 4 * 4 * 4;
			treeAllocator = (tree*)malloc(sizeof(tree) * poolamount);
			memset(treeAllocator, 0, sizeof(tree)*poolamount);
			objectAllocator.init_pool();
			objects.init_array(100);
			updateobjects.init_array(50);
			treeAllocatorSize = 1;
			this->worldCentrePos = worldCentrePos;
			create_new_node(treeAllocator, 0, worldCentrePos, worldDimensions);
			treeAllocatorSize++;
		}
		~worldScene()
		{
			constexpr uint32_t poolamount = 1 + 4 * 4 * 4;
			for(uint32_t i = 0; i < poolamount; i++)
			{
				if(treeAllocator[i].objects.data)
				{
					treeAllocator[i].objects.dispose_array();
				}
			}
			free(treeAllocator);
			treeAllocator = NULL;
			objectAllocator.dispose_pool();
			objects.dispose_array();
			updateobjects.dispose_array();
		}
		pool<object>			objectAllocator;
		dynamicArray<object*>	objects;
 		tree*					treeAllocator;
		uint32_t				treeAllocatorSize;
		dynamicArray<object*>	updateobjects;
		vec2					gravity;
		vec2					worldCentrePos;
		vec2					worldWidht;
	};
	bool AABB(const object* obj1, const object* obj2)
	{
		float disx = std::abs(obj1->pos.x - obj2->pos.x);
		float disY = std::abs(obj1->pos.y - obj2->pos.y);
		return disx < obj1->dim.x + obj2->dim.y && disY < obj1->dim.y + obj2->dim.y;
	}
	void update_objects(worldScene& scene,float dt)
	{

		for(uint32_t i = 0; i < scene.objects.get_size();i++)
		{
			object* currentobj = scene.objects.data[i];
			if(currentobj->t == type::simulateObj)
			{
				currentobj->pos = vec2{ (currentobj->m.velocity.x + currentobj->pos.x)* currentobj->m.mass , (currentobj->m.velocity.y + currentobj->pos.y)* currentobj->m.mass };
			}
		}
		scene.treeAllocatorSize = 1;
		for(int i = 0; i < scene.objects.get_size();i++)
		{
			insert_to_tree(scene.treeAllocator, scene.objects.data[i], scene.treeAllocator, &scene.treeAllocatorSize);
		}

		dynamicArray<collisionTable> collisiontable;
		dynamicArray<object*> collisionbuffer;
		for(uint32_t i = 0; i < scene.objects.get_size();i++)
		{
			object* currentobj = scene.objects.data[i];
			get_collisions(scene.treeAllocator,&collisionbuffer , currentobj);

			for (int i = 0; i < collisionbuffer.get_size();i++)
			{
				object* collider = collisionbuffer.data[i];
				if (currentobj == collider) continue;
				if (AABB(currentobj,collider))//collides
				{
					bool insert = true;
					for (uint32_t j = 0; j < collisiontable.get_size(); j++)
					{
						if (collisiontable.data[j].obj1 == currentobj && collider == collisiontable.data[j].obj2)
						{
							insert = false;
							break;
						}
					}
					if (insert)
					{
						collisionTable* t = collisiontable.get_new_item();
						t->obj1 = collider;
						t->obj2 = currentobj;
					}
				}
			}
			collisionbuffer.clear_array();
		}
		clear_tree(scene.treeAllocator);
		collisionbuffer.dispose_array();
		//**handle collisions**//
		for(uint32_t i = 0; i < scene.objects.get_size();i++)
		{

			object* currentobject = scene.objects.data[i];

		}
		collisiontable.dispose_array();
		//**handle genereal updates**//
	}
}
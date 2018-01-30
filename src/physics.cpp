#include <algorithm>
#include <new>
#include <physics.h>
#include <glm\glm\common.hpp>
#include <Texture.h>
#include <engine.h>
#include<InputManager.h>
#include <glfw3.h>

namespace PakkiPhysics
{


	enum class type
	{
		simulateObj,
		staticObj,
        hitBoxT
	};
	struct simulated
	{
		vec2 velocity;
        vec2 currentfriction;
		float mass;
        vec2 oldPos;
	};

	struct Static
	{
		float friction;
	};
    struct  hitBox
    {
        dynamicArray<object*> insideMe;
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
			hitBox		h;
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

    static uint32_t testID = 0;
    static object* player = NULL;
    void init_scene(worldScene* scene,vec2 worldCentrePos,vec2 worldDimensions, uint32_t texID)
    {
        constexpr uint32_t poolamount = 1 + 4 * 4 * 4;
        memset(scene, 0, sizeof *scene);
        scene->treeAllocator = (tree*)malloc(sizeof(tree) * poolamount);
        memset(scene->treeAllocator, 0, sizeof(tree)*poolamount);
        scene->objectAllocator.init_pool();
        scene->objects.init_array(100);
        scene->updateobjects.init_array(50);
        scene->treeAllocatorSize = 1;
        scene->worldCentrePos = worldCentrePos;
        create_new_node(scene->treeAllocator, 0, worldCentrePos, worldDimensions);
        testID = texID;

        object* Terrain = scene->objectAllocator.new_item();
        object* Box = scene->objectAllocator.new_item();

        Terrain->pos = vec2{ 100,100 };
        Terrain->dim = vec2{ 30,30 };
        Terrain->t = type::staticObj;
        Terrain->s.friction = 0.85f;


        Box->dim = vec2{ 5,5 };
        Box->pos = vec2{ 100,180 };
        Box->t = type::simulateObj;
        Box->m.mass = 1.0f;
        Box->m.velocity = vec2{ 0,0 };
        Box->m.currentfriction = vec2{ 1,1 };
        player = Box;

        object* Box2 = scene->objectAllocator.new_item();
        Box2->dim = vec2{ 5,5 };
        Box2->pos = vec2{ 100 ,180  + 20};
        Box2->t = type::simulateObj;
        Box2->m.mass = 1.0f;
        Box2->m.velocity = vec2{ 0 };
        Box2->m.currentfriction = vec2{ 1,1 };

        object* Box3 = scene->objectAllocator.new_item();
        Box3->dim = vec2{ 5,5 };
        Box3->pos = vec2{ 100 ,180 + 20 + 20 };
        Box3->t = type::simulateObj;
        Box3->m.mass = 1.0f;
        Box3->m.velocity = vec2{ 0 };
        Box3->m.currentfriction = vec2{ 1,1 };


        object* Terrain2 = scene->objectAllocator.new_item();
        Terrain2->pos = vec2{ 100 - 60,100 };
        Terrain2->dim = vec2{ 30,30 };
        Terrain2->t = type::staticObj;
        Terrain2->s.friction = 0.85f;


        object* Terrain3 = scene->objectAllocator.new_item();
        Terrain3->pos = vec2{ 100 + 60,100 };
        Terrain3->dim = vec2{ 30,30 };
        Terrain3->t = type::staticObj;
        Terrain3->s.friction = 0.85f;


        object* Terrain4 = scene->objectAllocator.new_item();
        Terrain4->pos = vec2{ 100 + 60 + 60,100 + 3 };
        Terrain4->dim = vec2{ 30,30 };
        Terrain4->t = type::staticObj;
        Terrain4->s.friction = 0.85f;

        object* Terrain5 = scene->objectAllocator.new_item();
        Terrain5->pos = vec2{ 100 - 60 - 60,100 + 3 };
        Terrain5->dim = vec2{ 30,30 };
        Terrain5->t = type::staticObj;
        Terrain5->s.friction = 0.85f;

		object* hitter = scene->objectAllocator.new_item();
		hitter->pos = vec2{ 100 - 60 - 60,100 + 3 + 60 };
		hitter->dim = vec2{ 30,30 };
		hitter->t = type::hitBoxT;
		hitter->h.insideMe.init_array();

        scene->objects.push_back(Terrain);
        scene->objects.push_back(Terrain2);
        scene->objects.push_back(Terrain3);
        scene->objects.push_back(Terrain4);
        scene->objects.push_back(Terrain5);
        scene->objects.push_back(Box);
        scene->objects.push_back(Box2);
        scene->objects.push_back(Box3);
		scene->objects.push_back(hitter);

        scene->gravity.y = -9.811f;
    }
    void dispose_scene(worldScene* scene)
    {
        constexpr uint32_t poolamount = 1 + 4 * 4 * 4;
        for (uint32_t i = 0; i < poolamount; i++)
        {
            if (scene->treeAllocator[i].objects.data)
            {
                scene->treeAllocator[i].objects.dispose_array();
            }
        }
        free(scene->treeAllocator);
        scene->treeAllocator = NULL;
        scene->objectAllocator.dispose_pool();
        scene->objects.dispose_array();
        scene->updateobjects.dispose_array();
    }
	bool AABB(const object* obj1, const object* obj2)
	{
		float disx = std::abs(obj1->pos.x - obj2->pos.x);
		float disY = std::abs(obj1->pos.y - obj2->pos.y);
		return disx < obj1->dim.x + obj2->dim.y && disY < obj1->dim.y + obj2->dim.y;
	}
	void update_objects(worldScene& scene,float dt, keys* k)
	{

		for(uint32_t i = 0; i < scene.objects.get_size();i++)
		{
			object* currentobj = scene.objects.data[i];
            if (currentobj == player)
            {
                if (is_key_pressed(GLFW_KEY_W))
                {
                    currentobj->m.velocity.y += 165.f ;
                }
                if (is_key_active(GLFW_KEY_A))
                {
                    currentobj->m.velocity.x -= 15.f;
                }
                if (is_key_active(GLFW_KEY_D))
                {
                    currentobj->m.velocity.x += 15.f ;
                }
            }
            if(currentobj->t == type::simulateObj)
            {
                currentobj->m.velocity.x += scene.gravity.x;
                currentobj->m.velocity.y += scene.gravity.y;
                if(currentobj->m.currentfriction.x * currentobj->m.currentfriction.y != 1)
                {
                    int a = 0;
                }
                currentobj->m.velocity.x *= currentobj->m.currentfriction.x;
                currentobj->m.velocity.y *= currentobj->m.currentfriction.y;
                currentobj->m.oldPos = currentobj->pos;
				currentobj->pos = vec2{ (currentobj->m.velocity.x *dt + currentobj->pos.x) ,
                    (currentobj->m.velocity.y *dt + currentobj->pos.y )};
                currentobj->m.currentfriction = vec2{ 1, 1 };
			}
			else if(currentobj->t == type::hitBoxT)
			{
				for(uint32_t r = 0; r < currentobj->h.insideMe.get_size(); r ++)
				{
					if (currentobj->h.insideMe.data[r]->t == type::simulateObj)
						currentobj->h.insideMe.data[r]->m.velocity.y += 15.f;
				}
				currentobj->h.insideMe.clear_array();
			}
		}
		scene.treeAllocatorSize = 1;
		for(int i = 0; i < scene.objects.get_size();i++)
		{
			insert_to_tree(scene.treeAllocator, scene.objects.data[i], scene.treeAllocator, &scene.treeAllocatorSize);
		}

		dynamicArray<collisionTable> collisiontable;
        dynamicArray<collisionTable> hardcollisiontable;

		dynamicArray<object*> collisionbuffer;
        collisionbuffer.init_array();
        hardcollisiontable.init_array();

        collisiontable.init_array();
		for(uint32_t i = 0; i < scene.objects.get_size();i++)
		{
			object* currentobj = scene.objects.data[i];
			get_collisions(scene.treeAllocator,&collisionbuffer , currentobj);

			for (int j = 0; j < collisionbuffer.get_size();j++)
			{
				object* collider = collisionbuffer.data[j];
				if (currentobj == collider) continue;
				if (AABB(currentobj,collider))//collides
				{
					bool insert = true;
                    dynamicArray<collisionTable>*  mytable;
                    if (collider->t == type::staticObj || currentobj->t == type::staticObj)
                    {
                        mytable = &hardcollisiontable;
                    }
					else if(collider->t == type::hitBoxT || currentobj->t == type::hitBoxT)
					{
						currentobj = currentobj->t == type::hitBoxT ? currentobj : collider;
						object** pt = currentobj->h.insideMe.get_new_item();
						*pt = collider;
						continue;
					}
                    else
                    {
                        mytable = &collisiontable;
                    }

					for (uint32_t jj = 0; jj < mytable->get_size(); jj++)
					{
						if (mytable->data[jj].obj1 == currentobj && collider == mytable->data[jj].obj2)
						{
							insert = false;
							break;
						}
					}
					if (insert)
					{
                            collisionTable* t = mytable->get_new_item();
                            t->obj1 = collider;
                            t->obj2 = currentobj;
					}
				}
			}
			collisionbuffer.clear_array();
		}
		clear_tree(scene.treeAllocator);
		collisionbuffer.dispose_array();
        //**new implementation**//
        for(uint32_t i = 0; i < collisiontable.get_size();i++)
        {
            object* currentobject = collisiontable.data[i].obj1;
            object* collider = collisiontable.data[i].obj2;         



            //**from which side they are colliding**//
            bool xside = abs(collider->m.oldPos.x - currentobject->m.oldPos.x) >= abs(collider->m.oldPos.y - currentobject->m.oldPos.y);

            if(xside)
            {
                //**calculate max distance for higher speed**//
                object* speedier = abs(collider->m.velocity.x) > abs(currentobject->m.velocity.x) ? collider : currentobject;
                object* slower = speedier == collider ? currentobject : collider;
                int dimS = (slower->pos.x - speedier->pos.x <= 0 ? 1 : -1);
                speedier->pos.x = (slower->dim.x + speedier->dim.x) * dimS  + slower->pos.x;
                float tempSpeed = speedier->m.velocity.x;
                speedier->m.velocity.x = slower->m.velocity.x / speedier->m.mass;
                slower->m.velocity.x = tempSpeed / slower->m.mass;        
            }
            else
            {
                //**calculate max distance for higher speed**//
                object* speedier = abs(collider->m.velocity.y) > abs(currentobject->m.velocity.y) ? collider : currentobject;
                object* slower = speedier == collider ? currentobject : collider;
                int dimS = (slower->pos.y - speedier->pos.y <= 0 ? 1 : -1);
                speedier->pos.y = (slower->dim.y + speedier->dim.y) * dimS + slower->pos.y;
                float tempSpeed = speedier->m.velocity.y;
                speedier->m.velocity.y = slower->m.velocity.y / speedier->m.mass;
                slower->m.velocity.y = tempSpeed / slower->m.mass;     
            }
        }
		
        for(uint32_t i = 0; i < hardcollisiontable.get_size();i++)
        {
            object* currentobject = hardcollisiontable.data[i].obj1;
            object* collider = hardcollisiontable.data[i].obj2;

            if (currentobject->t == type::staticObj && collider->t == type::simulateObj)
            {
                std::swap(currentobject, collider);
            }

            if(currentobject->t == type::simulateObj)
            {
                bool xside = abs(collider->pos.x - currentobject->pos.x) > abs(collider->pos.y - currentobject->pos.y);


                if(xside)
                {
                    int dimS = (collider->pos.x - currentobject->pos.x <= 0 ? 1 : -1);
                    currentobject->pos.x = collider->pos.x + ((collider->dim.x + currentobject->dim.x)*dimS);
                    if (dimS == 1)
                    {
                        currentobject->m.velocity.x = currentobject->m.velocity.x >= 0 ? currentobject->m.velocity.x : 0;
                    }
                    else
                    {
                        currentobject->m.velocity.x = currentobject->m.velocity.x <= 0 ? currentobject->m.velocity.x : 0;
                    }
                    vec2 newFriction = vec2{ 1,collider->s.friction };

                    float newLenght = newFriction.x*newFriction.x + newFriction.y*newFriction.y;//collider->s.friction;
                    float oldlenght = currentobject->m.currentfriction.x *currentobject->m.currentfriction.x + currentobject->m.currentfriction.y*currentobject->m.currentfriction.y;
                    currentobject->m.currentfriction = newLenght <= oldlenght ? newFriction : currentobject->m.currentfriction;
                }
                else
                {
                    int dimS = (collider->pos.y - currentobject->pos.y <= 0 ? 1 : -1);
                    currentobject->pos.y = collider->pos.y + ((collider->dim.y + currentobject->dim.y)*dimS);                   
                    if(dimS == 1)
                    {
                        currentobject->m.velocity.y = currentobject->m.velocity.y >= 0 ? currentobject->m.velocity.y : 0;
                    }
                    else
                    {
                        currentobject->m.velocity.y = currentobject->m.velocity.y <= 0 ? currentobject->m.velocity.y : 0;
                    }
                    vec2 newFriction = vec2{ collider->s.friction,1 };
                    float newLenght = newFriction.x*newFriction.x + newFriction.y*newFriction.y;//collider->s.friction;
                    float oldlenght = currentobject->m.currentfriction.x *currentobject->m.currentfriction.x + currentobject->m.currentfriction.y*currentobject->m.currentfriction.y;
                    currentobject->m.currentfriction = newLenght <= oldlenght ? newFriction : currentobject->m.currentfriction;
                }
            }
        }

		collisiontable.dispose_array();
        hardcollisiontable.dispose_array();
        object** ite = scene.objects.data;
        (*ite)->dim;
	}
    void draw_objects(worldScene* scene, SpriteBatch* batch)
    {
        for (object** iterator = scene->objects.data; iterator != scene->objects.get_back() + 1; iterator++)
        {
			if ((*iterator)->t == type::hitBoxT) continue;
            glm::vec4 destRec;
            destRec.x = (*iterator)->pos.x - (*iterator)->dim.x;
            destRec.y = (*iterator)->pos.y - (*iterator)->dim.y;
            destRec.z = (*iterator)->dim.x * 2;
            destRec.w = (*iterator)->dim.y * 2;
            glm::vec4 uv(0, 0, 1, 1);
            Color color{ (GLubyte)255, (GLubyte)255, (GLubyte)255,(GLubyte)255 };
            push_to_batch(batch, &destRec, &uv, testID, &color, 0, 0);
        }
    }
}
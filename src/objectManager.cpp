#include "objectManager.h"
#include <algorithm>
namespace ObjectManager
{

	//GUADTREE
#define USE_QUAD_TREE
#ifdef  USE_QUAD_TREE

#endif //  USE_QUAD_TREE
#define MAX_TREELEVEL 4
#define MAX_OBJECTAMOUNT 10
	
	void create_new_node(tree* node, uint32_t level, const vec2& pos, const vec2& dim)
	{
		node->level = level;
		node->pos = pos;
		node->level = level;
		node->dim = dim;
		if (node->objects.data == NULL)
		{
			node->objects.init_array(20);
		}
	}
	void clear_tree(tree* node)
	{
		if (node->treebuffer)
		{
			for (uint32_t i = 0; i < 4; i++)
			{
				clear_tree(&node->treebuffer[i]);
			}
		}
		node->objects.clear_array();
		node->treebuffer = NULL;
	}
	void split_tree(tree* node, tree* allocator, uint32_t* allocatorstart)
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

	void insert_to_tree(tree* node, object* obj, tree* allocator, uint32_t* allocatorsize)
	{
		if (node->treebuffer)
		{
			int index = get_index(node, obj->pos, obj->dim);
			if (index != -1)
			{
				insert_to_tree(&node->treebuffer[index], obj, allocator, allocatorsize);
				return;
			}
		}
		object** newObj = node->objects.get_new_item();
		(*newObj) = obj;

		if (node->objects.get_size() >= MAX_OBJECTAMOUNT && node->level < MAX_TREELEVEL)
		{
			if (node->treebuffer == NULL)
			{
				split_tree(node, allocator, allocatorsize);
			}
			uint32_t i = 0;
			while (i < node->objects.get_size())
			{
				int index = get_index(node, node->objects.data[i]->pos, node->objects.data[i]->dim);
				if (index != -1)
				{
					insert_to_tree(&node->treebuffer[index], node->objects.data[i], allocator, allocatorsize);
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

	void get_collisions(tree* node, dynamicArray<object*>* buffer, object* obj)
	{
		int index = get_index(node, obj->pos, obj->dim);
		if (index != -1 && node->treebuffer != NULL)
		{
			get_collisions(&node->treebuffer[index], buffer, obj);
		}
		for (uint32_t i = 0; i < node->objects.get_size(); i++)
		{
			object** newobj = buffer->get_new_item();
			*newobj = node->objects.data[i];
		}
	}




	/*GRID*/
#ifdef USE_GRIDS
#define GRIDSIZE	10
#define	GRIDAMOUNT		200
	bool AABB(float x,float y,float d1,float d2, const object* obj1)
	{
		float disx = std::abs(obj1->pos.x - x);
		float disY = std::abs(obj1->pos.y - y);
		return disx < obj1->dim.x + d1 && disY < obj1->dim.y + d2;
	}
	struct gridNode
	{
		dynamicArray<object*> my_objects;			
	};

	struct grid
	{
		gridNode grids[GRIDAMOUNT][GRIDAMOUNT];
		vec2 worldPos;
		vec2 dimensions;
	};
	void dispose_grid(grid* gr)
	{
		for(uint32_t x = 0; x < GRIDAMOUNT;x++)
		{
			for (uint32_t y = 0; y < GRIDAMOUNT; y++)
			{
				if(gr->grids[x][y].my_objects.data != NULL)
				{
					gr->grids[x][y].my_objects.dispose_array();
				}
			}
		}
	}
	void clear_grid(grid* gr)
	{
		for(uint32_t x = 0; x < GRIDAMOUNT; x++)
		{
			for (uint32_t y = 0; y < GRIDAMOUNT; y++)
			{
				if(gr->grids[x][y].my_objects.data != NULL)
				{
					gr->grids[x][y].my_objects.clear_array();
				}
			}
		}
	}
	void insert_to_grid(grid* gr,object* obj)
	{
		uint32_t myGridX =  (uint32_t)(obj->pos.x - (gr->worldPos.x - gr->dimensions.x));
		uint32_t myGridY = (uint32_t)(obj->pos.y - (gr->worldPos.y - gr->dimensions.y));
		if (myGridX >= GRIDAMOUNT || myGridY >= GRIDAMOUNT) return; // insertion failed

		if(gr->grids[myGridX][myGridY].my_objects.data == NULL)
		{
			gr->grids[myGridX][myGridY].my_objects.init_array(20);
		}
		object** e = gr->grids[myGridX][myGridY].my_objects.get_new_item();
		*e = obj;
	}
	void get_area_from_grid(grid* gr,dynamicArray<id> *buffer,const float x,const float y,const float d1,const float d2)
	{

		float startX = x - d1;
		float startY = y - d2;
		while(startX < x + d1)
		{
			while (startY < y + d2)
			{
				int indX = (startX - gr->worldPos.x - gr->dimensions.x)/GRIDSIZE;
				int indY = (startY - gr->worldPos.y - gr->dimensions.y)/GRIDSIZE;
				startY += d2;
				if (indX < 0 || indY < 0 || indX >= GRIDAMOUNT || indY >= GRIDAMOUNT) continue;

				for (uint32_t i = 0; i < gr->grids[indX][indY].my_objects._size; i++)
				{
					if(AABB(x,y,d1,d2,gr->grids[indX][indY].my_objects.data[i]))
					{
						id* _id = buffer->get_new_item();
						*_id = gr->grids[indX][indY].my_objects.data[i]->i;
					}
				}
			}
			startY = y - d2;
			startX += GRIDSIZE;
		}

	}
	void insert_to_multiple_grids(grid* gr, object* obj)
	{
		float startX = obj->pos.x - obj->dim.x;
		float startY = obj->pos.y - obj->dim.y;
		while (startX < obj->pos.x + obj->dim.x)
		{
			while (startY < obj->pos.y + obj->dim.y)
			{
				int indX = (startX - gr->worldPos.x - gr->dimensions.x) / GRIDSIZE;
				int indY = (startY - gr->worldPos.y - gr->dimensions.y) / GRIDSIZE;
				startY += obj->dim.y;
				if (indX < 0 || indY < 0 || indX >= GRIDAMOUNT || indY >= GRIDAMOUNT) continue;

				for (uint32_t i = 0; i < gr->grids[indX][indY].my_objects._size; i++)
				{
					if (AABB(obj->pos.x, obj->pos.y, obj->dim.x, obj->dim.y, gr->grids[indX][indY].my_objects.data[i]))
					{
						
						object** o =  gr->grids[indX][indY].my_objects.get_new_item();
						*o = obj;					
					}
				}
			}
			startY = obj->pos.y - obj->dim.y;
			startX += GRIDSIZE;
		}
	}

	void poll_multiple_grids(grid* gr, dynamicArray<id> *buffer, const float x, const float y, const float d1, const float d2)
	{
		float startX = x - d1;
		float startY = y - d2;
		while (startX < x + d1)
		{
			while (startY < y + d2)
			{
				int indX = (startX - gr->worldPos.x - gr->dimensions.x) / GRIDSIZE;
				int indY = (startY - gr->worldPos.y - gr->dimensions.y) / GRIDSIZE;
				startY += d2;
				if (indX < 0 || indY < 0 || indX >= GRIDAMOUNT || indY >= GRIDAMOUNT) continue;

				for (uint32_t i = 0; i < gr->grids[indX][indY].my_objects._size; i++)
				{
					if (AABB(x, y, d1, d2, gr->grids[indX][indY].my_objects.data[i]))
					{
						bool insert = true;
						for(uint32_t buffInd= 0; buffInd < buffer->_size; buffInd++)
						{
							if (buffer->data[buffInd] == gr->grids[indX][indY].my_objects.data[i]->i)
							{
								insert = false;
								break;
							}
						}
						if(insert)
						{
							id* _id = buffer->get_new_item();
							*_id = gr->grids[indX][indY].my_objects.data[i]->i;
						}
					}
				}
			}
			startY = y - d2;
			startX += GRIDSIZE;
		}
	}
#endif //USE_GRID
	/* OBJECTS */

	enum objstate : state
	{
		Static = (1<<1),
		Moving = (1<<2)
	};

	

/*
	inline void get_id(id id,uint16_t* arr, uint16_t* ind)
	{
		arr = (uint16_t*)&id;
		ind = ((uint16_t*)&id) + 1;
	}
	inline void set_id(id* _id, uint16_t arr, uint16_t ind)
	{
		id newid = 0;
		uint16_t* arptr = (uint16_t*)(&newid);
		*arptr = arr;
		arptr++;
		*arptr = ind;
		arptr--;
		*_id = *((id*)arptr);
	}*/

	void init_objects(objects* objs,vec2 worldMid,vec2 dimensions)
	{
		memset(objs, 0, sizeof(objects));
		constexpr uint32_t poolamount = 1 + 4 * 4 * 4;
		objs->treeAllocator = (tree*)malloc(sizeof(tree) * poolamount);
		objs->allocatorSize = 1;
		objs->worldMid = worldMid;
		objs->worldDims = dimensions;
		objs->objectPool.init_pool();
		objs->drawPool.init_pool();
		create_new_node(&objs->objTree, 0, worldMid, dimensions);
		objs->hardCollisionBuffer.init_array(50);
		objs->softCollisionBuffer.init_array(50);
		objs->updateBuffer.init_array(POOLSIZE);
		objs->generalcollisionBuffer.init_array();
		objs->drawAbleOnes.init_array(POOLSIZE);
	}
	bool AABB(const object* obj1, const object* obj2)
	{
		float disx = std::abs(obj1->pos.x - obj2->pos.x);
		float disY = std::abs(obj1->pos.y - obj2->pos.y);
		return disx < obj1->dim.x + obj2->dim.y && disY < obj1->dim.y + obj2->dim.y;
	}
	void get_collisions_to_buffer(objects* objs)
	{
		clear_tree(&objs->objTree);
		objs->updateBuffer.clear_array();
		objs->drawAbleOnes.clear_array();
		for(uint32_t i = 0; i < objs->objectPool._data._size; i++)
		{
			for(uint32_t j = 0; j < POOLSIZE;j++)
			{
				if ((objs->objectPool._data.data[i])[j].s == 0) continue;
				if((objs->objectPool._data.data[i])[j].drawPtr != NULL)
				{
					object** drawthis = objs->drawAbleOnes.get_new_item();
					*drawthis = &(objs->objectPool._data.data[i])[j];
				}
				insert_to_tree(&objs->objTree, &(objs->objectPool._data.data[i])[j], objs->treeAllocator, &objs->allocatorSize);
				//if ((objs->objectPool._data.data[i])[j].s == objstate::Static) continue; this would be optimal but then we cannot be dure that everything collides
				object** ptr = objs->updateBuffer.get_new_item();
				*ptr = &(objs->objectPool._data.data[i])[j];
			}
		}
		uint32_t size = objs->updateBuffer.get_size();
		for(uint32_t i = 0; i < size;i++)
		{
			objs->generalcollisionBuffer.clear_array();
			object* currenObject = objs->updateBuffer.data[i];
			get_collisions(objs->treeAllocator, &objs->generalcollisionBuffer, objs->updateBuffer.data[i]);
			for(uint32_t collInd = 0; collInd < objs->generalcollisionBuffer._size; collInd++)
			{
				object* collider = objs->generalcollisionBuffer.data[collInd];
				if (collider == currenObject)continue;
				if (currenObject->s == objstate::Static && collider->s == objstate::Static)continue;
				if (!AABB(currenObject, collider))continue;
				if(currenObject->s == objstate::Static || collider->s == objstate::Static)
				{
					if(currenObject->s != objstate::Moving)//swap
					{
						object* temp = currenObject;
						currenObject = collider;
						collider = currenObject;
					}
					bool insert = true;
					for(uint32_t copyCheck = 0; copyCheck < objs->hardCollisionBuffer._size;copyCheck++)
					{
						if(currenObject == objs->hardCollisionBuffer.data[copyCheck].obj1 && collider == objs->hardCollisionBuffer.data[copyCheck].obj2)
						{
							insert = false;
							break;
						}
					}
					if(insert)
					{
						collisiontable* t = objs->hardCollisionBuffer.get_new_item();
						t->obj1 = currenObject;
						t->obj2 = collider;
					}

				}
				else if(currenObject->s == objstate::Moving && collider->s == objstate::Moving)
				{
					bool insert = true;
					for(uint32_t copyCheck = 0;copyCheck < objs->softCollisionBuffer._size;copyCheck++)
					{
						if (currenObject == objs->softCollisionBuffer.data[copyCheck].obj1 && collider == objs->softCollisionBuffer.data[copyCheck].obj2)
						{
							insert = false;
							break;
						}
					}
					if (insert)
					{
						collisiontable* t = objs->softCollisionBuffer.get_new_item();
						t->obj1 = currenObject;
						t->obj2 = collider;
					}
				}
				else
				{
					assert(true);
				}
			}
		}

	}
	void dispose_objects(objects* objs)
	{
		constexpr uint32_t poolamount = 1 + 4 * 4 * 4;
		for (uint32_t i = 0; i < poolamount; i++)
		{
			if (objs->treeAllocator[i].objects.data)
			{
				objs->treeAllocator[i].objects.dispose_array();
			}
		}
		free(objs->treeAllocator);
		objs->objectPool.dispose_pool();
		objs->drawPool.dispose_pool();
		objs->hardCollisionBuffer.dispose_array();
		objs->softCollisionBuffer.dispose_array();
		objs->updateBuffer.dispose_array();
		objs->generalcollisionBuffer.dispose_array();
		objs->drawAbleOnes.dispose_array();
	}

	void draw_objects(dynamicArray<object*>* drawObjs,SpriteBatch* batch)//korjaa glm pois
	{
		for(uint32_t i = 0; i < drawObjs->_size;i++)
		{
			assert(drawObjs->data[i]->drawPtr != NULL);
			glm::vec4 destRec;
			glm::vec2 pos((int)drawObjs->data[i]->pos.x, (int)drawObjs->data[i]->pos.y);
			destRec.x = (pos.x - (int)drawObjs->data[i]->dim.x);
			destRec.y = pos.y - (int)drawObjs->data[i]->dim.y;
			destRec.z = (int)drawObjs->data[i]->dim.x * 2;
			destRec.w = (int)drawObjs->data[i]->dim.y * 2;
			glm::vec4 uv(drawObjs->data[i]->drawPtr->uv.x, drawObjs->data[i]->drawPtr->uv.y, drawObjs->data[i]->drawPtr->uv.z, drawObjs->data[i]->drawPtr->uv.w);
			push_to_batch(batch, &destRec, &uv, drawObjs->data[i]->drawPtr->spriteid, &drawObjs->data[i]->drawPtr->color, drawObjs->data[i]->drawPtr->level, 0);
		}
	}

}

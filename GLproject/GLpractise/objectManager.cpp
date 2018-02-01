#include <arrayD.h>
#include <algorithm>

namespace ObjectManager
{
	typedef uint32_t state;
	typedef uint32_t id;
	struct vec2
	{
		float	x;
		float	y;
	};

	struct object
	{
		vec2	pos;
		vec2	dim;
		state	s;
		id		i;
	};
	/*GRID*/

#define GRIDSIZE	10
#define	GRIDAMOUNT		200
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
	bool AABB(float x,float y,float d1,float d2, const object* obj1)
	{
		float disx = std::abs(obj1->pos.x - x);
		float disY = std::abs(obj1->pos.y - y);
		return disx < obj1->dim.x + d1 && disY < obj1->dim.y + d2;
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
	/* OBJECTS */

	enum objstate : state
	{
		Static = (1<<1),
		Moving = (1<<2)
	};

	


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
	}
#define POOLSIZE 255

	struct objects
	{
		pool<object, POOLSIZE>	objectPool;
		grid					movableObjects;
		grid					staticObjects;
		vec2					worldMid;
		vec2					worldDims;
	};
	void init_objects(objects* objs,vec2 worldMid,vec2 dimensions)
	{
		memset(objs, 0, sizeof(objects));
		objs->worldMid = worldMid;
		objs->worldDims = dimensions;
		objs->objectPool.init_pool();
		objs->staticObjects.dimensions = dimensions;
		objs->movableObjects.dimensions = dimensions;
		objs->staticObjects.worldPos = worldMid;
		objs->movableObjects.worldPos = worldMid;
	}
	void update_objects(objects* objs)
	{
		clear_grid(&objs->movableObjects);
		for(uint32_t i = 0; i < objs->objectPool._data._size; i++)
		{
			for(uint32_t j = 0; j < POOLSIZE;j++)
			{
				if((objs->objectPool._data.data[i])[j].i & objstate::Moving)
				{
					insert_to_grid(&objs->movableObjects, &(objs->objectPool._data.data[i])[j]);
				}
			}
		}
	}
	void dispose_objects(objects* objs)
	{
		dispose_grid(&objs->movableObjects);
		dispose_grid(&objs->staticObjects);
		objs->objectPool.dispose_pool();
	}
}
#include <arrayD.h>


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
	void get_area_from_grid(grid* gr,dynamicArray<object*> *buffer,float x,float y, float d1, float d2)
	{
		
	}

	/* OBJECTS */

	enum objstate : state
	{
		Static = (1<<1),
		Moving = (1<<2),
		Dead = (1<<3),
		Alive = (1<<4)
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
#define POOLSIZE 100

	struct objects
	{
		pool<object, POOLSIZE> objectPool;
	};


}
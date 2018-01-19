#include "../Header.h"
#include <algorithm>

namespace PakkiPhysics
{
	enum class type
	{
		simulateObj,
		staticObj
	};
	struct simulated
	{
		float velX;
		float velY;
	};

	struct Static
	{
		float friction;
	};

	struct object
	{
		type	t;
		float	x;
		float	y;
		float	widht;
		float	height;
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
	struct worldScene
	{
		pool<object>			objectAllocator;
		dynamicArray<object*>	objects;
		dynamicArray<object*>	updateobjects;

	};
	bool AABB(const object& obj1, const object& obj2)
	{
		float disx = abs(obj1.x - obj2.x);
		float disY = abs(obj1.y - obj2.y);
		return disx < obj1.widht + obj2.widht && disY < obj1.height + obj2.height;
	}
	void update_objects(worldScene& scene,float dt)
	{



		for(uint32_t i = 0; i < scene.updateobjects.get_size();i++)
		{
				
		}
	}
}
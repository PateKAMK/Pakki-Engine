#include <arrayD.h>
#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>
#include <perlinnoice.h>
#include <InstancedRenderer.h>
static int c = 0;

struct v2
{
	float x;
	float y;
};

#define DIMENSION 100

struct terrain
{
	v2 pos;
	dynamicArray<v2> destroyedTerrain;
	int xP;
	int yP;
};

struct terrainHandler
{
	terrain* currentTerrainsHandles[3 * 3 * 3];
	uint8_t* meshes[3 * 3 * 3];
	float scale;
	dynamicArray<terrain*> allTerrains;
	pool<terrain, 200>	terrainAllocator;
	bool update;
};
void createNewTerrain(pool<terrain,200>* allocator,terrain* create,int x,int y,v2 pos)
{
	create = allocator->new_item();
	create->destroyedTerrain.init_array();
	create->pos = pos;
	create->xP = x;
	create->yP = y;
}
void createMesh(uint8_t mesh)
{
	
}
void updateTerrain(terrainHandler* ter,InRender::instaRenderer* rend,v2 worldPos)
{
	float xp = ter->currentTerrainsHandles[1 * 3 + 1]->pos.x - worldPos.x;
	float yp = ter->currentTerrainsHandles[1 * 3 + 1]->pos.y - worldPos.y;

	if(abs(xp) > (DIMENSION * ter->scale))
	{
		terrain* t = NULL;
		terrain* m = NULL;
		terrain* b = NULL;

		if(xp < 0)
		{
			terrain* t = ter->currentTerrainsHandles[3 * 2];
			terrain* m = ter->currentTerrainsHandles[3 * 1];
			terrain* b = ter->currentTerrainsHandles[3 * 0];

			terrain* tN = NULL;
			terrain* mN = NULL;
			terrain* bN = NULL;

			for(int i = 0; i < ter->allTerrains.get_size();i++ )
			{
				if (ter->allTerrains.data[i]->xP == t->xP && ter->allTerrains.data[i]->yP == t->yP)
				{
					tN = ter->allTerrains.data[i];
				}
				else if (ter->allTerrains.data[i]->xP == m->xP && ter->allTerrains.data[i]->yP == m->yP)
				{
					mN = ter->allTerrains.data[i];
				}
				else if (ter->allTerrains.data[i]->xP == b->xP && ter->allTerrains.data[i]->yP == b->yP)
				{
					bN = ter->allTerrains.data[i];
				}
			}
			if(tN == NULL)
			{
			
			}
			if(mN == NULL)
			{
			
			}
			if(bN == NULL)
			{
				
			}

		}
		else
		{
			
		}
	}
	if(abs(yp) > (DIMENSION * ter->scale))
	{
		if (yp < 0)
		{

		}
		else
		{

		}
	}


	if(ter->update)
	{
		for (int h = 0; h < 3; h++)
		{
			for (int w = 0; w < 3; w++)
			{
				terrain* current = ter->currentTerrainsHandles[h * 3 + w];
				uint8_t* mesh = ter->meshes[h * 3 + w];
				for (int h = 0; h < DIMENSION; h++)
				{
					for (int w = 0; w < DIMENSION; w++)
					{
						if(mesh[h*DIMENSION + w] != 0)
						{
							InRender::push_to_renderer(rend,((current->pos.x - (DIMENSION * 0.5 * ter->scale)) + w * ter->scale) ,
								((current->pos.y - (DIMENSION * 0.5 * ter->scale)) + h * ter->scale), ter->scale, 0, 0, 1, 1);
						}
					}
				}


			}
		}
	}
	ter->update = false;

}
void generate_mesh(tileId** mesh)
{
	int widht = 100;
	int height = 100;
	*mesh = (tileId*)calloc(widht*height,sizeof(tileId));
	float offset = 0;
	for(int h = 0; h < height; h++)
	{
		for (int w = 0; w < widht; w++)
		{
			float val = stb_perlin_noise3((w + c)/ 30.f, ( h )/30.f, 10, 256, 256, 256);
			//float val = stb_perlin_ridge_noise3(w, h, w * h,  2,   0.5   , offset,3  ,0  ,  0  , 0);
			tileId n = (tileId)(val < -0.1 ? 0 : 1);
			(*mesh)[(h  *  widht) + w] = n;
				// float stb_perlin_noise3(float x, float y, float z, int x_wrap, int y_wrap, int z_wrap);


				// float stb_perlin_ridge_noise3(float x, float y, float z,
				//                               float lacunarity, float gain, float offset, int octaves,
				//                               int x_wrap, int y_wrap, int z_wrap)
			offset += 0.007;
		}
	}
			 c++;
}


void generate_noice_map()
{

}
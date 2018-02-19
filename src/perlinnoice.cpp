#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>
#include <perlinnoice.h>

static int c = 0;

void createMesh(uint8_t* mesh, float x, float y,float scale)
{
	constexpr float dim = DIMENSION / 2.f;
	printf("GENERATING --%f -- %f\n", x, y);
//	float inc = scale / (float)DIMENSION;
	float height = y - dim;
	for (int h = 0; h < DIMENSION; h++)
	{
		float widht = x - dim;
		for (int w = 0; w < DIMENSION; w++)
		{
			float val = stb_perlin_noise3(widht / 50.f, height / 50.f, 1, 256, 256, 256);
			tileId n = (tileId)(val < -0.2? 0 : 1);
			mesh[(h  *  DIMENSION) + w] = n;
			widht += scale;
		}
		height += scale;
	}
}

void init_terrains(terrainHandler* ter, float worldX,float worldY)
{
	ter->allTerrains.init_array(100);
	ter->meshAllocator = (uint8_t*)malloc(sizeof(uint8_t) * DIMENSION * DIMENSION * 9);
	ter->terrainAllocator.init_pool();
	ter->update = false;
	ter->scale = 2;
	for(int y = 0; y < 9; y++)
	{
		ter->meshes[y] = &ter->meshAllocator[DIMENSION * DIMENSION * y];
		terrain* nter = ter->terrainAllocator.new_item();
		ter->currentTerrainsHandles[y] = nter;
		nter->destroyedTerrain.init_array();
		ter->allTerrains.push_back(nter);
	}
	ter->currentTerrainsHandles[0]->xP = -1;
	ter->currentTerrainsHandles[0]->yP = -1;

	ter->currentTerrainsHandles[1]->xP =  0;
	ter->currentTerrainsHandles[1]->yP = -1;

	ter->currentTerrainsHandles[2]->xP =  1;
	ter->currentTerrainsHandles[2]->yP = -1;

	ter->currentTerrainsHandles[3]->xP = -1;
	ter->currentTerrainsHandles[3]->yP =  0;

	ter->currentTerrainsHandles[4]->xP = 0;
	ter->currentTerrainsHandles[4]->yP = 0;

	ter->currentTerrainsHandles[5]->xP = 1;
	ter->currentTerrainsHandles[5]->yP = 0;

	ter->currentTerrainsHandles[6]->xP = -1;
	ter->currentTerrainsHandles[6]->yP =  1;

	ter->currentTerrainsHandles[7]->xP = 0;
	ter->currentTerrainsHandles[7]->yP = 1;

	ter->currentTerrainsHandles[8]->xP = 1;
	ter->currentTerrainsHandles[8]->yP = 1;

	for(int i = 0; i < 9; i++)
	{
		ter->currentTerrainsHandles[i]->pos.x = worldX + DIMENSION * ter->currentTerrainsHandles[i]->xP * ter->scale;
		ter->currentTerrainsHandles[i]->pos.y = worldX + DIMENSION * ter->currentTerrainsHandles[i]->yP * ter->scale;
		createMesh(ter->meshes[i], ter->currentTerrainsHandles[i]->pos.x, ter->currentTerrainsHandles[i]->pos.y,ter->scale);
		//ter->meshes[i]
	}
}
void dispose_terrain(terrainHandler* ter)
{
	for(int i = 0; i < ter->allTerrains._size; i++)
	{
		ter->allTerrains.data[i]->destroyedTerrain.dispose_array();
	}
	ter->allTerrains.dispose_array();
	free(ter->meshAllocator);
	ter->terrainAllocator.dispose_pool();
}
terrain* createNewTerrain(pool<terrain,200>* allocator,dynamicArray<terrain*>* allterrains,int x,int y,v2 pos)
{
	terrain* create = NULL;
	create = allocator->new_item();
	create->destroyedTerrain.init_array();
	create->pos = pos;
	create->xP = x;
	create->yP = y;
	allterrains->push_back(create);
	return create;
}

void updateTerrain(terrainHandler* ter,InRender::instaRenderer* rend,v2 worldPos, DebugRenderer* deb)
{
	float xp = ter->currentTerrainsHandles[1 * 3 + 1]->pos.x - worldPos.x;
	float yp = ter->currentTerrainsHandles[1 * 3 + 1]->pos.y - worldPos.y;

	if(abs(xp) > (DIMENSION * ter->scale))
	{


		terrain* tN = NULL;
		terrain* mN = NULL;
		terrain* bN = NULL;
		if(xp > 0)
		{


			int tX = ter->currentTerrainsHandles[3 * 2]->xP - 1;
			int tY = ter->currentTerrainsHandles[3 * 2]->yP;
			int mX = ter->currentTerrainsHandles[3 * 1]->xP - 1;
			int mY = ter->currentTerrainsHandles[3 * 1]->yP;
			int bX = ter->currentTerrainsHandles[3 * 0]->xP - 1;
			int bY = ter->currentTerrainsHandles[3 * 0]->yP;


			for(int i = 0; i < ter->allTerrains.get_size();i++ ) // not visited
			{
				if (ter->allTerrains.data[i]->xP == tX && ter->allTerrains.data[i]->yP == tY)
				{
					tN = ter->allTerrains.data[i];
				}
				else if (ter->allTerrains.data[i]->xP == mX && ter->allTerrains.data[i]->yP == mY)
				{
					mN = ter->allTerrains.data[i];
				}
				else if (ter->allTerrains.data[i]->xP == bX && ter->allTerrains.data[i]->yP == bY)
				{
					bN = ter->allTerrains.data[i];
				}
			}
			if(tN == NULL)
			{
				terrain* m = ter->currentTerrainsHandles[3 * 2];
				//uint8_t* tMesh = (uint8_t*)malloc(sizeof(uint8_t)*DIMENSION*DIMENSION);
				//tN = (terrain*)malloc(sizeof(terrain));
				v2 newPos{ m->pos.x - (DIMENSION * ter->scale),m->pos.y /*+ (DIMENSION * ter->scale)*/ };
				tN = createNewTerrain(&ter->terrainAllocator,&ter->allTerrains, m->xP - 1, m->yP /*+ 1*/, newPos);
				//createMesh(tMesh, m->xP - 1, m->yP + 1);
				//ter->meshes
			}
			if(mN == NULL)
			{
				terrain* m = ter->currentTerrainsHandles[3 * 1];
				//uint8_t* tMesh = (uint8_t*)malloc(sizeof(uint8_t)*DIMENSION*DIMENSION);
				//tN = (terrain*)malloc(sizeof(terrain));
				v2 newPos{ m->pos.x - (DIMENSION * ter->scale),m->pos.y };
				mN = createNewTerrain(&ter->terrainAllocator,&ter->allTerrains, m->xP - 1, m->yP, newPos);
				//createMesh(tMesh, m->xP - 1, m->yP);
			}
			if(bN == NULL)
			{
				terrain* m = ter->currentTerrainsHandles[3 * 0];
				//uint8_t* tMesh = (uint8_t*)malloc(sizeof(uint8_t)*DIMENSION*DIMENSION);
				//tN = (terrain*)malloc(sizeof(terrain));
				v2 newPos{ m->pos.x - (DIMENSION * ter->scale),m->pos.y /*- (DIMENSION * ter->scale) */};
				bN = createNewTerrain(&ter->terrainAllocator,&ter->allTerrains ,m->xP - 1, m->yP/* - 1*/, newPos);
				//createMesh(tMesh, m->xP - 1, m->yP - 1);
			}
			// change old out and new in
			ter->currentTerrainsHandles[3 * 2 + 2] = ter->currentTerrainsHandles[3 * 2 + 1];
			ter->currentTerrainsHandles[3 * 1 + 2] = ter->currentTerrainsHandles[3 * 1 + 1];
			ter->currentTerrainsHandles[3 * 0 + 2] = ter->currentTerrainsHandles[3 * 0 + 1];

			ter->meshes[3 * 2 + 2] = ter->meshes[3 * 2 + 1];
			ter->meshes[3 * 1 + 2] = ter->meshes[3 * 1 + 1];
			ter->meshes[3 * 0 + 2] = ter->meshes[3 * 0 + 1];

			ter->currentTerrainsHandles[3 * 2 + 1] = ter->currentTerrainsHandles[3 * 2 ];
			ter->currentTerrainsHandles[3 * 1 + 1] = ter->currentTerrainsHandles[3 * 1 ];
			ter->currentTerrainsHandles[3 * 0 + 1] = ter->currentTerrainsHandles[3 * 0 ];

			ter->meshes[3 * 2 + 1] = ter->meshes[3 * 2 ];
			ter->meshes[3 * 1 + 1] = ter->meshes[3 * 1 ];
			ter->meshes[3 * 0 + 1] = ter->meshes[3 * 0 ];


			ter->currentTerrainsHandles[3 * 2 ] = tN;
			ter->currentTerrainsHandles[3 * 1 ] = mN;
			ter->currentTerrainsHandles[3 * 0 ] = bN;

			//createMesh(ter->meshes[3 * 2], tN->pos.x, tN->pos.y,ter->scale);
			//createMesh(ter->meshes[3 * 1], mN->pos.x, mN->pos.y, ter->scale);
			//createMesh(ter->meshes[3 * 0], bN->pos.x, bN->pos.y, ter->scale);
			memset(ter->meshes[3 * 2], 0, sizeof(uint8_t)* DIMENSION * DIMENSION);
			memset(ter->meshes[3 * 1], 0, sizeof(uint8_t)* DIMENSION * DIMENSION);
			memset(ter->meshes[3 * 0], 0, sizeof(uint8_t)* DIMENSION * DIMENSION);


		}
		else // move right
		{


			 int tX = ter->currentTerrainsHandles[3 * 2 + 2]->xP + 1;
			 int tY = ter->currentTerrainsHandles[3 * 2 + 2]->yP;
			 int mX = ter->currentTerrainsHandles[3 * 1 + 2]->xP + 1;
			 int mY = ter->currentTerrainsHandles[3 * 1 + 2]->yP;
			 int bX = ter->currentTerrainsHandles[3 * 0 + 2]->xP + 1;
			 int bY = ter->currentTerrainsHandles[3 * 0 + 2]->yP ;



			for (int i = 0; i < ter->allTerrains.get_size(); i++) // not visited
			{
				if (ter->allTerrains.data[i]->xP == tX && ter->allTerrains.data[i]->yP == tY)
				{
					tN = ter->allTerrains.data[i];
				}
				else if (ter->allTerrains.data[i]->xP == mX && ter->allTerrains.data[i]->yP == mY)
				{
					mN = ter->allTerrains.data[i];
				}
				else if (ter->allTerrains.data[i]->xP == bX && ter->allTerrains.data[i]->yP == bY)
				{
					bN = ter->allTerrains.data[i];
				}
			}
			if (tN == NULL)
			{
				terrain* m = ter->currentTerrainsHandles[3 * 2 + 2];
				//uint8_t* tMesh = (uint8_t*)malloc(sizeof(uint8_t)*DIMENSION*DIMENSION);
				//tN = (terrain*)malloc(sizeof(terrain));
				v2 newPos{ m->pos.x + (DIMENSION * ter->scale),m->pos.y/* + (DIMENSION * ter->scale) */};
				tN = createNewTerrain(&ter->terrainAllocator, &ter->allTerrains, m->xP + 1, m->yP + 1, newPos);
				//createMesh(tMesh, m->xP + 1, m->yP + 1);
			}
			if (mN == NULL)
			{
				terrain* m = ter->currentTerrainsHandles[3 * 1 + 2];
				//uint8_t* tMesh = (uint8_t*)malloc(sizeof(uint8_t)*DIMENSION*DIMENSION);
				//tN = (terrain*)malloc(sizeof(terrain));
				v2 newPos{ m->pos.x + (DIMENSION * ter->scale),m->pos.y };
				mN = createNewTerrain(&ter->terrainAllocator, &ter->allTerrains, m->xP + 1, m->yP, newPos);
				//createMesh(tMesh, m->xP + 1, m->yP);
			}
			if (bN == NULL)
			{
				terrain* m = ter->currentTerrainsHandles[3 * 0 + 2];
				//uint8_t* tMesh = (uint8_t*)malloc(sizeof(uint8_t)*DIMENSION*DIMENSION);
				//tN = (terrain*)malloc(sizeof(terrain));
				v2 newPos{ m->pos.x + (DIMENSION * ter->scale),m->pos.y /*- (DIMENSION * ter->scale) */};
				bN = createNewTerrain(&ter->terrainAllocator, &ter->allTerrains, m->xP + 1, m->yP /*- 1*/, newPos);
				//createMesh(tMesh, m->xP + 1, m->yP - 1);
			}
			// change old out and new in
			ter->currentTerrainsHandles[3 * 2 ] = ter->currentTerrainsHandles[3 * 2 + 1];
			ter->currentTerrainsHandles[3 * 1 ] = ter->currentTerrainsHandles[3 * 1 + 1];
			ter->currentTerrainsHandles[3 * 0 ] = ter->currentTerrainsHandles[3 * 0 + 1];

			ter->meshes[3 * 2 ] = ter->meshes[3 * 2 + 1];
			ter->meshes[3 * 1 ] = ter->meshes[3 * 1 + 1];
			ter->meshes[3 * 0 ] = ter->meshes[3 * 0 + 1];

			ter->currentTerrainsHandles[3 * 2 + 1] = ter->currentTerrainsHandles[3 * 2 + 2];
			ter->currentTerrainsHandles[3 * 1 + 1] = ter->currentTerrainsHandles[3 * 1 + 2];
			ter->currentTerrainsHandles[3 * 0 + 1] = ter->currentTerrainsHandles[3 * 0 + 2];

			ter->meshes[3 * 2 + 1] = ter->meshes[3 * 2 + 2];
			ter->meshes[3 * 1 + 1] = ter->meshes[3 * 1 + 2];
			ter->meshes[3 * 0 + 1] = ter->meshes[3 * 0 + 2];

			ter->currentTerrainsHandles[3 * 2 + 2] = tN;
			ter->currentTerrainsHandles[3 * 1 + 2] = mN;
			ter->currentTerrainsHandles[3 * 0 + 2] = bN;

			createMesh(ter->meshes[3 * 2 + 2], tN->pos.x, tN->pos.y,ter->scale);
			createMesh(ter->meshes[3 * 1 + 2], mN->pos.x, mN->pos.y, ter->scale);
			createMesh(ter->meshes[3 * 0 + 2], bN->pos.x, bN->pos.y, ter->scale);
		}
	}
	if(abs(yp) > (DIMENSION * ter->scale))
	{


		terrain* lN = NULL;
		terrain* mN = NULL;
		terrain* rN = NULL;

		if (yp < 0) // move down
		{
			int lX = ter->currentTerrainsHandles[0]->xP - 1;
			int lY = ter->currentTerrainsHandles[0]->yP - 1;
			int mX = ter->currentTerrainsHandles[1]->xP;
			int mY = ter->currentTerrainsHandles[1]->yP - 1;
			int rX = ter->currentTerrainsHandles[2]->xP + 1;
			int rY = ter->currentTerrainsHandles[2]->yP - 1;

			for (int i = 0; i < ter->allTerrains._size; i++) // not visited
			{
				if(ter->allTerrains.data[i]->xP == lX && ter->allTerrains.data[i]->yP == lY)
				{
					lN =  ter->allTerrains.data[i];
				}
				if (ter->allTerrains.data[i]->xP == mX && ter->allTerrains.data[i]->yP == mY)
				{
					mN = ter->allTerrains.data[i];
				}
				if (ter->allTerrains.data[i]->xP == rX && ter->allTerrains.data[i]->yP == rY)
				{
					rN = ter->allTerrains.data[i];
				}
			}
			if(lN == NULL)
			{
				terrain* m = ter->currentTerrainsHandles[0];
				//uint8_t* tMesh = (uint8_t*)malloc(sizeof(uint8_t)*DIMENSION*DIMENSION);
				//tN = (terrain*)malloc(sizeof(terrain));
				v2 newPos{ m->pos.x - (DIMENSION * ter->scale),m->pos.y - (DIMENSION * ter->scale) };
				lN = createNewTerrain(&ter->terrainAllocator, &ter->allTerrains, m->xP - 1, m->yP - 1, newPos);
				//createMesh(tMesh, m->xP - 1, m->yP + 1);
			}
			if (mN == NULL)
			{
				terrain* m = ter->currentTerrainsHandles[1];
				//uint8_t* tMesh = (uint8_t*)malloc(sizeof(uint8_t)*DIMENSION*DIMENSION);
				//tN = (terrain*)malloc(sizeof(terrain));
				v2 newPos{ m->pos.x ,m->pos.y - (DIMENSION * ter->scale) };
				mN = createNewTerrain(&ter->terrainAllocator, &ter->allTerrains, m->xP , m->yP - 1, newPos);
				//createMesh(tMesh, m->xP - 1, m->yP + 1);
			}
			if (rN == NULL)
			{
				terrain* m = ter->currentTerrainsHandles[2];
				//uint8_t* tMesh = (uint8_t*)malloc(sizeof(uint8_t)*DIMENSION*DIMENSION);
				//tN = (terrain*)malloc(sizeof(terrain));
				v2 newPos{ m->pos.x + (DIMENSION * ter->scale),m->pos.y - (DIMENSION * ter->scale) };
				rN = createNewTerrain(&ter->terrainAllocator, &ter->allTerrains, m->xP + 1, m->yP - 1, newPos);
				//createMesh(tMesh, m->xP - 1, m->yP + 1);
			}
			
			ter->currentTerrainsHandles[3 * 2] = ter->currentTerrainsHandles[3 * 1 ];
			ter->currentTerrainsHandles[3 * 2 + 1] = ter->currentTerrainsHandles[3 * 1 + 1];
			ter->currentTerrainsHandles[3 * 2 + 2] = ter->currentTerrainsHandles[3 * 1 + 2];

			ter->meshes[3 * 2] = ter->meshes[3 * 1 ];
			ter->meshes[3 * 2 + 1] = ter->meshes[3 * 1 + 1];
			ter->meshes[3 * 2 + 2] = ter->meshes[3 * 1 + 2];

			ter->currentTerrainsHandles[3 * 1 ] = ter->currentTerrainsHandles[3 * 0 ];
			ter->currentTerrainsHandles[3 * 1 + 1] = ter->currentTerrainsHandles[3 * 0 + 1];
			ter->currentTerrainsHandles[3 * 1 + 2] = ter->currentTerrainsHandles[3 * 0 + 2];

			ter->meshes[3 * 1 ] = ter->meshes[3 * 0];
			ter->meshes[3 * 1 + 1] = ter->meshes[3 * 0 + 1];
			ter->meshes[3 * 1 + 2] = ter->meshes[3 * 0 + 2];

			ter->currentTerrainsHandles[3 * 0 + 0] = rN;
			ter->currentTerrainsHandles[3 * 0 + 1] = mN;
			ter->currentTerrainsHandles[3 * 0 + 2] = lN;

			createMesh(ter->meshes[3 * 0 + 0], rN->pos.x, rN->pos.y,ter->scale);
			createMesh(ter->meshes[3 * 0 + 1], mN->pos.x, mN->pos.y, ter->scale);
			createMesh(ter->meshes[3 * 0 + 2], lN->pos.x, lN->pos.y, ter->scale);

		}
		else // move up
		{
			int lX = ter->currentTerrainsHandles[3 * 2]->xP - 1;
			int lY = ter->currentTerrainsHandles[3 * 2]->yP + 1;
			int mX = ter->currentTerrainsHandles[3 * 2 + 1]->xP;
			int mY = ter->currentTerrainsHandles[3 * 2 + 1]->yP + 1;
			int rX = ter->currentTerrainsHandles[3 * 2 + 2]->xP + 1;
			int rY = ter->currentTerrainsHandles[3 * 2 + 2]->yP + 1;

			for (int i = 0; i < ter->allTerrains._size; i++) // not visited
			{
				if (ter->allTerrains.data[i]->xP == lX && ter->allTerrains.data[i]->yP == lY)
				{
					lN = ter->allTerrains.data[i];
				}
				if (ter->allTerrains.data[i]->xP == mX && ter->allTerrains.data[i]->yP == mY)
				{
					mN = ter->allTerrains.data[i];
				}
				if (ter->allTerrains.data[i]->xP == rX && ter->allTerrains.data[i]->yP == rY)
				{
					rN = ter->allTerrains.data[i];
				}
			}
			if (lN == NULL)
			{
				terrain* m = ter->currentTerrainsHandles[3 * 2];
				//uint8_t* tMesh = (uint8_t*)malloc(sizeof(uint8_t)*DIMENSION*DIMENSION);
				//tN = (terrain*)malloc(sizeof(terrain));
				v2 newPos{ m->pos.x - (DIMENSION * ter->scale),m->pos.y + (DIMENSION * ter->scale) };
				lN = createNewTerrain(&ter->terrainAllocator, &ter->allTerrains, m->xP - 1, m->yP + 1, newPos);
				//createMesh(tMesh, m->xP - 1, m->yP + 1);
			}
			if (mN == NULL)
			{
				terrain* m = ter->currentTerrainsHandles[3 * 2 + 1];
				//uint8_t* tMesh = (uint8_t*)malloc(sizeof(uint8_t)*DIMENSION*DIMENSION);
				//tN = (terrain*)malloc(sizeof(terrain));
				v2 newPos{ m->pos.x ,m->pos.y + (DIMENSION * ter->scale) };
				mN = createNewTerrain(&ter->terrainAllocator, &ter->allTerrains, m->xP, m->yP + 1, newPos);
				//createMesh(tMesh, m->xP - 1, m->yP + 1);
			}
			if (rN == NULL)
			{
				terrain* m = ter->currentTerrainsHandles[3 * 2 + 1];
				//uint8_t* tMesh = (uint8_t*)malloc(sizeof(uint8_t)*DIMENSION*DIMENSION);
				//tN = (terrain*)malloc(sizeof(terrain));
				v2 newPos{ m->pos.x + (DIMENSION * ter->scale),m->pos.y + (DIMENSION * ter->scale) };
				rN = createNewTerrain(&ter->terrainAllocator, &ter->allTerrains, m->xP + 1, m->yP + 1, newPos);
				//createMesh(tMesh, m->xP - 1, m->yP + 1);
			}

			ter->currentTerrainsHandles[3 * 0] = ter->currentTerrainsHandles[3 * 1];
			ter->currentTerrainsHandles[3 * 0 + 1] = ter->currentTerrainsHandles[3 * 1 + 1];
			ter->currentTerrainsHandles[3 * 0 + 2] = ter->currentTerrainsHandles[3 * 1 + 2];

			ter->meshes[3 * 0] = ter->meshes[3 * 1];
			ter->meshes[3 * 0 + 1] = ter->meshes[3 * 1 + 1];
			ter->meshes[3 * 0 + 2] = ter->meshes[3 * 1 + 2];

			ter->currentTerrainsHandles[3 * 1] = ter->currentTerrainsHandles[3 * 2];
			ter->currentTerrainsHandles[3 * 1 + 1] = ter->currentTerrainsHandles[3 * 2 + 1];
			ter->currentTerrainsHandles[3 * 1 + 2] = ter->currentTerrainsHandles[3 * 2 + 2];

			ter->meshes[3 * 1] = ter->meshes[3 * 2];
			ter->meshes[3 * 1 + 1] = ter->meshes[3 * 2 + 1];
			ter->meshes[3 * 1 + 2] = ter->meshes[3 * 2 + 2];

			ter->currentTerrainsHandles[3 * 2 + 0] = rN;
			ter->currentTerrainsHandles[3 * 2 + 1] = mN;
			ter->currentTerrainsHandles[3 * 2 + 2] = lN;

			createMesh(ter->meshes[3 * 2 + 0], rN->pos.x, rN->pos.y,ter->scale);
			createMesh(ter->meshes[3 * 2 + 1], mN->pos.x, mN->pos.y, ter->scale);
			createMesh(ter->meshes[3 * 2 + 2], lN->pos.x, lN->pos.y, ter->scale);
		}
	}




	// älä puske kaikkea loppujen lopuksi
	if(true)
	{
		for (int h = 0; h < 3; h ++)
		{
		
			for (int w = 0; w < 3; w ++)
			{
				//printf("%d %d\n", h, w);
				terrain* current = ter->currentTerrainsHandles[h * 3 + w];
				uint8_t* mesh = ter->meshes[h * 3 + w];
				for (int hter = 0; hter < DIMENSION; hter++)
				{
					for (int wter = 0; wter < DIMENSION; wter++)
					{
						if(mesh[hter*DIMENSION + wter] > 0)
						{
						/*	printf("-- %d -- %d -- %d -- %d\n", hter, wter,w,h);

							if(wter == 66 && hter == 66 && w == 2)
							{
								int b = 10;
							}
*/
							InRender::push_to_renderer(rend,((current->pos.x - (DIMENSION * 0.5 * ter->scale)) + wter * ter->scale) ,
								((current->pos.y - (DIMENSION * 0.5 * ter->scale)) + hter * ter->scale), ter->scale, 0, 0, 1, 1);


						}
					}
					draw_debug_box(deb, current->pos.x - (DIMENSION* ter->scale) * 0.5f, current->pos.y - (DIMENSION* ter->scale) * 0.5f, DIMENSION * ter->scale, DIMENSION * ter->scale, 0);
				}


			}
		}
	}
	ter->update = false;

}
//void generate_mesh(tileId** mesh)
//{
//	int widht = 100;
//	int height = 100;
//	*mesh = (tileId*)calloc(widht*height,sizeof(tileId));
//	float offset = 0;
//	for(int h = 0; h < height; h++)
//	{
//		for (int w = 0; w < widht; w++)
//		{
//			float val = stb_perlin_noise3((w + c)/ 30.f, ( h )/30.f, 10, 256, 256, 256);
//			//float val = stb_perlin_ridge_noise3(w, h, w * h,  2,   0.5   , offset,3  ,0  ,  0  , 0);
//			tileId n = (tileId)(val < -0.1 ? 0 : 1);
//			(*mesh)[(h  *  widht) + w] = n;
//				// float stb_perlin_noise3(float x, float y, float z, int x_wrap, int y_wrap, int z_wrap);
//
//
//				// float stb_perlin_ridge_noise3(float x, float y, float z,
//				//                               float lacunarity, float gain, float offset, int octaves,
//				//                               int x_wrap, int y_wrap, int z_wrap)
//			offset += 0.007;
//		}
//	}
//			 c++;
//}



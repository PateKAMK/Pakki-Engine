#include <arrayD.h>
#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>
#include <perlinnoice.h>

static int c = 0;

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
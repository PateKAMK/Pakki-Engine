#include "filesystem.h"
#include "projectDefinitions.h"
#include <stdlib.h> 
#include <stdio.h>
#include <cstring>


#include "stb_image.h"
//#include <boost/filesystem.hpp>
#ifdef P_ANDROID
#include <android/asset_manager.h> // pate
#include <android/asset_manager_jni.h>//pate
#include <android/log.h>
#include <texture.h>
#include <memory.h>
#endif //P_ANDROID

#define MAXBUFLEN 1000000
namespace FileSystem
{
	static Texture picfilecache[maxpicfiles];

	void init_filesystem()
	{
		memset(picfilecache, 0, sizeof(picfilecache));
	}
#ifdef P_WINDOWS

	void load_file_to_buffer(const char* name, char** buffer, int* size,engine* engine)
	{
		FILE *f = fopen(name, "rb");
		if (f == NULL) {
			*buffer = NULL;
			return;
		}
		fseek(f, 0, SEEK_END);
		long fsize = ftell(f);
		fseek(f, 0, SEEK_SET);  //same as rewind(f);

		*buffer = (char*)malloc(fsize + 1);
		if (*buffer == NULL) return;
		fread(*buffer, fsize, 1, f);
		fclose(f);
		(*buffer)[fsize] = 0;
	}
	Texture load_sprite(picture_files file, engine* engine, bool loadOneChannel)
	{
		Texture texture;
		if(picfilecache[file].ID) //jos on muistissa jo niin �l� lataa
		{
			texture = picfilecache[file];
			return texture;
		}
		LOGI("loading %s\n",pic_file_names[file]);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //eetut
		memset(&texture, 0, sizeof &texture);//prank
		glGenTextures(1, &texture.ID);
		glBindTexture(GL_TEXTURE_2D, texture.ID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT/* GL_CLAMP_TO_EDGE *//*GL_REPEAT*/); //https://www.opengl.org/discussion_boards/showthread.php/167808-2D-texture-problem-lines-between-textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT /*GL_CLAMP_TO_EDGE*/  /*GL_REPEAT*/);
		
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // kokeile n�it� jos tulee ongelma textuurien v�leihin
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		unsigned char *data;
		if(!loadOneChannel)data = stbi_load(pic_file_names[file], &texture.widht, &texture.height, &texture.channels, 0);
		else data = stbi_load(pic_file_names[file], &texture.widht, &texture.height, &texture.channels, 1);

		if (!data) {
			LOGI("error loading %s", pic_file_names[file]);
			memset(&texture, 0, sizeof &texture);
			return texture;
		}
		if(!loadOneChannel)
		{
			if(texture.channels == 3)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.widht, texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else if (texture.channels == 4)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.widht, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				fatalerror(false);
			}
		}
		else 
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texture.widht, texture.height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		stbi_image_free(data);
		picfilecache[file] = texture;
		return texture;
	}
#endif // P_WINDOWS
#ifdef P_ANDROID
	void load_file_to_buffer(const char* name, char** buffer, int* size, engine* engine)
	{
		AAssetManager* ptrm = (AAssetManager*)(engine->platformState);
        AAsset* ptr = AAssetManager_open( ptrm,name, AASSET_MODE_STREAMING);
        size_t lenght = AAsset_getLength(ptr);
        *buffer = (char*)malloc(lenght +1);
        AAsset_read(ptr,*buffer,lenght);
        (*buffer)[lenght] = 0;
        //LOGI(buffer);
	}
	Texture load_sprite(picture_files file, engine* engine,bool loadOneChannel)
	{
		// android load pic
		Texture texture;
		if(picfilecache[file].ID) //jos on muistissa jo niin ala lataa
		{
			texture = picfilecache[file];
			return texture;
		}
        LOGI("loading texture %s",pic_file_names[file]);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //eetut
		memset(&texture, 0, sizeof texture);//prank
		glGenTextures(1, &texture.ID);
		glBindTexture(GL_TEXTURE_2D, texture.ID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE/* GL_CLAMP_TO_EDGE *//*GL_REPEAT*/); //https://www.opengl.org/discussion_boards/showthread.php/167808-2D-texture-problem-lines-between-textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE /*GL_CLAMP_TO_EDGE*/  /*GL_REPEAT*/);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		AAssetManager* ptrm = (AAssetManager*)(engine->platformState);
		AAsset* ptr = AAssetManager_open(ptrm, pic_file_names[file], AASSET_MODE_STREAMING);
        fatalerror(ptr);
		size_t lenght = AAsset_getLength(ptr);
		unsigned char* buffer = (unsigned char*)malloc(lenght);
		AAsset_read(ptr, buffer, lenght);
		//buffer[lenght] = 0;
		buffer = stbi_load_from_memory(buffer,lenght,&texture.widht,&texture.height,&texture.channels,STBI_default); //x y?

        if(texture.channels == 3) {
            //stbi_load_from_memory()
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.widht, texture.height, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, buffer);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else if(texture.channels == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.widht, texture.height, 0, GL_RGBA,
                         GL_UNSIGNED_BYTE, buffer);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        picfilecache[file] = texture;
		free(buffer); //membe

        return texture;
	}
#endif // P_ANDROID

}


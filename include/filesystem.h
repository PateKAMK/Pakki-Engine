#pragma once
#include <engine.h>
#include <texture.h>


#define TXT_FILES(FILE) \
		FILE(VERT_SHA)  \
		FILE(FRAG_SHA)  \

#define PNG_FILES(FILE) \
		FILE(linux_pingu)\
		FILE(gwen)\
		FILE(spritesheet_spaceships)\
		FILE(starfield)\
		FILE(starfield2)\
		FILE(starfield3)\
		FILE(spaceShips_009)\
		FILE(spaceEffects_006)\
		FILE(spaceEffects_009)\
		FILE(spaceStation_028)\
		FILE(spaceMeteors_004)\

#define JPG_FILES(FILE) \
		FILE(wall)		\
		FILE(lataus)	\

#define GENERATE_ENUM(ENUM) ENUM,

#define GENERATE_STRING(STRING) #STRING".txt",

#define GENERATE_STRINGPNG(STRING) #STRING".png",

#define GENERATE_STRINGJPG(STRING) #STRING".jpg"

static const char* txt_file_names[] = {
	TXT_FILES(GENERATE_STRING)
};
static const char* pic_file_names[] = {
	PNG_FILES(GENERATE_STRINGPNG)
	JPG_FILES(GENERATE_STRINGJPG)
};
enum txt_files
{
	TXT_FILES(GENERATE_ENUM)
	maxtxtfiles
};

enum picture_files {
	PNG_FILES(GENERATE_ENUM)
	JPG_FILES(GENERATE_ENUM)
	maxpicfiles
};




namespace FileSystem
{
	extern void load_file_to_buffer(const char* name,char** buffer,int* size,engine* engine);
	extern Texture load_sprite(picture_files file, engine* engine,bool loadOneChannel = false);
	extern void init_filesystem();
}
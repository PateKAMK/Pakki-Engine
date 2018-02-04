#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include "soundsystem.h"
#ifndef P_ANDROID

static std::vector<char *> gPathList;

void (*ss_Private_Error)(FMOD_RESULT, const char*, int);
void ss_Fatal(const char *format, ...);


void soundsystem::initsounds()
{
	ss = new soundsystem();
}

soundsystem* soundsystem::ss = NULL;

soundsystem::soundsystem():volume(0.1f),pitch(1.f)
{

    result = FMOD::System_Create(&fmodSystem);
    ERRCHECK(result);

    result = fmodSystem->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        std::cout << "FMOD Library version mismatch with header!" << std::endl;
    }

    result = fmodSystem->init(100, FMOD_INIT_NORMAL, extradriverdata);
    ERRCHECK(result);


	/*result = fmodSystem->createSound("Bag Raiders - Shooting Stars.mp3", FMOD_2D, nullptr, &shoottingstarsound);

	result = fmodSystem->playSound(shoottingstarsound, nullptr, true, &shoottingstarchannel);
	shoottingstarchannel->setVolume(volume);
	shoottingstarchannel->setPitch(1.f);

	result = fmodSystem->createSound("WATER BLIPY 1.wav", FMOD_2D, nullptr, &bliby);


	ERRCHECK(result);*/
}

soundsystem::~soundsystem()
{
    //for (auto &i : sound)
    //{
    //    result = i->release();
    //    ERRCHECK(result);
    //}

    result = fmodSystem->release();
    ERRCHECK(result);
}

void soundsystem::update(bool pause,float nvolume, float npitch)
{
    result = fmodSystem->update();
    ERRCHECK(result);

	/*bool playing = true;
	result = shoottingstarchannel->isPlaying(&playing);
	if(result == FMOD_ERR_INVALID_HANDLE)
	{
		result = fmodSystem->playSound(shoottingstarsound, nullptr, true, &shoottingstarchannel);
		shoottingstarchannel->setVolume(0.1);
		shoottingstarchannel->setPitch(1.f);
		ERRCHECK(result);

	}

	if(pause)
	{
		bool k;
		shoottingstarchannel->getPaused(&k);
		if(!k)
		{
			shoottingstarchannel->setPaused(true);
		}
	}
	if (!pause)
	{
		shoottingstarchannel->getPaused(&pause);
		if (pause)
		{
			shoottingstarchannel->setPaused(false);
		}
		else if(nvolume != volume)
		{
			volume = nvolume;
			shoottingstarchannel->setVolume(volume);
		}
		else if (npitch != pitch)
		{
			pitch = npitch;
			shoottingstarchannel->setPitch(pitch);
		}
	}*/
}


void soundsystem::ERRCHECK_fn(FMOD_RESULT, const char *file, int line)
{
    if  (result != FMOD_OK)
    {
        if(ss_Private_Error)
        {
            ss_Private_Error(result, file, line);
        }
        ss_Fatal("%s(%d): FMOD error %d - %s", file, line, result, FMOD_ErrorString(result));

    }
}

void soundsystem::playSound(bool paused)
{
	//FMOD::Channel* ch;
 //   result = fmodSystem->playSound(bliby, nullptr, paused, &ch);
	//ch->setVolume(this->volume);
	//ch->setPitch(this->pitch);
	////sound[sndID].
 //   ERRCHECK(result);
}

void soundsystem::loadSound(std::string path,int channelnum)
{
    //result = fmodSystem->createSound(MediaPath(fileToLoad), FMOD_DEFAULT, nullptr, &sound[channelnum]);
	/*result = fmodSystem->createSound(path.c_str(), FMOD_2D, nullptr, &sound[channelnum]);

    ERRCHECK(result);*/

}

const char *soundsystem::MediaPath(const char *fileName)
{
    //char *filePath = (char *)calloc(256, sizeof(char));

   // strcat(filePath, "../media/");
   // strcat(filePath, fileName);
  //  gPathList.push_back(filePath);

    return fileName;


}


void ss_Fatal(const char *format, ...)
{
    //char error[1024];
    std::string error;

    std::cout << "!!!Fatal error occurred in FMOD!!!" << std::endl;
    std::cout << std::endl;
    std::cout << error << std::endl;
    std::cout << std::endl;
}
#endif
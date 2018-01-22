#ifndef PAKKI_ENGINE_SOUNDSYSTEM_H
#define PAKKI_ENGINE_SOUNDSYSTEM_H

#include <projectDefinitions.h>
#ifndef P_ANDROID
#include <fmod\fmod.hpp>
#include <fmod\fmod_errors.h>

class soundsystem {

private:
    FMOD::System    *fmodSystem;
    FMOD::Sound     *sound[128];
    //FMOD::Channel   *channel[12];
	FMOD::Channel*	shoottingstarchannel;
	FMOD::Sound*	shoottingstarsound;
	FMOD::Sound*	bliby;
    FMOD_RESULT     result;
    unsigned int    version;
    void            *extradriverdata = nullptr;
	float			volume;
	float			pitch;
    void ERRCHECK_fn(FMOD_RESULT, const char *file, int line);
#define ERRCHECK(_result) ERRCHECK_fn(_result, __FILE__, __LINE__)

    const char *MediaPath(const char *fileName);


public:
    soundsystem();
    ~soundsystem();
	void    playSound(bool paused = false);
    void    loadSound(std::string path, int channelnum);
    void    update(bool pause, float nvolume,float npitch);
	static	soundsystem* ss;
	static	void initsounds();
};


#endif //PAKKI_ENGINE_SOUNDSYSTEM_H
#endif
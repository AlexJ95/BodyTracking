#pragma once

#include "pch.h"

#include <Kore/Audio1/Audio.h>
#include <Kore/Audio1/Sound.h>
#include <Kore/Audio1/SoundStream.h>
#include <Kore/Audio2/Audio.h>

#include <string>
#include <map>

class AudioManager
{
	static AudioManager* instance;

	std::map<std::string, Kore::Sound*> soundLibrary;

public:
	AudioManager* getInstance();
	AudioManager* getInstanceAndAppend(std::map<std::string, Kore::Sound*> sounds);

	AudioManager();
	AudioManager(std::map<std::string, Kore::Sound*> sounds);

	void play(std::string soundName);
};

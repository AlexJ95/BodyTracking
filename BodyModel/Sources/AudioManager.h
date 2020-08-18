#pragma once

#include <Kore/Graphics4/PipelineState.h> //this one is somehow needed for audio, so don't touch unless you know what you're doing
#include <Kore/Audio1/Audio.h>
#include <Kore/Audio1/Sound.h>
#include <Kore/Audio1/SoundStream.h>
#include <Kore/Audio2/Audio.h>
#include <string>
#include <map>

class AudioManager
{
	std::map<std::string, Kore::Sound*> soundLibrary;

public:
	AudioManager(std::map<std::string, Kore::Sound*> sounds);
	void play(std::string soundName);
};


#include "AudioManager.h"

AudioManager::AudioManager(std::map<std::string, Kore::Sound*> sounds)
{
	Kore::Audio1::init();
	Kore::Audio2::init();
	soundLibrary = sounds;
}

void AudioManager::play(std::string soundName)
{
	Kore::Audio1::play(soundLibrary.find(soundName)->second);
}
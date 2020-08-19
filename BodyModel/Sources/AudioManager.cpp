
#include "AudioManager.h"

AudioManager* AudioManager::instance;

AudioManager* AudioManager::getInstanceAndAppend(std::map<std::string, Kore::Sound*> sounds)
{
	if (!instance)
		instance = new AudioManager(sounds);
	else instance->soundLibrary.insert(sounds.begin(), sounds.end());
	return instance;
}

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
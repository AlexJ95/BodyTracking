
#include "AudioManager.h"

AudioManager::AudioManager(map<string, Sound*> sounds)
{
	Audio1::init();
	Audio2::init();
	soundLibrary = sounds;
}

void AudioManager::play(string soundName)
{
	Audio1::play(soundLibrary.find(soundName)->second);
}

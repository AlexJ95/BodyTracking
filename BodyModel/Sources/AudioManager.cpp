
#include "AudioManager.h"

AudioManager* AudioManager::instance;

AudioManager* AudioManager::getInstance()
{
	if (!instance)
		instance = new AudioManager();
	return instance;
}

AudioManager* AudioManager::getInstanceAndAppend(std::map<std::string, Kore::Sound*> sounds)
{
	if (!instance)
		instance = new AudioManager(sounds);
	else {	instance->soundLibrary.insert(sounds.begin(), sounds.end()); }
	return instance;
}

AudioManager::AudioManager()
{
	Kore::Audio1::init();
	Kore::Audio2::init();
}

AudioManager::AudioManager(std::map<std::string, Kore::Sound*> sounds) : AudioManager()
{
	soundLibrary = sounds;
}

void AudioManager::play(std::string soundName, double deltaT)
{
	auto search = isPlaying.find(soundName);
	if (search != isPlaying.end()) {
		Wav* wav = isPlaying.find(soundName)->second;
		if (!wav->isPlayed) {
			Kore::Audio1::play(soundLibrary.find(soundName)->second);
			wav->isPlayed = true;
		}else waitEnd(soundName, (float)deltaT);
	}else instance->isPlaying.insert({ soundName,new Wav(soundLibrary.find(soundName)->second->length) });

	
}

void AudioManager::waitEnd(std::string soundName, float deltaT) {
	Wav* wav = isPlaying.find(soundName)->second;
	if (wav->currentSoundLength < wav->soundLength) { wav->currentSoundLength += deltaT; }
	else {
		wav->isPlayed = false;
		wav->currentSoundLength = 0;
	}
}

void AudioManager::play(std::string soundName)
{
	Kore::Audio1::play(soundLibrary.find(soundName)->second);
}

Kore::Sound* AudioManager::getSound(std::string soundName) {

	return soundLibrary.find(soundName)->second;
}

AudioManager::Wav::Wav(float musicLength)
{	
	isPlayed = false;
	soundLength = musicLength;
	currentSoundLength = 0;
}



#include <Kore/Audio2/Audio.h>
#include <Kore/Audio1/Audio.h>
#include <Kore/Audio1/Sound.h>
#include <Kore/Audio1/SoundStream.h>
#include <string>
#include <map>

using namespace Kore;
using namespace std;

#pragma once
class AudioManager
{
	map<string, Sound*> soundLibrary;

public:
	AudioManager(map<string, Sound*> sounds);
	void play(string soundName);
};

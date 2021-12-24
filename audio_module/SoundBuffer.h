#pragma once
#include <AL/al.h>
#include <vector>
class SoundBuffer
{
public:
	SoundBuffer();
	~SoundBuffer();

	ALuint addSoundEffect(const char* filename);
	bool removeSoundEffect(const ALuint& buffer);
	
private:
	std::vector<ALuint> p_SoundEffectBuffers;
};


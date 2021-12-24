#pragma once
#include <AL/al.h>

class SoundSource
{
public:
	SoundSource();
	~SoundSource();

	void Play(const ALuint& buffer_to_play);
	void Stop();
	void Pause();
	void Resume();

	void SetBufferToPlay(const ALuint& buffer_to_play);
	void SetLooping(const bool& loop);
	void SetPosition(const float& x, const float& y, const float& z);
	void SetVelocity(const float& x, const float& y, const float& z);
	void SetState(bool& state);
	bool GetState();
	void SetType(int& type);
	int GetType();
	bool isPlaying();

private:
	ALuint p_Source;
	float p_Pitch = 1.f;
	float p_Gain = 1.f;
	float p_Position[3] = { 0,0,0 };
	float p_Velocity[3] = { 0,0,0 };
	bool p_LoopSound = true;
	bool source_state = false;			// false for idle, true for moving'
	ALuint source_type;
	ALuint p_Buffer = 0;	
};


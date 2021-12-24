#include "SoundSource.h"
#include <iostream>
#include <chrono>
#include "OpenALErrorCheck.h"

SoundSource::SoundSource()
{
	alGenSources(1, &p_Source);
	alSourcef(p_Source, AL_PITCH, p_Pitch);
	alSourcef(p_Source, AL_GAIN, p_Gain);
	alSource3f(p_Source, AL_POSITION, p_Position[0], p_Position[1], p_Position[2]);
	alSource3f(p_Source, AL_VELOCITY, p_Velocity[0], p_Velocity[1], p_Velocity[2]);
	alSourcei(p_Source, AL_LOOPING, p_LoopSound);
	alSourcei(p_Source, AL_BUFFER, p_Buffer);
}

SoundSource::~SoundSource()
{
	alDeleteSources(1, &p_Source);
}

void SoundSource::Play(const ALuint& buffer_to_play)
{
	if (buffer_to_play != p_Buffer){
		p_Buffer = buffer_to_play;
		alSourcei(p_Source, AL_BUFFER, (ALint)p_Buffer);
		AL_CheckAndThrow();
	}
	alSourcePlay(p_Source);
	AL_CheckAndThrow();
}

void SoundSource::Stop()
{
	alSourceStop(p_Source);
	AL_CheckAndThrow();
}

void SoundSource::Pause()
{
	alSourcePause(p_Source);
	AL_CheckAndThrow();
}

void SoundSource::Resume()
{
	alSourcePlay(p_Source);
	AL_CheckAndThrow();
}

void SoundSource::SetBufferToPlay(const ALuint& buffer_to_play)
{
	if (buffer_to_play != p_Buffer)
	{
		p_Buffer = buffer_to_play;
		alSourcei(p_Source, AL_BUFFER, (ALint)p_Buffer);
		AL_CheckAndThrow();
	}
}

void SoundSource::SetLooping(const bool& loop)
{
	alSourcei(p_Source, AL_LOOPING, (ALint)loop);
	AL_CheckAndThrow();
}

void SoundSource::SetPosition(const float& x, const float& y, const float& z)
{
	alSource3f(p_Source, AL_POSITION, x, y, z);
	AL_CheckAndThrow();
}

void SoundSource::SetVelocity(const float& x, const float& y, const float& z)
{
	alSource3f(p_Source, AL_VELOCITY, x, y, z);
	AL_CheckAndThrow();
}

void SoundSource::SetState(bool& state){
	if(state != source_state){
		source_state = state;
	}
}

bool SoundSource::GetState(){
	return source_state;
}

void SoundSource::SetType(int& type){
	if(source_type != type){
		source_type = type;
	}
}

int SoundSource::GetType(){
	return source_type;
}

bool SoundSource::isPlaying()
{
	ALint playState;
	alGetSourcei(p_Source, AL_SOURCE_STATE, &playState);
	return (playState == AL_PLAYING);
}
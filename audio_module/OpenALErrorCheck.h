#pragma once
#include <AL/alc.h>
#include <AL/al.h>


static void ALC_CheckAndThrow(ALCdevice* device)
{
	if (ALCenum err = alcGetError(device) != ALC_NO_ERROR)
	{
		std::cout << "ALC ERROR CODE" << err << std::endl;
		if(err == ALC_INVALID_DEVICE){
			std::cout << "Error, invalid device" << std::endl;
		}
		else if(err == ALC_INVALID_CONTEXT){
			std::cout << "Error, invalid context" << std::endl;
		}
		else if(err == ALC_INVALID_ENUM){
			std::cout << "Error, invalid enum" << std::endl;
		}
		else if(err == ALC_INVALID_VALUE){
			std::cout << "Error, invalid value" << std::endl;
		}
		else if(err == ALC_OUT_OF_MEMORY){
			std::cout << "Error, out of memory" << std::endl;
		}
	}
}


static void AL_CheckAndThrow()
{
	if (ALenum err = alGetError() != AL_NO_ERROR)
	{
		std::cout << "AL ERROR CODE" << err << std::endl;
		if(err == AL_INVALID_NAME){
			std::cout << "Error, invalid device" << std::endl;
		}
		else if(err == AL_INVALID_OPERATION){
			std::cout << "Error, invalid context" << std::endl;
		}
		else if(err == AL_INVALID_ENUM){
			std::cout << "Error, invalid enum" << std::endl;
		}
		else if(err == AL_INVALID_VALUE){
			std::cout << "Error, invalid value" << std::endl;
		}
		else if(err == AL_OUT_OF_MEMORY){
			std::cout << "Error, invalid memory" << std::endl;
		}
	}
}
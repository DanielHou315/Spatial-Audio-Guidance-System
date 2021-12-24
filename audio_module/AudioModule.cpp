#include <iostream>
#include <string.h>
#include <vector>
#include <map>
#include <algorithm>
#include <AL/alc.h>
#include <AL/al.h>
#include <stdio.h>
#include "SoundBuffer.h"
#include "SoundSource.h"
#include "OpenALErrorCheck.h"

extern "C"
{
// Global Variables

    // Audio Device and Context, Required by OpenAL
    ALCdevice * p_ALCDevice;
    ALCcontext * p_ALCContext;

    SoundBuffer * Buffers = new SoundBuffer();                  // A buffer of sounds, to load in the audio effects
    std::map<int, ALuint> Buffer_map;                           // A Buffer map, to record pointer to the buffer
    
    // Sound Sources
    static int number_of_IDs_registered = 0;                    // This variable gives one unique ID for each object detected throughout the program.
    std::map<int, SoundSource*> Source_map;                     // This links unique ID to one unique source
    std::map<int,int> Source_ID_map;                            // This links existing ID (maybe recycled) to its unique ID. 
                                                                // This is not needed for the built-in ZED model but may come in handy for other object detection models that recycle labels.
    
    std::vector<int> previous_source;                           // A vector of Sources detecetd in the last cycle. This is a permanently maintained vector, unlike current_source. 
    std::vector<int> current_source;                            // A vector recording objects detected in the current round -- it is compared to Source list, which holds Sources from the last cycle. 
                                                                // This is cleared at the end of each cycle. 



// Starting and Ending Audio Processing Related

    // OpenAL Initialization
    void audio_init()
    {
        p_ALCDevice = alcOpenDevice(nullptr); // nullptr = get default device
        if (!p_ALCDevice)
            throw("failed to get sound device");

        p_ALCContext = alcCreateContext(p_ALCDevice, nullptr);  // create context
        if (!p_ALCContext)
            throw("Failed to set sound context");

        if (!alcMakeContextCurrent(p_ALCContext))   // make context current
            throw("failed to make context current");

        const ALCchar* name = nullptr;
        if (alcIsExtensionPresent(p_ALCDevice, "ALC_ENUMERATE_ALL_EXT"))
            name = alcGetString(p_ALCDevice, ALC_ALL_DEVICES_SPECIFIER);
        if (!name || alcGetError(p_ALCDevice) != AL_NO_ERROR)
            name = alcGetString(p_ALCDevice, ALC_DEVICE_SPECIFIER);
        printf("Opened \"%s\"\n", name);
        return;
    }

    // Destroy Context
    void audio_destroy()
    {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(p_ALCContext);
        alcCloseDevice(p_ALCDevice);
        return;
    }

    // Set Attenuation of the Program. Default is key == 1. 
    void SetAttunation(int key)
    {
        if(key == 1){
            alDistanceModel(AL_INVERSE_DISTANCE);
            AL_CheckAndThrow();
        }
        else if(key == 2){
            alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
            AL_CheckAndThrow();
        }
        else if(key == 3){
            alDistanceModel(AL_LINEAR_DISTANCE);
            AL_CheckAndThrow();
        }
        else if(key == 4){
            alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
            AL_CheckAndThrow();
        }
        else if(key == 5){
            alDistanceModel(AL_EXPONENT_DISTANCE);
            AL_CheckAndThrow();
        }
        else if(key == 6){
            alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
            AL_CheckAndThrow();
        }
        return;
    }

    /*
    // Set location of the user
    void SetLocation(const float& x, const float& y, const float& z)
    {
        alListener3f(AL_POSITION, x, y, z);
        AL_CheckAndThrow();
    }

    // Set velocity of the user. In our case, the camera is set to be still, and everything moves relative to the user. 
    void SetVelocity(const float& x, const float& y, const float& z)
    {
        alListener3f(AL_VELOCITY, x, y, z);
        AL_CheckAndThrow();
    }

    // Set orientation of the user, defined by two normal vectors. See OpenAL Documentation for more details. 
    void SetOrientation(const float& atx, const float& aty, const float& atz, const float& upx, const float& upy, const float& upz)
    {
        std::vector<float> ori;
        ori.push_back(atx);
        ori.push_back(aty);
        ori.push_back(atz);
        ori.push_back(upx);
        ori.push_back(upy);
        ori.push_back(upz);
        alListenerfv(AL_ORIENTATION, ori.data());
        AL_CheckAndThrow();
    }
    */

    // Set gain of audio output. 
    void SetGain(const float& val)
    {
        // clamp between 0 and 5
        float newVol = val;
        if (newVol < 0.f)
            newVol = 0.f;
        else if (newVol > 5.f)
            newVol = 5.f;

        alListenerf(AL_GAIN, newVol);
        AL_CheckAndThrow();
        return;
    }





// Buffer Related Functions

    // Create a buffer, called at the beginning of the function. 
    void create_buffers()
    {
        // Load Warning sound, for the purpose of this program, we are not using the warning sound. 
        ALuint bf = Buffers->addSoundEffect("sound_source/buffer/warning.wav");
        Buffer_map[0] = bf;
        // Load Person Sound
        bf = Buffers->addSoundEffect("sound_source/buffer/person.wav");
        Buffer_map[1] = bf;
        // Load Bicycle Sound
        bf = Buffers->addSoundEffect("sound_source/buffer/bike.wav");
        Buffer_map[2] = bf;
        // Load Car Sound
        bf = Buffers->addSoundEffect("sound_source/buffer/car.wav");
        Buffer_map[3] = bf;
        // Load Bus Sound
        bf = Buffers->addSoundEffect("sound_source/buffer/bus.wav");
        Buffer_map[4] = bf;
        // Load Cat Sound
        bf = Buffers->addSoundEffect("sound_source/buffer/cat.wav");
        Buffer_map[6] = bf;
        // Load Dog Sound
        bf = Buffers->addSoundEffect("sound_source/buffer/dog.wav");
        Buffer_map[7] = bf;
        return;
    }




// Source Related Functions

    // Determine if an ID is a new source: return 1 for "ID is new", and 0 for "ID has been loaded and rendered by OpenAL before"
    bool is_new_source(int ID){
        if (std::find(previous_source.begin(),previous_source.end(),ID) != previous_source.end()) {return 0;}
        return 1;
    }

    // Create a new source (corresponding to an object detected by ZED)
    void create_source(int ID, int type){
        if (is_new_source(ID)){
            previous_source.push_back(ID);
            Source_ID_map[ID] = number_of_IDs_registered;
            Source_map[number_of_IDs_registered] = new SoundSource();
            number_of_IDs_registered++;
            Source_map[Source_ID_map[ID]]->Play(Buffer_map[type]);
            Source_map[Source_ID_map[ID]]->SetLooping(true);
        }
        else{std::cout << "There is an error creating source: source ID already created" << std::endl;}
        return;
    }

    // Update Velocity of an object
    void update_source_velocity(int ID, float x,float y,float z){
        Source_map[Source_ID_map[ID]]->SetVelocity(x,y,z);
        return;
    }

    // Update Position of an object
    void update_source_position(int ID, float x,float y,float z){
        Source_map[Source_ID_map[ID]]->SetPosition(x,y,z);
        return;
    }

    // Update State of an object (ie. moving or static) -- this is not used for the purpose of this demo. 
    void update_source_state(int ID, bool state){
        Source_map[Source_ID_map[ID]]->SetState(state);
        return;
    }

    // Update Source Type of an object (i.e. ZED decides that the object should actually be a car instead of a bike, which very rarely happens)
    void update_source_type(int ID, int type){
        if(Source_map[Source_ID_map[ID]]->GetType() != type){
            Source_map[Source_ID_map[ID]]->Play(Buffer_map[type]);
        }
        return;
    }

    // Pause a source
    void pause_source(int ID){
        Source_map[Source_ID_map[ID]]->Pause();
        return;
    }

    // Resume a source
    void resume_source(int ID){
        Source_map[Source_ID_map[ID]]->Resume();
        return;
    }

    // Determine if a source is playing: return 1 for yes, 0 for no. 
    int is_source_playing(int ID){
        if(Source_map[Source_ID_map[ID]]->isPlaying()){
            return 1;
        }
        return 0;
    }

    // Delete a source (i.e. when it is no longer detected by ZED)
    void delete_source(int ID){
        if (!is_new_source(ID)){
            Source_map[Source_ID_map[ID]]->Stop();
            Source_map[Source_ID_map[ID]]->~SoundSource();
            Source_map.erase(Source_ID_map[ID]);
            Source_ID_map.erase(ID);
            for(int i = 0;i < previous_source.size();i++){
                if(previous_source[i] == ID){
                    previous_source.erase(previous_source.begin()+i);
                    return;
                }
            }
        }
        else{std::cout << "This ID is not REGISTERED so not DELETED!!!" << std::endl;}
        return;
    }

    // Delete all sources, called upon when no object is detected by ZED. 
    void delete_all_sources(){
        int i = previous_source.size()-1;
        while (i >= 0){
            delete_source(previous_source[i]);
            i--;
        }
        current_source.clear();
        return;
    }
    
// Keeping Source Record Related 
    // Add record of an object detected in the current cycle
    void add_current_source(int ID){
        current_source.push_back(ID);
        return;
    }

    // Determine if an object is detected in the current cycle
    bool is_in_current_source(int ID){
        if(find(current_source.begin(),current_source.end(),ID) == current_source.end()){
            return 0;
        }
        return 1;
    }

    // Cleanup previous_source, which is the permanently maintained list of sources being rendered. 
    void cleanup_current_source(){
        // If a source in previous_source does not appear in current_source, it is deleted. 
        // All new sources added this turn are pushed into previous_source as well as current_source, see ../video_module/AudioInterface.py
        for(short i = 0;i < previous_source.size();){
            if(find(current_source.begin(),current_source.end(),previous_source[i]) == current_source.end()){
                delete_source(previous_source[i]);
            }
            else{i++;}
        }
        // Clear the current_source list for next cycle. 
        current_source.clear();
        return;
    }



// Final Cleanup Function, called when the entire program ends. 
    void audio_cleanup(){
        delete_all_sources();
        Buffers->~SoundBuffer();
        audio_destroy();
        std::cout << "Audio Cleanup done" << std::endl;
        return;
    }
}
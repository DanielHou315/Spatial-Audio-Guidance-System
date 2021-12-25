# Spatial Audio Guidance System (SAGS)
A Guiding system prototype for visually impaired using spatial audio.

# Features
This program uses a ZED 2 camera to detect objects and their spatial positions. Then, OpenAL renders spatial audio signals that indicates the position of the objects detected.

On a Jetson Xavier NX, this program can update all object positions 20-30 times per second, and it will detect people, cars, trucks, buses, bicycles, motorcycles, cats, and dogs, using the built-in object detection module. The maximum detection distance is 35 meters, which should leave sufficient time for users to react to signals.

One limitation of the program is that it currently lacks any filtering algorithm to choose which objects to render and which not. In crowded scenes, where multiple people or cars pass by, this program could fail or generate audio that is indistinguishable. Future research into prioritizing rendering sequence may resolve the problem. 


# Hardware
## Nvidia Jetson Xavier NX
The development and execution of SAGS is done on [Nvidia Jetson Xavier NX platform](https://www.nvidia.com/en-us/autonomous-machines/embedded-systems/jetson-xavier-nx/) in Jetpack 4.5. I have not tested this program on other Jetson platforms or x86 PCs. 

## Stereolabs ZED 2 Stereo Camera
The depth sensing and object detection are empowered by a single [ZED 2 camera](https://www.stereolabs.com/zed-2/). The ZED 2 camera requires CUDA to achieve some of its features, so a CUDA-enabled Jetson board or Nvidia graphics card is required by this program. 

## Other Hardware
To achieve outdoor mobility, a mobile power supply is required to power Jetson Xavier NX. No extra power supply is needed if the program runs on a notebook. 

To immerse in a spatial audio environment, a dual-channel headphone is required. Both bluetooth and wired USB headphones have been used for this project. 

# Software
This program is developed primarily with Python, ZED SDK, and OpenAL.

The audio framework of this program is programmed in C++, while the video handling is in Python. This is to enable future expandability of object recognition models in Python, such as YOLO integration. 


# Setup
## python
Install Python 3 and pip. This program is developed with Python 3.6.9. Higher versions of Python may not work with ZED SDK, so check documentation before you install. Some platforms ship with Python3 already.
## ZED SDK
Install [ZED SDK](https://www.stereolabs.com/developers/) on your machine to drive ZED cameras. Python API is required by SAGS. This program is developed under ZED SDK 3.5.\
Before you move on, run diagnostics and calibrate your ZED camera with tools located in the folder /usr/local/zed/tools. 
## CUDA
Install [CUDA](https://developer.nvidia.com/cuda-toolkit). Make sure your CUDA version matches the requirement of your ZED SDK. Jetpack ships with CUDA. 
## Dependencies
Install SAGS dependencies with apt. Copy and paste this command into terminal to install all dependencies: 
```
sudo apt install libopenal1 libopenal-dev libsndfile-dev gcc
pip3 install ctypes datetime playsound
```
## Audio Dynamically Linked Library
AudioInterface.py calls on the audio module by referring to a .so library called audiolib.so, located in the audio_module folder. The provided audiolib.so is compiled for Jetson Xavier NX. To compile for your machine, direct to audio_module folder, then copy and paste this command into the terminal: 
```
g++ -shared -fPIC -o audio.so SoundSource.cpp SoundBuffer.cpp AudioModule.cpp -lsndfile -lopenal
```
(I know, I know, I should've used CMake. That will come in a future update.)
## Run the Program
```
python3 main.py
```
## Recording
For the purpose of creating a demo, I enabled the recording feature in this program.

Every recording will be saved in the Recordings folder as a .svo file.

To export .svo to .avi, follow instructions on [Stereolabs Support](https://support.stereolabs.com/hc/en-us/articles/360009986754-How-do-I-convert-SVO-files-to-AVI-or-image-depth-sequences-).


To turn recording off, comment out the function "enable_recording()" in line 169 of main.py.

# Acknowledgement
The project is not possible without the supported of Mr. Hou and Ms. Zhang.

The OpenAL framework of this project is based on the work of Youtuber "Code, Tech, and Tutorials". Check out his channel [here](https://www.youtube.com/c/CodeTechandTutorials) and his code repository [here](https://github.com/codetechandtutorials/openal-impl).

The power supply that enabled outdoor testing of this program is sponsored by the Haverford School Science Department. 
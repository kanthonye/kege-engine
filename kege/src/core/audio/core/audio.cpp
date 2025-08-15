//
//  Audio.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/6/25.
//

#include "audio.hpp"
namespace kege::audio{

//    void Device::setListenerOrientation(const kege::vec3& forward, const kege::vec3& up)
//    {
//        float orientation[] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
//        alListenerfv(AL_ORIENTATION, orientation);
//    }
//    void Device::setListenerPosition(const kege::vec3& pos)
//    {
//        alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
//    }
//    void Device::setListenerVelocity(const kege::vec3& vel)
//    {
//        alListener3f(AL_VELOCITY, vel.x, vel.y, vel.z);
//    }
//
//    kege::vec3 Device::getListenerVelocity() const {
//        kege::vec3 vel;
//        alGetListener3f(AL_VELOCITY, &vel.x, &vel.y, &vel.z);
//        return vel;
//    }
//
//    Device::Device()
//    {
//        // Open default audio device
//        _device = alcOpenDevice(nullptr);
//        if ( !_device )
//        {
//            std::cerr << "Failed to open OpenAL device!" << std::endl;
//            return;
//        }
//
//        // Create and activate context
//        _context = alcCreateContext(_device, nullptr);
//        if (!_context)
//        {
//            std::cerr << "Failed to create OpenAL context!" << std::endl;
//            alcCloseDevice(_device);
//            return;
//        }
//        alcMakeContextCurrent(_context);
//
//        alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
//        alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
//
//        float orientation[] = { 0.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f };
//    }
//
//    Device::~Device() {
//        alcMakeContextCurrent(nullptr);
//        alcDestroyContext(_context);
//        alcCloseDevice(_device);
//    }
//
//    void Device::printAudioInfo() {
//        const ALCchar* deviceName = alcGetString(_device, ALC_DEVICE_SPECIFIER);
//        std::cout << "OpenAL Device: " << (deviceName ? deviceName : "Unknown") << std::endl;
//    }
}

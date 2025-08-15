//
//  audio-source.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/6/25.
//

#include "audio-source.hpp"

//#include <OpenAL/al.h>


//namespace kege::audio{
//
//
//    kege::vec3 Source::getVelocity() const {
//        kege::vec3 vel;
//        alGetSource3f(_id, AL_VELOCITY, &vel.x, &vel.y, &vel.z);
//        return vel;
//    }
//
//    Source::Source()
//    {
//        alGenSources(1, &_id);
//        alSourcef(_id, AL_GAIN, 1.0f);  // Default volume
//        alSource3f(_id, AL_POSITION, 0.0f, 0.0f, 0.0f);
//    }
//
//    Source::~Source()
//    {
//        alDeleteSources(1, &_id);
//    }
//
//    void Source::play()
//    {
//        alSourcePlay(_id);
//    }
//
//    void Source::pause()
//    {
//        alSourcePause(_id);
//    }
//
//    void Source::stop()
//    {
//        alSourceStop(_id);
//    }
//
//    void Source::setLooping(bool loop)
//    {
//        alSourcei(_id, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
//    }
//
//    void Source::setVolume(float volume)
//    {
//        alSourcef(_id, AL_GAIN, volume);
//    }
//
//    void Source::attachBuffer(const audio::Buffer& buffer )
//    {
//        alSourcei(_id, AL_BUFFER, buffer.getID());
//    }
//
//    void Source::setPosition(const vec3& pos)
//    {
//        alSource3f(_id, AL_POSITION, pos.x, pos.y, pos.z);
//    }
//
//    void Source::setVelocity(const vec3& vel)
//    {
//        alSource3f(_id, AL_VELOCITY, vel.x, vel.y, vel.z);
//    }
//}

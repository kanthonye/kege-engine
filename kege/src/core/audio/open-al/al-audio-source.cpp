//
//  audio-source.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/6/25.
//

#define OPENAL_DEPRECATED
#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#include "al-audio-source.hpp"



namespace kege::audio::al{


    Source::Source()
    :   _id(0)
    ,   _is_playing(false)
    {
        alGenSources(1, &_id);
        CheckALError("Failed to generate audio source");
    }

    Source::~Source()
    {
        stop();
        if ( _id )
        {
            alDeleteSources(1, &_id);
            _id = 0;
        }
    }

    void Source::play()
    {
        if (_id)
        {
            alSourcePlay(_id);
            if (CheckALError("Failed to play audio source"))
            {
                _is_playing = true;
                std::cout << "Audio source is playing." << std::endl;
            }
        }
    }

    void Source::pause() {
        if (_id) {
            alSourcePause(_id);
            if (CheckALError("Failed to pause audio source"))
            {
                _is_playing = false;
                std::cout << "Audio source is paused." << std::endl;
            }
        }
    }

    void Source::stop() {
        if (_id) {
            alSourceStop(_id);
            if (CheckALError("Failed to stop audio source"))
            {
                _is_playing = false;
                std::cout << "Audio source is stopped." << std::endl;
            }
        }
    }

    void Source::setBuffer(const Ref< audio::Buffer >& buffer)
    {
        if ( _id )
        {
            alSourcei(_id, AL_BUFFER, buffer->getID() );
            CheckALError("Failed to set audio buffer");
        }
    }

    void Source::setPosition( const kege::vec3 &pos )
    {
        if ( _id )
        {
            alSource3f(_id, AL_POSITION, pos.x, pos.y, pos.z);
            CheckALError("Failed to set audio source position");
        }
    }

    void Source::setVelocity( const kege::vec3 &vel )
    {
        if ( _id )
        {
            alSource3f(_id, AL_VELOCITY, vel.x, vel.y, vel.z);
            CheckALError("Failed to set audio source velocity");
        }
    }

    void Source::setDirection( const kege::vec3 &dir )
    {
        if ( _id )
        {
            alSource3f(_id, AL_DIRECTION, dir.x, dir.y, dir.z);
            CheckALError("Failed to set audio source direction");
        }
    }

    void Source::setVolume(float volume)
    {
        if ( _id )
        {
            alSourcef(_id, AL_GAIN, volume);
            CheckALError("Failed to set audio source volume");
        }
    }

    void Source::setPitch(float pitch)
    {
        if ( _id )
        {
            alSourcef(_id, AL_PITCH, pitch);
            CheckALError("Failed to set audio source pitch");
        }
    }

    void Source::setLooping(bool loop)
    {
        if ( _id )
        {
            alSourcei(_id, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
            CheckALError("Failed to set audio source looping");
        }
    }

    bool Source::isPlaying() const
    {
        if ( _id )
        {
            ALint state;
            alGetSourcei(_id, AL_SOURCE_STATE, &state);
            return state == AL_PLAYING;
        }
        return false;
    }

    bool Source::CheckALError(const std::string& message)
    {
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            std::cerr << message << " (OpenAL Error: " << error << ")" << std::endl;
            return false;
        }
        return true;
    }

}

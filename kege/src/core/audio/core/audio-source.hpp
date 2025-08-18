//
//  audio-source.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/6/25.
//

#ifndef audio_source_hpp
#define audio_source_hpp

#include "audio-buffer.hpp"
#include "audio-stream.hpp"

namespace kege::audio{

    class Buffer;

    class Source : public kege::RefCounter
    {
    public:

        // Set the audio buffer to play
        virtual void setBuffer( const Ref< audio::Buffer >& buffer ) = 0;

        // Set the position of the audio source in 3D space
        virtual void setPosition( const kege::vec3& pos ) = 0;

        // Set the velocity of the audio source (for Doppler effect)
        virtual void setVelocity( const kege::vec3& vel ) = 0;

        // Set the direction of the audio source (for directional sounds)
        virtual void setDirection( const kege::vec3& dir ) = 0;

        // Set the volume (gain) of the audio source (0.0 to 1.0)
        virtual void setVolume(float volume) = 0;

        // Set the pitch of the audio source (1.0 is normal speed)
        virtual void setPitch(float pitch) = 0;

        // Set whether the audio source should loop
        virtual void setLooping(bool loop) = 0;

        // Check if the audio source is currently playing
        virtual bool isPlaying()const = 0;

        // Play the audio source
        virtual void play() = 0;

        // Pause the audio source
        virtual void pause() = 0;

        // Stop the audio source
        virtual void stop() = 0;

        virtual ~Source(){}
    };

}

#endif /* audio_source_hpp */

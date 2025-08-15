//
//  audio-source.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/6/25.
//

#ifndef oal_audio_source_hpp
#define oal_audio_source_hpp

#include "../core/audio-source.hpp"

namespace kege::audio::al{

    class Source : public audio::Source{
    public:

        // Set the audio buffer to play
        void setBuffer( const Ref< audio::Buffer >& buffer );

        // Set the position of the audio source in 3D space
        void setPosition( const kege::vec3& pos );

        // Set the velocity of the audio source (for Doppler effect)
        void setVelocity( const kege::vec3& vel );

        // Set the direction of the audio source (for directional sounds)
        void setDirection( const kege::vec3& dir );

        // Set the volume (gain) of the audio source (0.0 to 1.0)
        void setVolume(float volume);

        // Set the pitch of the audio source (1.0 is normal speed)
        void setPitch(float pitch);

        // Set whether the audio source should loop
        void setLooping(bool loop);

        // Check if the audio source is currently playing
        bool isPlaying() const;

        // Play the audio source
        void play();

        // Pause the audio source
        void pause();

        // Stop the audio source
        void stop();

        Source();
        ~Source();

    private:

        uint32_t _id; // OpenAL source ID
        bool _is_playing;  // Whether the source is currently playing

        // Helper function to check for OpenAL errors
        bool CheckALError(const std::string& message);
    };


}

#endif /* oal_audio_source_hpp */

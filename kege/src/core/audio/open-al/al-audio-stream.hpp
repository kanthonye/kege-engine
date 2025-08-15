//
//  audio-stream.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/10/25.
//

#ifndef open_al_audio_stream_hpp
#define open_al_audio_stream_hpp

#include "../core/audio-stream.hpp"

namespace kege::audio::al{

    class Stream : public audio::Stream{
    public:

        // Open an audio file for streaming
        bool open(const std::string& filename);

        // Check if the stream is playing
        bool isPlaying()const;

        // Update the stream (call this periodically)
        void update();

        // Start streaming the audio
        void play();

        // Stop streaming the audio
        void stop();

        ~Stream();
        Stream();

    private:

        uint32_t _buffers[2]; // Double buffering
        uint32_t _id; // OpenAL source and buffers

        // File handling
        SNDFILE* _snd_file;
        SF_INFO _sf_info;

        // Thread for streaming
        std::thread _stream_thread;
        std::atomic<bool> _is_streaming;

        // Helper functions
        bool fillBuffer( uint32_t buffer );
        void streamData();

        bool checkALError(const std::string& message);
    };

}

#endif /* open_al_audio_stream_hpp */

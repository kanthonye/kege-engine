//
//  audio-stream.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/10/25.
//

#ifndef audio_stream_hpp
#define audio_stream_hpp

#include <stdio.h>
#include <vector>
#include <thread>
#include <atomic>
#include <string>
//#include <sndfile.h>

#include "ref.hpp"
#include "vectors.hpp"
#include "audio-buffer.hpp"

namespace kege::audio{

    // for large audio file
    class Stream : public kege::RefCounter {
    public:

        // Open an audio file for streaming
        //virtual bool open(const std::string& filename) = 0;

        // Check if the stream is playing
        virtual bool isPlaying()const = 0;

        // Update the stream (call this periodically)
        virtual void update() = 0;

        // Start streaming the audio
        virtual void play() = 0;

        // Stop streaming the audio
        virtual void stop() = 0;

        virtual ~Stream(){}
    };

}

#endif /* audio_stream_hpp */

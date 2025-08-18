//
//  audio-stream-source.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/6/25.
//

#ifndef audio_stream_source_hpp
#define audio_stream_source_hpp

#include <vector>
#include <thread>
#include <atomic>
#include <string>
//#include <sndfile.h>

#include "audio-buffer.hpp"

namespace kege::audio{

//    // for large audio file
//    class SourceStreamer {
//    public:
//
//        bool open(const std::string& filename);
//        void setLooping(bool enable);
//        void play();
//        void stop();
//        void updateStream(); // Called in a loop to keep buffering data
//
//        SourceStreamer();
//        ~SourceStreamer();
//
//    private:
//        static constexpr int NUM_BUFFERS = 3;
//        static constexpr int BUFFER_SIZE = 4096;
//
//        ALuint _source;
//        ALuint _buffers[ NUM_BUFFERS ];
//        std::thread _stream_thread;
//        std::atomic<bool> _is_streaming;
//
//        SF_INFO _sf_info;      // Holds audio file information
//        SNDFILE* _file;       // Pointer to the opened sound file
//
//        ALenum _format;
//        ALsizei _sample_rate;
//
//        bool _loop;
//
//        bool refillBuffer(ALuint buffer);
//    };

}

#endif /* audio_stream_source_hpp */

//
//  audio-stream-source.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/6/25.
//

#include "audio-source-streamer.hpp"
#include <iostream>
#include <fstream>
#include <vector>

namespace kege::audio{

//    bool SourceStreamer::open(const std::string& filename)
//    {
//        _file = sf_open(filename.c_str(), SFM_READ, &_sf_info);
//        if (!_file) {
//            std::cerr << "Failed to open " << filename << std::endl;
//            return false;
//        }
//
//        _format = (_sf_info.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
//        _sample_rate = _sf_info.samplerate;
//
//        for (int i = 0; i < NUM_BUFFERS; i++) {
//            if (!refillBuffer(_buffers[i])) return false;
//        }
//
//        alSourceQueueBuffers(_source, NUM_BUFFERS, _buffers);
//        return true;
//    }
//
//    void SourceStreamer::setLooping(bool enable) {
//        _loop = enable;
//    }
//
//    void SourceStreamer::play()
//    {
//        if (!_is_streaming)
//        {
//            _is_streaming = true;
//            alSourcePlay(_source);
//            _stream_thread = std::thread(&SourceStreamer::updateStream, this);
//        }
//    }
//
//    void SourceStreamer::stop()
//    {
//        if (_is_streaming) {
//            _is_streaming = false;
//            _stream_thread.join();
//            alSourceStop(_source);
//        }
//    }
//
//    void SourceStreamer::updateStream()
//    {
//        while (_is_streaming)
//        {
//            ALint processed = 0;
//            alGetSourcei(_source, AL_BUFFERS_PROCESSED, &processed);
//
//            while (processed-- > 0)
//            {
//                ALuint buffer;
//                alSourceUnqueueBuffers(_source, 1, &buffer);
//                if ( refillBuffer(buffer) )
//                {
//                    alSourceQueueBuffers(_source, 1, &buffer);
//                }
//            }
//
//            // Ensure playback continues
//            ALint state;
//            alGetSourcei(_source, AL_SOURCE_STATE, &state);
//            if (state != AL_PLAYING)
//            {
//                alSourcePlay(_source);
//            }
//
//            std::this_thread::sleep_for(std::chrono::milliseconds(16));
//        }
//    }
//
//    bool SourceStreamer::refillBuffer(ALuint buffer)
//    {
//        std::vector<short> data(BUFFER_SIZE / sizeof(short));
//        sf_count_t samples_read = sf_read_short(_file, data.data(), data.size());
//
//        if (samples_read > 0)
//        {
//            alBufferData(buffer, _format, data.data(), samples_read * sizeof(short), _sample_rate);
//            return true;
//        }
//
//        // If we reach the end of the file
//        if ( _loop )
//        {
//            sf_seek(_file, 0, SEEK_SET); // Seek back to start
//            samples_read = sf_read_short(_file, data.data(), data.size());
//            if (samples_read > 0)
//            {
//                alBufferData(buffer, _format, data.data(), samples_read * sizeof(short), _sample_rate);
//                return true;
//            }
//        }
//
//        return false;
////        std::vector<short> data(BUFFER_SIZE / sizeof(short));
////        sf_count_t samples_read = sf_read_short(_file, data.data(), data.size());
////
////        if (samples_read > 0) {
////            alBufferData(buffer, _format, data.data(), samples_read * sizeof(short), _sample_rate);
////            return true;
////        }
////
////        return true;
//    }
//
//    SourceStreamer::SourceStreamer()
//    :   _is_streaming(false)
//    {
//        alGenSources(1, &_source);
//        alGenBuffers(NUM_BUFFERS, _buffers);
//    }
//
//    SourceStreamer::~SourceStreamer()
//    {
//        stop();
//        if (_file) sf_close(_file);
//        alDeleteSources(1, &_source);
//        alDeleteBuffers(NUM_BUFFERS, _buffers);
//    }
}

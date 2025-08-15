//
//  audio-stream.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/10/25.
//

#define OPENAL_DEPRECATED
#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#include "al-audio-stream.hpp"

namespace kege::audio::al{

    Stream::Stream()
    : _id(0)
    , _snd_file(nullptr)
    , _is_streaming(false)
    {
        alGenSources(1, &_id);
        alGenBuffers(2, _buffers);
        checkALError("Failed to generate OpenAL source/_buffers");
    }

    Stream::~Stream()
    {
        stop();
        if ( _snd_file )
        {
            sf_close(_snd_file);
        }
        alDeleteSources(1, &_id);
        alDeleteBuffers(2, _buffers);
    }

    bool Stream::open(const std::string& filepath )
    {
        _snd_file = sf_open(filepath.c_str(), SFM_READ, &_sf_info);
        if (!_snd_file)
        {
            std::cerr << "Failed to open audio file: " << filepath << std::endl;
            return false;
        }

        // Check if the format is supported
        if (_sf_info.channels != 1 && _sf_info.channels != 2)
        {
            std::cerr << "Unsupported number of channels: " << _sf_info.channels << std::endl;
            sf_close(_snd_file);
            return false;
        }

        std::cout << "Opened audio file: " << filepath << std::endl;
        return true;
    }

    void Stream::play()
    {
        if (!_snd_file || _is_streaming)
        {
            return;
        }

        // Fill the initial _buffers
        if ( fillBuffer( _buffers[0] ) && fillBuffer( _buffers[1] ) )
        {
            alSourceQueueBuffers( _id, 2, _buffers );
            alSourcePlay( _id );
            _is_streaming = true;
            _stream_thread = std::thread( &Stream::streamData, this );
        }
    }

    void Stream::stop()
    {
        if (_is_streaming)
        {
            _is_streaming = false;
            if (_stream_thread.joinable())
            {
                _stream_thread.join();
            }
            alSourceStop(_id);
            alSourceUnqueueBuffers(_id, 2, _buffers);
        }
    }

    void Stream::update()
    {
        if ( !_is_streaming )
        {
            return;
        }

        // Check for processed _buffers
        ALint processed_buffers;
        alGetSourcei(_id, AL_BUFFERS_PROCESSED, &processed_buffers);

        while ( processed_buffers-- )
        {
            ALuint buffer;
            alSourceUnqueueBuffers(_id, 1, &buffer);
            if ( fillBuffer(buffer) )
            {
                alSourceQueueBuffers(_id, 1, &buffer);
            }
        }
    }

    bool Stream::isPlaying() const
    {
        ALint state;
        alGetSourcei(_id, AL_SOURCE_STATE, &state);
        return state == AL_PLAYING;
    }

    bool Stream::fillBuffer(ALuint buffer)
    {
        const int bufferSize = 4096; // Number of samples per chunk
        std::vector<short> pcmData(bufferSize * _sf_info.channels);

        // Read audio data from the file
        sf_count_t frames_read = sf_readf_short(_snd_file, pcmData.data(), bufferSize);
        if (frames_read <= 0)
        {
            return false; // End of file
        }

        // Determine the OpenAL format
        ALenum format = (_sf_info.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

        // Upload the data to the OpenAL buffer
        uint32_t size = uint32_t(frames_read * _sf_info.channels * sizeof(short));
        alBufferData(buffer, format, pcmData.data(), size, _sf_info.samplerate);
        return checkALError("Failed to fill audio buffer");
    }

    void Stream::streamData()
    {
        while ( _is_streaming )
        {
            update();
            std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) ); // Adjust as needed
        }
    }

    bool Stream::checkALError(const std::string& message)
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

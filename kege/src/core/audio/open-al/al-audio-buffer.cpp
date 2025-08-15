//
//  audio-buffer.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/6/25.
//

#define OPENAL_DEPRECATED
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <iostream>
#include <fstream>

#include "al-audio-buffer.hpp"

namespace kege::audio::al{

    // WAV Header Struct
    struct WAVHeader
    {
        char riff[4];       // "RIFF"
        uint32_t file_size;
        char wave[4];       // "WAVE"
        char fmt[4];        // "fmt "
        uint32_t fmt_size;
        uint16_t audio_format;
        uint16_t num_channels;
        uint32_t sample_rate;
        uint32_t byte_rate;
        uint16_t block_align;
        uint16_t bits_per_sample;
        char data[4];       // "data"
        uint32_t data_size;
    };

    bool Buffer::loadWAV(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::binary);
        if (!file)
        {
            std::cerr << "Failed to open WAV file: " << filename << std::endl;
            return false;
        }

        WAVHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));
        if (std::string(header.riff, 4) != "RIFF" || std::string(header.wave, 4) != "WAVE")
        {
            std::cerr << "Invalid WAV file format!" << std::endl;
            return false;
        }

        std::vector<char> audio_data( header.data_size );
        file.read( audio_data.data(), header.data_size );

        ALenum format = (header.num_channels == 1) ?
                        (header.bits_per_sample == 8 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16) :
                        (header.bits_per_sample == 8 ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16);

        audio_format = header.audio_format;
        channels = header.num_channels;
        sample_rate = header.sample_rate;
        byte_rate = header.byte_rate;
        bits_per_sample = header.bits_per_sample;

        alGenBuffers(1, &_id);
        alBufferData( _id, format, audio_data.data(), (uint32_t)audio_data.size(), header.sample_rate );
        return _id != 0;
    }

    uint32_t Buffer::getID() const
    {
        return _id;
    }

    Buffer::Buffer()
    :   _id( 0 )
    {
    }

    Buffer::~Buffer()
    {
        if ( _id != 0 )
        {
            alDeleteBuffers( 1, &_id );
            _id = 0;
        }
    }
    
}

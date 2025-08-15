//
//  audio-buffer.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/6/25.
//

#include "audio-buffer.hpp"
#include <iostream>
#include <fstream>

//namespace kege::audio{
//
//    // WAV Header Struct
//    struct WAVHeader
//    {
//        char riff[4];       // "RIFF"
//        uint32_t fileSize;
//        char wave[4];       // "WAVE"
//        char fmt[4];        // "fmt "
//        uint32_t fmtSize;
//        uint16_t audioFormat;
//        uint16_t numChannels;
//        uint32_t sampleRate;
//        uint32_t byteRate;
//        uint16_t blockAlign;
//        uint16_t bitsPerSample;
//        char data[4];       // "data"
//        uint32_t dataSize;
//    };
//
//    audio::Buffer::Buffer() {
//        alGenBuffers(1, &_buffer);
//    }
//
//    audio::Buffer::~Buffer() {
//        alDeleteBuffers(1, &_buffer);
//    }
//
//    bool audio::Buffer::loadWAV(const std::string& filename)
//    {
//        std::ifstream file(filename, std::ios::binary);
//        if (!file) {
//            std::cerr << "Failed to open WAV file: " << filename << std::endl;
//            return false;
//        }
//
//        WAVHeader header;
//        file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));
//
//        if (std::string(header.riff, 4) != "RIFF" || std::string(header.wave, 4) != "WAVE") {
//            std::cerr << "Invalid WAV file format!" << std::endl;
//            return false;
//        }
//
//        std::vector<char> audioData(header.dataSize);
//        file.read(audioData.data(), header.dataSize);
//
//        ALenum format = (header.numChannels == 1) ?
//                        (header.bitsPerSample == 8 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16) :
//                        (header.bitsPerSample == 8 ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16);
//
//        alBufferData(_buffer, format, audioData.data(), audioData.size(), header.sampleRate);
//        return true;
//    }
//}

//
//  audio-buffer.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/6/25.
//

#ifndef open_al_audio_buffer_hpp
#define open_al_audio_buffer_hpp

#include "../core/audio-buffer.hpp"

namespace kege::audio::al{

    class Buffer : public audio::Buffer{
    public:

        bool loadWAV( const std::string& filename );
        uint32_t getID() const;

        ~Buffer();
        Buffer();

    private:

        uint32_t _id;

        uint16_t audio_format;
        uint16_t channels;
        uint32_t sample_rate;
        uint32_t byte_rate;
        uint16_t bits_per_sample;
    };
}

#endif /* open_al_audio_buffer_hpp */

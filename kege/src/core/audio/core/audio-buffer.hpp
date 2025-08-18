//
//  audio-buffer.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/6/25.
//

#ifndef audio_buffer_hpp
#define audio_buffer_hpp

#include <stdio.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <iostream>

#include "../../memory/ref.hpp"
#include "../../math/algebra/vmath.hpp"

namespace kege::audio{

    class Buffer : public kege::RefCounter {
    public:

        virtual uint32_t getID() const = 0;
        virtual ~Buffer(){}
    };
}

#endif /* audio_buffer_hpp */

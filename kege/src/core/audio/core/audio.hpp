//
//  Audio.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/6/25.
//

#ifndef kege_audio_hpp
#define kege_audio_hpp

#include "audio-buffer.hpp"
#include "audio-stream.hpp"
#include "audio-source.hpp"
#include "audio-listener.hpp"

namespace kege{

    typedef Ref< audio::Buffer > AudioBuffer;
    typedef Ref< audio::Source > AudioSource;
    typedef Ref< audio::Stream > AudioStream;
    
}

namespace kege::audio{

    /**
     * This class will serve as the central hub for managing audio resources,
     * initializing the audio system, and providing methods to create and manage
     * audio sources, buffers, and listeners.
     */
    class Device : public kege::RefCounter{
    public:

        virtual Ref< audio::Stream > createStream(const std::string& filePath) = 0;
        virtual Ref< audio::Buffer > createBuffer(const std::string& filePath) = 0;
        virtual Ref< audio::Source > createSource() = 0;

        virtual audio::Listener* getListener() = 0;

        virtual bool initialize() = 0;
        virtual void shutdown() = 0;
        virtual ~Device(){}
    };
}
#endif /* kege_audio_hpp */

//
//  al-audio.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/10/25.
//

#ifndef al_audio_hpp
#define al_audio_hpp
#define OPENAL_DEPRECATED
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <iostream>

#include "../core/audio.hpp"
#include "al-audio-buffer.hpp"
#include "al-audio-effect.hpp"
#include "al-audio-stream.hpp"
#include "al-audio-source.hpp"
#include "al-audio-listener.hpp"

namespace kege::audio::al{

    /**
     * This class will serve as the central hub for managing audio resources,
     * initializing the audio system, and providing methods to create and manage
     * audio sources, buffers, and listeners.
     */
    class Device : public audio::Device {
    public:

        Ref< audio::Stream > createStream(const std::string& filePath);
        Ref< audio::Buffer > createBuffer(const std::string& filePath);
        Ref< audio::Source > createSource();

        Ref< audio::Effect > createEffectDistortion
        (
            float edge,
            float gain,
            float lowpassCutoff,
            float eqCenter,
            float eqBandwidth
        );

        Ref< audio::Effect > createEffectFlanger
        (
            float rate,
            float depth,
            float feedback,
            float delay
        );

        Ref< audio::Effect > createEffectReverb
        (
            float density,
            float diffusion,
            float gain,
            float decayTime,
            float decayHFRatio
        );

        Ref< audio::Effect > createEffectEcho
        (
            float delay,
            float lrDelay,
            float damping,
            float feedback,
            float spread
        );

        Ref< audio::Effect > createEffectChorus
        (
            float rate,
            float depth,
            float feedback,
            float delay
        );




        int getAuxEffectSlot();

        al::Listener* getListener();

        void setVolume(float volume);

        bool initialize();
        void shutdown();

        Device();
        virtual ~Device();

    private:

        bool checkEFXError(const std::string& message);
        bool checkALError(const std::string& message);


        ALCdevice* _device;
        ALCcontext* _context;
        uint32_t _aux_effect_slot;
        al::Listener _listener;
    };
}

namespace kege::audio::al{

    // Effect types
    #define AL_EFFECT_TYPE 0x8001
    #define AL_EFFECT_REVERB 0x0001
    #define AL_EFFECT_CHORUS 0x0002
    #define AL_EFFECT_DISTORTION 0x0003
    #define AL_EFFECT_ECHO 0x0004
    #define AL_EFFECT_FLANGER 0x0005

    // Filter types
    #define AL_FILTER_TYPE 0x8001
    #define AL_FILTER_NULL 0x0000
    #define AL_FILTER_LOWPASS 0x0001
    #define AL_FILTER_HIGHPASS 0x0002
    #define AL_FILTER_BANDPASS 0x0003

    // Auxiliary effect slot
    #define AL_EFFECTSLOT_NULL 0x0000
    #define AL_EFFECTSLOT_EFFECT 0x0001
    #define AL_AUXILIARY_SEND_FILTER 0x0002




    // Reverb parameters
    #define AL_REVERB_DENSITY 0x0001
    #define AL_REVERB_DIFFUSION 0x0002
    #define AL_REVERB_GAIN 0x0003
    #define AL_REVERB_GAINHF 0x0004
    #define AL_REVERB_DECAY_TIME 0x0005
    #define AL_REVERB_DECAY_HFRATIO 0x0006

    // Echo parameters
    #define AL_ECHO_DELAY 0x0001
    #define AL_ECHO_LRDELAY 0x0002
    #define AL_ECHO_DAMPING 0x0003
    #define AL_ECHO_FEEDBACK 0x0004
    #define AL_ECHO_SPREAD 0x0005

    // Chorus parameters
    #define AL_CHORUS_WAVEFORM 0x0001
    #define AL_CHORUS_PHASE 0x0002
    #define AL_CHORUS_RATE 0x0003
    #define AL_CHORUS_DEPTH 0x0004
    #define AL_CHORUS_FEEDBACK 0x0005
    #define AL_CHORUS_DELAY 0x0006

    // Distortion parameters
    #define AL_DISTORTION_EDGE 0x0001
    #define AL_DISTORTION_GAIN 0x0002
    #define AL_DISTORTION_LOWPASS_CUTOFF 0x0003
    #define AL_DISTORTION_EQCENTER 0x0004
    #define AL_DISTORTION_EQBANDWIDTH 0x0005

    // Flanger parameters
    #define AL_FLANGER_WAVEFORM 0x0001
    #define AL_FLANGER_PHASE 0x0002
    #define AL_FLANGER_RATE 0x0003
    #define AL_FLANGER_DEPTH 0x0004
    #define AL_FLANGER_FEEDBACK 0x0005
    #define AL_FLANGER_DELAY 0x0006
}



#endif /* al_audio_hpp */

//
//  al-audio.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/10/25.
//

//#include <sndfile.h>
#include "al-audio.hpp"

namespace kege::audio::al{

    Ref< audio::Stream > Device::createStream(const std::string& filepath )
    {
        Ref< al::Stream > stream = new al::Stream;
        if ( !stream->open( filepath ) )
        {
            stream.clear();
            return nullptr;
        }
        return stream.ref();
    }

    Ref< audio::Buffer > Device::createBuffer( const std::string& filepath )
    {
        Ref< al::Buffer > buffer = new al::Buffer;
        if ( !buffer->loadWAV( filepath ) )
        {
            buffer.clear();
            return nullptr;
        }
        return buffer.ref();
    }

    void Device::setVolume(float volume)
    {
        alListenerf(AL_GAIN, volume);
        //CheckALError("Failed to set master volume");
    }

    Ref< audio::Source > Device::createSource()
    {
        return new al::Source;
    }

    Ref< audio::Effect > Device::createEffectDistortion
    (
        float edge,
        float gain,
        float lowpass_cutoff,
        float eq_center,
        float eq_bandwidth
    )
    {
        Ref< al::Effect > effect = new al::Effect( AL_EFFECT_DISTORTION, this );
        if ( effect->getID() != 0 )
        {
            effect.clear();
            return nullptr;
        }
        effect->setDistortionParameters( edge, gain, lowpass_cutoff, eq_center, eq_bandwidth );
        return effect.ref();
    }

    Ref< audio::Effect > Device::createEffectFlanger
    (
        float rate,
        float depth,
        float feedback,
        float delay
    )
    {
        Ref< al::Effect > effect = new al::Effect( AL_EFFECT_FLANGER, this );
        if ( effect->getID() != 0 )
        {
            effect.clear();
            return nullptr;
        }
        effect->setFlangerParameters( rate, depth, feedback, delay );
        return effect.ref();
    }

    Ref< audio::Effect > Device::createEffectReverb
    (
        float density,
        float diffusion,
        float gain,
        float decay_time,
        float decay_hf_ratio
    )
    {
        Ref< al::Effect > effect = new al::Effect( AL_EFFECT_FLANGER, this );
        if ( effect->getID() != 0 )
        {
            effect.clear();
            return nullptr;
        }
        effect->setReverbParameters( density, diffusion, gain, decay_time, decay_hf_ratio );
        return effect.ref();
    }

    Ref< audio::Effect > Device::createEffectEcho
    (
        float delay,
        float lr_delay,
        float damping,
        float feedback,
        float spread
    )
    {
        Ref< al::Effect > effect = new al::Effect( AL_EFFECT_FLANGER, this );
        if ( effect->getID() != 0 )
        {
            effect.clear();
            return nullptr;
        }
        effect->setEchoParameters( delay, lr_delay, damping, feedback, spread );
        return effect.ref();
    }

    Ref< audio::Effect > Device::createEffectChorus
    (
        float rate,
        float depth,
        float feedback,
        float delay
    )
    {
        Ref< al::Effect > effect = new al::Effect( AL_EFFECT_FLANGER, this );
        if ( effect->getID() != 0 )
        {
            effect.clear();
            return nullptr;
        }
        effect->setChorusParameters( rate, depth, feedback, delay );
        return effect.ref();
    }

    int Device::getAuxEffectSlot()
    {
        return _aux_effect_slot;
    }

    al::Listener* Device::getListener()
    {
        return &_listener;
    }

    bool Device::initialize()
    {
        // Open default audio device
        _device = alcOpenDevice(nullptr);
        if ( !_device )
        {
            std::cerr << "Failed to open OpenAL device!" << std::endl;
            return false;
        }

        // Create and activate context
        _context = alcCreateContext(_device, nullptr);
        if (!_context)
        {
            std::cerr << "Failed to create OpenAL context!" << std::endl;
            alcCloseDevice(_device);
            return false;
        }

        if (!alcMakeContextCurrent( _context ))
        {
            std::cerr << "Failed to make audio context current!" << std::endl;
            alcDestroyContext( _context );
            alcCloseDevice( _device );
            return false;
        }

        // Initialize EFX
        if ( alcIsExtensionPresent( _device, "ALC_EXT_EFX" ) )
        {
            std::cout << "EFX supported!" << std::endl;
            Effect::loadEFXFunctions();
            Effect::alGenAuxiliaryEffectSlots( 1, &_aux_effect_slot );
            if ( !checkEFXError("Failed to create auxiliary effect slot") )
            {
                std::cerr << "Failed to initialize EFX!" << std::endl;
                return false;
            }
        }
        else
        {
            std::cerr << "EFX not supported!" << std::endl;
        }


        alcMakeContextCurrent(_context);
        return true;
    }

    void Device::shutdown()
    {
        // Delete auxiliary effect slot
        if ( _aux_effect_slot )
        {
            Effect::alDeleteAuxiliaryEffectSlots( 1, &_aux_effect_slot );
            _aux_effect_slot = 0;
        }

        if ( _context )
        {
            alcMakeContextCurrent(nullptr);
            alcDestroyContext( _context );
            _context = nullptr;
        }
        if ( _device )
        {
            alcCloseDevice( _device );
            _device = nullptr;
        }
    }

    Device::Device()
    :   _device( nullptr )
    ,   _context( nullptr )
    {
    }

    Device::~Device()
    {
        al::Device::shutdown();
    }

//    void Device::printAudioInfo() {
//        const ALCchar* deviceName = alcGetString(_device, ALC_DEVICE_SPECIFIER);
//        std::cout << "OpenAL Device: " << (deviceName ? deviceName : "Unknown") << std::endl;
//    }

    bool Device::checkALError(const std::string& message)
    {
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            std::cerr << message << " (OpenAL Error: " << error << ")" << std::endl;
            return false;
        }
        return true;
    }

    bool Device::checkEFXError(const std::string& message)
    {
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            std::cerr << message << " (EFX Error: " << error << ")" << std::endl;
            return false;
        }
        return true;
    }
}

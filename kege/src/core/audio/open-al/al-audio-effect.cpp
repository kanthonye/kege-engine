//
//  al-audio-effect.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/10/25.
//

#define OPENAL_DEPRECATED
#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#include "al-audio.hpp"
#include "al-audio-effect.hpp"


namespace kege::audio::al{

    void Effect::setReverbParameters(float density, float diffusion, float gain, float decayTime, float decayHFRatio)
    {
        if (!_id)
        {
            std::cerr << "Effect not created!" << std::endl;
            return;
        }

        alEffectf(_id, AL_REVERB_DENSITY, density);
        alEffectf(_id, AL_REVERB_DIFFUSION, diffusion);
        alEffectf(_id, AL_REVERB_GAIN, gain);
        alEffectf(_id, AL_REVERB_DECAY_TIME, decayTime);
        alEffectf(_id, AL_REVERB_DECAY_HFRATIO, decayHFRatio);
        checkEFXError("Failed to set reverb parameters");
    }

    void Effect::setEchoParameters(float delay, float lrDelay, float damping, float feedback, float spread)
    {
        if (!_id)
        {
            std::cerr << "Effect not created!" << std::endl;
            return;
        }

        alEffectf(_id, AL_ECHO_DELAY, delay);
        alEffectf(_id, AL_ECHO_LRDELAY, lrDelay);
        alEffectf(_id, AL_ECHO_DAMPING, damping);
        alEffectf(_id, AL_ECHO_FEEDBACK, feedback);
        alEffectf(_id, AL_ECHO_SPREAD, spread);
        checkEFXError("Failed to set echo parameters");
    }

    void Effect::setChorusParameters(float rate, float depth, float feedback, float delay)
    {
        if (!_id)
        {
            std::cerr << "Effect not created!" << std::endl;
            return;
        }

        alEffectf(_id, AL_CHORUS_RATE, rate);
        alEffectf(_id, AL_CHORUS_DEPTH, depth);
        alEffectf(_id, AL_CHORUS_FEEDBACK, feedback);
        alEffectf(_id, AL_CHORUS_DELAY, delay);
        checkEFXError("Failed to set chorus parameters");
    }

    void Effect::setDistortionParameters(float edge, float gain, float lowpassCutoff, float eqCenter, float eqBandwidth)
    {
        if (!_id)
        {
            std::cerr << "Effect not created!" << std::endl;
            return;
        }

        alEffectf(_id, AL_DISTORTION_EDGE, edge);
        alEffectf(_id, AL_DISTORTION_GAIN, gain);
        alEffectf(_id, AL_DISTORTION_LOWPASS_CUTOFF, lowpassCutoff);
        alEffectf(_id, AL_DISTORTION_EQCENTER, eqCenter);
        alEffectf(_id, AL_DISTORTION_EQBANDWIDTH, eqBandwidth);
        checkEFXError("Failed to set distortion parameters");
    }

    void Effect::setFlangerParameters(float rate, float depth, float feedback, float delay)
    {
        if (!_id)
        {
            std::cerr << "Effect not created!" << std::endl;
            return;
        }

        alEffectf(_id, AL_FLANGER_RATE, rate);
        alEffectf(_id, AL_FLANGER_DEPTH, depth);
        alEffectf(_id, AL_FLANGER_FEEDBACK, feedback);
        alEffectf(_id, AL_FLANGER_DELAY, delay);
        checkEFXError("Failed to set flanger parameters");
    }

    ALuint Effect::getEffectID() const
    {
        return _id;
    }

    void Effect::applyToSource( ALuint sourceID )
    {
        if (!_id)
        {
            std::cerr << "Effect not created!" << std::endl;
            return;
        }
        alSource3i(sourceID, AL_AUXILIARY_SEND_FILTER, _device->getAuxEffectSlot(), 0, AL_FILTER_NULL);
        checkEFXError("Failed to apply effect to source");
    }

    bool Effect::checkEFXError(const std::string& message)
    {
        ALenum error = alGetError();
        if (error != AL_NO_ERROR) {
            std::cerr << message << " (EFX Error: " << error << ")" << std::endl;
            return false;
        }
        return true;
    }

    uint32_t Effect::getID() const
    {
        return _id;
    }

    Effect::Effect( uint32_t effect_type, al::Device* device )
    :   _id( 0 )
    ,   _device( device )
    {
        alGenEffects( 1, &_id );
        if ( !checkEFXError("Failed to generate effect") )
        {
            alEffecti( _id, AL_EFFECT_TYPE, effect_type );
            if ( checkEFXError("Failed to set effect type") )
            {
                std::cout << "Effect created successfully!" << std::endl;
                _id = 0;
            }
        }
    }

    Effect::~Effect()
    {
        if ( _id )
        {
            alDeleteEffects( 1, &_id );
            _id = 0;
        }
    }

}




namespace kege::audio::al{

    Effect::LPALGENEFFECTS Effect::alGenEffects = nullptr;
    Effect::LPALDELETEEFFECTS Effect::alDeleteEffects = nullptr;
    Effect::LPALISEFFECT Effect::alIsEffect = nullptr;
    Effect::LPALEFFECTI Effect::alEffecti = nullptr;
    Effect::LPALEFFECTF Effect::alEffectf = nullptr;
    Effect::LPALGENAUXILIARYEFFECTSLOTS Effect::alGenAuxiliaryEffectSlots = nullptr;
    Effect::LPALDELETEAUXILIARYEFFECTSLOTS Effect::alDeleteAuxiliaryEffectSlots = nullptr;
    Effect::LPALAUXILIARYEFFECTSLOTI Effect::alAuxiliaryEffectSloti = nullptr;

    // Function to load EFX functions
    bool Effect::loadEFXFunctions()
    {
        alGenEffects = (LPALGENEFFECTS)alGetProcAddress("alGenEffects");
        alDeleteEffects = (LPALDELETEEFFECTS)alGetProcAddress("alDeleteEffects");
        alIsEffect = (LPALISEFFECT)alGetProcAddress("alIsEffect");
        alEffecti = (LPALEFFECTI)alGetProcAddress("alEffecti");
        alEffectf = (LPALEFFECTF)alGetProcAddress("alEffectf");
        alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)alGetProcAddress("alGenAuxiliaryEffectSlots");
        alDeleteAuxiliaryEffectSlots = (LPALDELETEAUXILIARYEFFECTSLOTS)alGetProcAddress("alDeleteAuxiliaryEffectSlots");
        alAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI)alGetProcAddress("alAuxiliaryEffectSloti");

        if (!alGenEffects || !alDeleteEffects || !alIsEffect || !alEffecti || !alEffectf ||
            !alGenAuxiliaryEffectSlots || !alDeleteAuxiliaryEffectSlots || !alAuxiliaryEffectSloti) {
            std::cerr << "Failed to load EFX functions!" << std::endl;
            return false;
        }

        return true;
    }
}


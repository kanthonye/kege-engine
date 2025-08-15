//
//  al-audio-effect.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/10/25.
//

#ifndef al_audio_effect_hpp
#define al_audio_effect_hpp

#include "../core/audio-effect.hpp"

namespace kege::audio::al{

    class Device;
    
    class Effect : public audio::Effect{
    public:

        void setReverbParameters(float density, float diffusion, float gain, float decayTime, float decayHFRatio);
        void setEchoParameters(float delay, float lrDelay, float damping, float feedback, float spread);
        void setChorusParameters(float rate, float depth, float feedback, float delay);
        void setDistortionParameters(float edge, float gain, float lowpassCutoff, float eqCenter, float eqBandwidth);
        void setFlangerParameters(float rate, float depth, float feedback, float delay);

        // Apply the effect to a source
        void applyToSource( uint32_t source_id );

        // Get the effect ID
        ALuint getEffectID() const;

        uint32_t getID() const;

        // Create an effect of the specified type
        Effect( uint32_t effect_type, al::Device* device );
        ~Effect();

    private:

        // Function pointers for EFX functions
        typedef void (AL_APIENTRY* LPALGENEFFECTS)(ALsizei, ALuint*);
        typedef void (AL_APIENTRY* LPALDELETEEFFECTS)(ALsizei, const ALuint*);
        typedef void (AL_APIENTRY* LPALISEFFECT)(ALuint);
        typedef void (AL_APIENTRY* LPALEFFECTI)(ALuint, ALenum, ALint);
        typedef void (AL_APIENTRY* LPALEFFECTF)(ALuint, ALenum, ALfloat);
        typedef void (AL_APIENTRY* LPALGENAUXILIARYEFFECTSLOTS)(ALsizei, ALuint*);
        typedef void (AL_APIENTRY* LPALDELETEAUXILIARYEFFECTSLOTS)(ALsizei, const ALuint*);
        typedef void (AL_APIENTRY* LPALAUXILIARYEFFECTSLOTI)(ALuint, ALenum, ALint);

        // Global function pointers
        static LPALGENEFFECTS alGenEffects;
        static LPALDELETEEFFECTS alDeleteEffects;
        static LPALISEFFECT alIsEffect;
        static LPALEFFECTI alEffecti;
        static LPALEFFECTF alEffectf;
        static LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
        static LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
        static LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;

        // Function to load EFX functions
        static bool loadEFXFunctions();


        // Helper function to check for EFX errors
        bool checkEFXError(const std::string& message);

    private:

        uint32_t _id;

        uint16_t audio_format;
        uint16_t channels;
        uint32_t sample_rate;
        uint32_t byte_rate;
        uint16_t bits_per_sample;

        al::Device* _device;
        friend Device;
    };
}

#endif /* al_audio_effect_hpp */

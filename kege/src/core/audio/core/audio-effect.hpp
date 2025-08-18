//
//  audio-effect.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/10/25.
//

#ifndef audio_effect_hpp
#define audio_effect_hpp

#include "../../memory/ref.hpp"
#include "../../math/algebra/vmath.hpp"
#include "audio-buffer.hpp"

namespace kege::audio{

    class Effect : public kege::RefCounter {
    public:

//        // Create an effect of the specified type
//        bool Create(ALenum effectType);
//
//        // Apply the effect to a source
//        void ApplyToSource(ALuint sourceID, ALuint auxEffectSlot);
//
//        // Set reverb parameters
//        void SetReverbParameters(float density, float diffusion, float gain, float decayTime, float decayHFRatio);
//
//        // Get the effect ID
//        ALuint GetEffectID() const;

        virtual ~Effect(){}
    };

}
#endif /* audio_effect_hpp */

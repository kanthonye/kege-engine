//
//  al-audio-listener.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/10/25.
//

#ifndef audio_listener_hpp
#define audio_listener_hpp

#include "../../math/algebra/vmath.hpp"

namespace kege::audio{

    class Device;

    class Listener{
    public:

        virtual void setOrientation(const kege::vec3& forward, const kege::vec3& up) = 0;
        virtual void setPosition(const kege::vec3& pos) = 0;
        virtual void setVelocity(const kege::vec3& vel) = 0;

        virtual const kege::vec3& getVelocity()const = 0;
        virtual const kege::vec3& getPosition()const = 0;
        virtual const kege::vec3& getForward()const = 0;
        virtual const kege::vec3& getUp()const = 0;

        virtual ~Listener();
    };
}
#endif /* audio_listener_hpp */

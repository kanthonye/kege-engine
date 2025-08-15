//
//  al-audio-listener.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/10/25.
//

#ifndef al_audio_listener_hpp
#define al_audio_listener_hpp

#include "../core/audio-listener.hpp"

namespace kege::audio::al{

    class Device;

    class Listener : public audio::Listener{
    public:

        void setOrientation(const kege::vec3& forward, const kege::vec3& up);
        void setPosition(const kege::vec3& pos);
        void setVelocity(const kege::vec3& vel);

        const kege::vec3& getVelocity()const;
        const kege::vec3& getPosition()const;
        const kege::vec3& getForward()const;
        const kege::vec3& getUp()const;

    private:

        virtual ~Listener();
        Listener();

    private:

        kege::vec3 _position;
        kege::vec3 _velocity;
        kege::vec3 _forward;
        kege::vec3 _up;
        friend Device;
    };
}
#endif /* al_audio_listener_hpp */

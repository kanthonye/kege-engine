//
//  audio-listener.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/10/25.
//

#include "al-audio-listener.hpp"

namespace kege::audio{
//
//    void Listener::setOrientation(const kege::vec3& forward, const kege::vec3& up)
//    {
//        float orientation[] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
//        alListenerfv(AL_ORIENTATION, orientation);
//        _forward = forward;
//        _up = up;
//    }
//    
//    void Listener::setPosition(const kege::vec3& pos)
//    {
//        alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
//        _position = pos;
//    }
//
//    void Listener::setVelocity(const kege::vec3& vel)
//    {
//        alListener3f(AL_VELOCITY, vel.x, vel.y, vel.z);
//        _velocity = vel;
//    }
//
//    const kege::vec3& Listener::getVelocity()const
//    {
//        return _velocity;
//    }
//
//    const kege::vec3& Listener::getPosition()const
//    {
//        return _position;
//    }
//
//    const kege::vec3& Listener::getForward()const
//    {
//        return _forward;
//    }
//
//    const kege::vec3& Listener::getUp()const
//    {
//        return _up;
//    }
//
//    Listener::Listener()
//    :   _position{ 0.0f, 0.0f, 0.0f }
//    ,   _velocity{ 0.0f, 0.0f, 0.0f }
//    ,   _forward{ 0.0f, 0.0f, -1.0f }
//    ,   _up{ 0.0f, 1.0f, 0.0f }
//    {
//        setPosition( _position );
//        setVelocity( _velocity );
//        setOrientation( _forward, _up );
//    }
//
    Listener::~Listener()
    {
    }

}

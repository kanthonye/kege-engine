//
//  sleep-system.cpp
//  game
//
//  Created by Kenneth Esdaile on 3/20/25.
//

#include "sleep-system.hpp"

// namespace kege{

//     void SleepSystem::operator()( const WakeSleepableObjects& )
//     {
//         _wake_sleepable_objects = true;
//     }

//     void SleepSystem::update( double dms )
//     {
//         if ( !_entities ) return;

//         kege::Rigidbody* body;
//         for ( kege::Entity& entity : *_entities )
//         {
//             body = entity.get< kege::Rigidbody >();
//             if ( body->immovable )
//             {
//                 continue;
//             }

//             zeroSmallComponents( body->angular.velocity );
//             zeroSmallComponents( body->linear.velocity );
//             if ( body->sleepable )
//             {
//                 if ( _wake_sleepable_objects )
//                 {
//                     body->frames_since_move = 0;
//                     body->is_awake = true;
//                 }
                
//                 // Calculate body as a combination of linear and angular velocities
//                 float current_motion = magnSq( body->linear.velocity ) + magnSq( body->angular.velocity );

//                 // Use a bias to smoothly transition the body value
//                 body->body = _sleep_motion_bias * body->body + (1.0f - _sleep_motion_bias) * current_motion;

//                 //float mag = magn( body->linear.velocity );
//                 // Check if the body should be put to sleep // Only sleep after inactivity for N frames
//                 if ( body->body < _sleep_threshold && body->frames_since_move > 16 )
//                 //if ( mag < 1.76 && body->frames_since_move > 16 )
//                 {
//                     std::cout << "object->setAwake( false )\n";
//                     //object->setAwake( false );
//                     body->is_awake = false;
//                     body->linear.velocity = {0.f,0.f,0.f};
//                     body->angular.velocity = {0.f,0.f,0.f};
//                 }
//                 else
//                 {
//                     body->frames_since_move++;
//                 }
//             }
//         }
//         _wake_sleepable_objects = false;
//     }

//     bool SleepSystem::initialize()
//     {
//         _signature = kege::createEntitySignature< kege::Rigidbody, kege::Transform >();
//         _comm.add< const WakeSleepableObjects&, SleepSystem>( this );
//         return EntitySystem::initialize();
//     }

//     void SleepSystem::shutdown()
//     {
//         _comm.remove< const WakeSleepableObjects&, SleepSystem>( this );
//         return EntitySystem::shutdown();
//     }

//     SleepSystem::SleepSystem( kege::Engine* engine )
//     :   kege::EntitySystem( manager, "sleep-system", REQUIRE_UPDATE )
//     ,   _sleep_threshold( 0.0005 )
//     ,   _sleep_motion_bias( 0.98f )
//     ,   _wake_sleepable_objects( false )
//     {}

// }

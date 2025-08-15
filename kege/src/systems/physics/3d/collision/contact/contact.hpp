//
//  collision-manager.hpp
//  kege
//
//  Created by Kenneth Esdaile on 10/27/24.
//

#ifndef collision_manager_hpp
#define collision_manager_hpp

#include "vectors.hpp"
#include "collider.hpp"
#include "rigidbody.hpp"

namespace kege
{
    class Rigidbody;
    class CollisionRegistry;

    enum{ MAX_CONTACTS = 8 };

    struct Contact
    {
        /**
         * The contact point
         */
        vec3  point;
        
        /**
         * The contact penetration depth
         */
        float depth;


        /**
         * The rest of the member are cached, this save cpu us from re-computing it every solver that requires this info
         */

        /**
         * The contact normal (pointing from first to second object)
         */
        kege::vec3 normal;

        /**
         * The combined restitution coefficient at this point
         */
        float restitution;

        /**
         * The combined friction coefficient at this point
         */
        float friction;

        /**
         * The combined inverse mass
         */
        float inv_mass_sum;


        /**
         * The computed impulse calculated for the this contact
         */
        float impulse;

        /**
         * The computed dot product between the relative velocity and the contact normal
         */
        float v_dot_n;

        /**
         * The computed relative velocity
         */
        vec3  relative_velocity;

        /**
         * The computed relative positions
         */
        vec3  relative_position[2];
    };

    struct CollisionManifold
    {
        /**
         * The CollisionManifold contacts
         */
        Contact contacts[ MAX_CONTACTS ];

        /**
         * Track the contact count
         */
        uint32_t contact_count = 0;

        /**
         * The rigidbodys in collision
         */
        Rigidbody* objects[ 2 ];

        /**
         * The contact normal
         */
        vec3 normal;
    };


    class CollisionRegistry
    {
    public:

        CollisionManifold* operator[](uint32_t i);
        void resize(uint32_t size);
        void reset();

        CollisionManifold* generate();
        uint32_t count()const;

        CollisionRegistry();

    private:

        std::vector< CollisionManifold > _collisions;
        uint32_t _count;
    };

}
#endif /* collision_manager_hpp */

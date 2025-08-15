//
//  cloth.cpp
//  kege
//
//  Created by Kenneth Esdaile on 10/26/24.
//

#include "vectors.hpp"
#include "particle.hpp"
#include "cloth.hpp"
namespace kege{

    class Spring{
    public:

        void applyForce(double dms);
        
        Spring(float k, float b, float len)
        :   resisting_length(len), k(k), b(b)
        {}

        Particle* partcles[2];
        float resisting_length;
        float k; // [-n to 0] : higher k = stiff sprint, lower k = loose spring
        float b; // [0 to 1], default to 0
    };

    void Spring::applyForce(double dms)
    {
        vec3 relitive_position = partcles[1]->position - partcles[0]->position;
        vec3 relitive_velocity = partcles[1]->velocity - partcles[0]->velocity;
        float x = magn( relitive_position ) - resisting_length;
        float v = magn( relitive_velocity );
        float F = (-k * x) + (-b * v);
        // Turn that force into an impulse that can be applied to the particles.
        vec3 impulse = normalize( relitive_position ) * F;
        // Apply the impulse to both the particles that the spring connects.
        partcles[0]->velocity += impulse * partcles[0]->invmass;
        partcles[1]->velocity -= impulse * partcles[1]->invmass;
    }
//
//
//    /**
//     * In a point mass system, every vertex of a mesh is represented by a particle.
//     * Every particle is attached to other particles by springs to force the object
//     * to maintain its shape.
//     */
//    class PointMassSystem
//    {};
//
    class Cloth {
    protected:
        void Initialize(int gridSize, float distance, const vec3& position);
        void setStructuralSprings(float k, float b);
        void setShearSprings(float k, float b);
        void setBendSprings(float k, float b);


        void applySpringForces(double dms);
        void setParticleMass(float mass);
        void update(double dms);
        void applyForces();

        std::vector< Particle > verts;
        std::vector< Spring > structural;
        std::vector< Spring > shear;
        std::vector< Spring > bend;
        float cloth_size;
    };

    void Cloth::Initialize(int grid_size, float distance, const vec3& position)
    {
        float k = -1.0f;
        float b = 0.0f;
        cloth_size = grid_size;

        //In case we are recycling the cloth, clear any old values:
        verts.clear();
        structural.clear();
        shear.clear();
        bend.clear();

        //Reserve enough vertices for each particle:
        verts.resize(grid_size * grid_size);

        //Find the half size of the cloth. Our cloth will always be a square, the hs value represents both half width and half depth:
        float hs = (float)(grid_size - 1) * 0.5f;

        //We need at least nine particles for a stable simulation:
        if (grid_size< 3)
        {
            grid_size = 3;
        }

        // Next, create the particles that will represent the vertices of the cloth mesh. Loop through the width and depth of the new cloth:
        for (int x = 0; x <grid_size; ++x)
        {
            for (int z = 0; z <grid_size; ++z)
            {
                int i = z * grid_size + x;
                //Find the world space position of the particle/vertex:
                float x_pos = ((float)x + position.x - hs) * distance;
                float z_pos = ((float)z + position.z - hs) * distance;
                //Set the particle/vertex position in world space:
                verts[i].position = vec3(x_pos, position.y, z_pos);
                verts[i].invmass = 1.0f;
                //verts[i].SetBounce(0.0f);
                //verts[i].SetFriction(0.9f);
            }
        }

        //Create the left to right structural springs of the Cloth class:
        for (int x = 0; x <grid_size; ++x)
        {
            for (int z = 0; z <grid_size - 1; ++z)
            {
                //Find the indices of the two particles that need to be connected by the spring:
                int i = z * grid_size + x;
                int j = (z + 1) * grid_size + x;

                //Find the resting length of the spring:
                vec3 iPos = verts[i].position;
                vec3 jPos = verts[j].position;
                float rest = magn(iPos - jPos);

                // Use this resting length to create a new spring between the two particles and add that spring to the structural spring list:
                Spring spring(k, b, rest);
                spring.partcles[0] = &verts[i];
                spring.partcles[1] = &verts[j];
                structural.push_back(spring);
            }
        }

        for (int x = 0; x <grid_size - 1; ++x)
        {
            for (int z = 0; z <grid_size; ++z)
            {
                int i = z * grid_size + x;
                int j = z * grid_size + (x + 1);

                //Find the resting length of the spring:
                float rest = magn(verts[i].position - verts[j].position);

                //Use the resting length to create a new spring connecting the two particles:
                Spring spring(k, b, rest);
                spring.partcles[0] = &verts[i];
                spring.partcles[1] = &verts[j];
                structural.push_back(spring);
            }
        }

        //Create the left to right shear springs of the Cloth class:
        for (int x = 0; x <grid_size - 1; ++x)
        {
            for (int z = 0; z <grid_size - 1; ++z)
            {
                //Find the indices of the particles that need to be connected:
                int i = z * grid_size + x;
                int j = (z + 1) * grid_size + (x + 1);

                //Find the resting length of the string:
                float rest = magn(verts[i].position - verts[j].position);

                //Use the resting length to create a new spring connecting the two particles:
                Spring spring(k, b, rest);
                spring.partcles[0] = &verts[i];
                spring.partcles[1] = &verts[j];
                shear.push_back(spring);
            }
        }

        //Create the up and down shear springs of the Cloth class:
        for (int x = 1; x <grid_size; ++x)
        {
            for (int z = 0; z <grid_size - 1; ++z)
            {
                //Find the indices of the particles that need to be connected:
                int i = z * grid_size + x;
                int j = (z + 1) * grid_size + (x - 1);

                //Find the resting length of the string:
                float rest = magn(verts[i].position - verts[j].position);

                //Use the resting length to create a new spring connecting the two particles:
                Spring spring(k, b, rest);
                spring.partcles[0] = &verts[i];
                spring.partcles[1] = &verts[j];
                shear.push_back(spring);
            }
        }

        //Create the left to right bend springs of the Cloth class:
        for (int x = 0; x <grid_size; ++x)
        {
            for (int z = 0; z <grid_size - 2; ++z)
            {
                //Find the indices of the particles that need to be connected:
                int i = z * grid_size + x;
                int j = (z + 2) * grid_size + x;

                //Find the resting length of the spring:
                float rest = magn(verts[i].position - verts[j].position);

                //Use the resting length to create a new spring connecting the two particles:
                Spring spring(k, b, rest);
                spring.partcles[0] = &verts[i];
                spring.partcles[1] = &verts[j];
                bend.push_back(spring);
            }
        }

        //Finish implementing the Initialize function of the Cloth class by creating the up and down bend springs:
        for (int x = 0; x <grid_size - 2; ++x)
        {
            for (int z = 0; z <grid_size; ++z)
            {
                //Find the indices of the particles that need to be connected:
                int i = z * grid_size + x;
                int j = z * grid_size + (x + 2);

                //Find the resting length of the spring:
                float rest = magn(verts[i].position - verts[j].position);

                //Use the resting length to create a new spring connecting the two particles:
                Spring spring(k, b, rest);
                spring.partcles[0] = &verts[i];
                spring.partcles[1] = &verts[j];
                bend.push_back(spring);
            }
        }
    }

    void Cloth::setStructuralSprings(float k, float b)
    {
        for (int i = 0; i < structural.size(); ++i)
        {
            structural[i].k = k;
            structural[i].b = b;
        }
    }
    void Cloth::setShearSprings(float k, float b)
    {
        for (int i = 0; i < shear.size(); ++i)
        {
            shear[i].k = k;
            shear[i].b = b;
        }
    }
    void Cloth::setBendSprings(float k, float b)
    {
        for (int i = 0; i<  bend.size(); ++i)
        {
            bend[i].k = k;
            bend[i].b = b;
        }
    }
    void Cloth::setParticleMass(float mass)
    {
        float invmass = 1.0 / mass;
        for (int i = 0; i < verts.size(); ++i)
        {
            verts[i].invmass = invmass;
        }
    }
    void Cloth::applyForces()
    {
        for (int i = 0; i< verts.size(); ++i)
        {
            //verts[i].applyForces();
        }
    }
    void Cloth::update(double dms)
    {
        for (int i = 0; i< verts.size(); ++i)
        {
            //verts[i].update(dt);
        }
    }
    void Cloth::applySpringForces(double dms)
    {
        for (int i = 0; i < structural.size(); ++i)
        {
            structural[i].applyForce( dms );
        }
        for (int i = 0; i < shear.size(); ++i)
        {
            shear[i].applyForce( dms );
        }
        for (int i = 0; i < bend.size(); ++i)
        {
            bend[i].applyForce( dms );
        }
    }

}


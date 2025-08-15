//
//  box-vs-box.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "box-vs-box.hpp"

namespace kege::algo{
    
    bool getClipPoints( const OBB* box, Plane plane, Line edges[4], std::vector< vec3 >* intersections )
    {
        vec3 point;
        for ( int i = 0; i < 4; ++i )
        {
            if ( clipLine( plane, edges[ i ], point ) )
            {
                if ( testPointBox( point, box ) )
                {
                    //float penetration = computePenetrationDepth(edges[ i ], plane);
                    intersections->push_back( point );
                }
            }
        }
#ifdef KEGE_DEBUG_CONTACT_GENERATION
        if ( !intersections->empty() )
        {
            for (int i = 0; i < 4; ++i) Communication::broadcast<const MsgDrawLine&>({ edges[ i ] });
            Communication::broadcast<const MsgDrawLine&>({ Line{
                plane.point,
                plane.point + plane.normal
            } });
        }
#endif
        return !intersections->empty();
    }

    void generateContacts( const OBB* box[2], CollisionManifold* contact, float penetration, bool flip )
    {
        std::vector< vec3 > intersections;
        contact->contact_count = 0;
        /**
         * 1. Understanding Reference vs. Incident Boxes
         *
         * - The reference box is the box against which the collision is evaluated,
         *   while the incident box is the box that "hits" the reference box. The
         *   choice depends on the collision normal:
         *
         * - The reference face is the face of the reference box most aligned with
         *   the collision normal.
         *
         * - The incident face is the face of the incident box most anti-aligned with
         *   the collision normal (i.e., the face that "contacts" the reference face).
         *
         * 2. Determining the Reference Box
         *
         *  - The collision normal typically points from one box toward the other.
         *    The convention is:
         *
         *  - If the normal points from Box A to Box B, then:
         *
         *  - Reference Box = Box A (the box the normal points away from).
         *
         *  - Incident Box = Box B (the box the normal points toward).
         *
         *  - But sometimes, the choice is arbitrary, and you can pick either box as
         *    the reference (as long as you handle the normal direction correctly).
         */
        const OBB* reference_box = box[0];
        const OBB* incident_box  = box[1];
        const vec3 axes[2][6] =
        {
            {
                reference_box->axes[0], -reference_box->axes[0],
                reference_box->axes[1], -reference_box->axes[1],
                reference_box->axes[2], -reference_box->axes[2],
            },
            {
                incident_box->axes[0], -incident_box->axes[0],
                incident_box->axes[1], -incident_box->axes[1],
                incident_box->axes[2], -incident_box->axes[2],
            }
        };
        /**
         * 3. Finding the Reference Face
         *
         * Once the reference box is chosen: The reference face is the one whose normal is
         * closest to the collision normal (i.e., has the largest dot product). For an OBB,
         * each face normal is one of its local axes (transformed into world space).
         */
        int ref_axis = 0;
        float max_dot = -FLT_MAX;
        for (int i = 0; i < 6; i++)
        {
            float d = dot( contact->normal, axes[0][i] );
            if (d >= max_dot)
            {
                max_dot = d;
                ref_axis = i;
            }
        }
        /**
         * 4. Finding the Incident Face
         *
         * The incident face is the face of the incident box whose normal is most opposite to
         * the collision normal (i.e., has the most negative dot product).
         */
        int inc_axis = 0;
        float min_dot = FLT_MAX;
        for (int i = 0; i < 6; ++i)
        {
            float d = dot( contact->normal, axes[1][i] );
            if (d <= min_dot)
            {
                min_dot = d;
                inc_axis = i;
            }
        }
        /**
         * store the reference normal
         */
        vec3 reference_normal = axes[0][ref_axis];
        /**
         * store the incident normal
         */
        vec3 incident_normal = axes[1][inc_axis];


        inc_axis /= 2;
        ref_axis /= 2;

        Line ref_side_edges[4];
        Line ref_face_edges[4];
        Line inc_side_edges[4];
        Line inc_face_edges[4];

        Plane ref_face_plane;
        Plane ref_side_planes[4];
        Plane inc_face_plane;
        Plane inc_side_planes[4];

        getOBBFacePlane( reference_box, reference_normal, ref_axis, &ref_face_plane );
        getOBBSideEdges( incident_box, incident_normal, inc_axis, inc_side_edges );
        getClipPoints( reference_box, ref_face_plane, inc_side_edges, &intersections );

        getOBBFaceEdges( reference_box, reference_normal, ref_axis, ref_face_edges );
        getOBBSidePlanes( incident_box, incident_normal, inc_axis, inc_side_planes );
        getClipPoints( incident_box, inc_side_planes[0], ref_face_edges, &intersections );
        getClipPoints( incident_box, inc_side_planes[1], ref_face_edges, &intersections );
        getClipPoints( incident_box, inc_side_planes[2], ref_face_edges, &intersections );
        getClipPoints( incident_box, inc_side_planes[3], ref_face_edges, &intersections );

        getOBBSideEdges( reference_box, reference_normal, ref_axis, ref_side_edges );
        getOBBFacePlane( incident_box, incident_normal, inc_axis, &inc_face_plane );
        getClipPoints( incident_box, inc_face_plane, ref_side_edges, &intersections );

        if( intersections.size() <= 2 )
        {
            getClipPoints( incident_box, inc_face_plane, ref_face_edges, &intersections );
        }

        if( intersections.size() <= 2 )
        {
            getOBBFaceEdges( incident_box, incident_normal, inc_axis, inc_face_edges );
            getClipPoints( reference_box, ref_face_plane, inc_face_edges, &intersections );
        }

//        /**
//         * Check for intersection points
//         */
//        getClipPoints( reference_box, face_plane, edges, &intersections );
//
//        /**
//         * Get the incident plane to test the reference edges against.
//         */
//        getOBBPlane( incident_box, incident_normal, inc_axis / 2, &face_plane );
//        /**
//         * If no contacts are detected, it means the intersection point is located on the incident box
//         * rather than the reference box. In this case, we need to check the reference box against
//         * the incident box to find the contact point.
//         *
//         * Get the side corner edges of the reference box.
//         */
//        getOBBSideEdges( reference_box, reference_normal, ref_axis / 2, edges );
//        /**
//         * Check for intersection points
//         */
//        getClipPoints( incident_box, face_plane, edges, &intersections );
//        /**
//         * If no intersection point is detected using the side corner edges of the reference box,
//         * use the reference box face edges.
//         *
//         * Get the reference box face edges.
//         */
//        getOBBFaceEdges( incident_box, incident_normal, inc_axis / 2, edges );
//        /**
//         * Check for intersection points
//         */
//        if ( !getClipPoints( incident_box, face_plane, edges, &intersections ) )
//        {}

        /**
         * Check for intersection points
         */
        //if ( !getClipPoints( reference_box, plane, edges, &intersections ) )
//        {
//            /**
//             * If no contact point is detected using the side corner edges of the incident box,
//             * use the incident box face edges.
//             */
//            getOBBFaceEdges( incident_box, incident_normal, inc_axis / 2, edges );
//            /**
//             * Check for intersection points
//             */
//            if ( !getClipPoints( reference_box, plane, edges, &intersections ) )
//            {
//                /**
//                 * If no contacts are detected, it means the intersection point is located on the incident box
//                 * rather than the reference box. In this case, we need to check the reference box against
//                 * the incident box to find the contact point.
//                 *
//                 * Get the side corner edges of the reference box.
//                 */
//                getOBBSideEdges( reference_box, reference_normal, ref_axis / 2, edges );
//                /**
//                 * Get the incident plane to test the reference edges against.
//                 */
//                getOBBPlane( incident_box, incident_normal, inc_axis / 2, &plane );
//                /**
//                 * Check for intersection points
//                 */
//                if ( !getClipPoints( incident_box, plane, edges, &intersections ) )
//                {
//                    /**
//                     * If no intersection point is detected using the side corner edges of the reference box,
//                     * use the reference box face edges.
//                     *
//                     * Get the reference box face edges.
//                     */
//                    getOBBFaceEdges( incident_box, incident_normal, inc_axis / 2, edges );
//                    /**
//                     * Check for intersection points
//                     */
//                    if ( !getClipPoints( incident_box, plane, edges, &intersections ) )
//                    {}
//                }
//            }
//        }
        for (int i = 0; i < intersections.size() && contact->contact_count < MAX_CONTACTS; ++i)
        {
            contact->contacts[i].point = intersections[i];
            contact->contacts[i].depth = penetration;
            contact->contact_count++;
        }
#ifdef KEGE_DEBUG_CONTACT_GENERATION
        for (int i = 0; i < intersections.size(); ++i)
        {
            Communication::broadcast< const MsgDrawAABB& >(MsgDrawAABB{
                AABB
                {
                    contact->contacts[i].point - 0.05f,
                    contact->contacts[i].point + 0.05f
                }
            });
        }
#endif
    }

    bool boxBoxCollision(Rigidbody* a, Rigidbody* b, kege::CollisionRegistry& collisions)
    {
        const OBB* box[2] = { a->collider->getBox(), b->collider->getBox() };

        vec3 axes[15];
        int axes_index = 0;
        float penetration = FLT_MAX;
        Interval interval[2];

        // 1. Face axes (local x, y, z of both boxes)
        for (int i = 0; i < 3; ++i) axes[  i  ] = box[0]->axes[i];
        for (int i = 0; i < 3; ++i) axes[i + 3] = box[1]->axes[i];

        // 2. Edge-edge axes (cross products of edges)
        int axes_count = 6;
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                vec3 axis = cross(box[0]->axes[i], box[1]->axes[j]);
                float mag = magn(axis);
                if (mag > 1e-5f)
                {
                    axes[axes_count++] = axis / mag;
                }
            }
        }

        // 3. Perform SAT (Separating Axis Test)
        for (int i = 0; i < axes_count; ++i)
        {
            projectOBB(box[0], axes[i], interval[0]);
            projectOBB(box[1], axes[i], interval[1]);

            if (!overlaps(interval[0], interval[1]))
            {
                return false; // No collision
            }

            float depth = computePenetration(interval[0], interval[1]);
            if (depth < penetration)
            {
                penetration = depth;
                axes_index = i;
            }
        }

        // 4. Setup collision result
        CollisionManifold* collision = collisions.generate();
        collision->normal = axes[axes_index];
        collision->contact_count = 0;
        collision->objects[0] = a;
        collision->objects[1] = b;

        // 5. Make sure normal is pointing from A to B
        bool flip = false;
        if (dot(box[1]->center - box[0]->center, collision->normal) < 0.f)
        {
            collision->normal = -collision->normal;
            flip = true;
        }

        generateContacts(box, collision, penetration, flip);
        return true;
    }

}

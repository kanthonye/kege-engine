//
//  utils.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#ifndef utils_hpp
#define utils_hpp

#define KEGE_DEBUG_CONTACT_GENERATION

#include "../../../debug/draw-commands.hpp"
#include "../../dynamics/rigidbody.hpp"
#include "../collider/collider.hpp"
#include "../contact/contact.hpp"

namespace kege::algo{

    struct RayHit
    {
        bool hit;         // True if the ray intersects the OBB
        float distance;   // Distance from ray origin to hit point
        kege::vec3 point;       // Intersection point in world space
    };

    struct Intersection
    {
        vec3 point;
        float penetration;
    };

    // Solves the intersection between three planes
    bool intersectThreePlanes(const Plane& p1, const Plane& p2, const Plane& p3, kege::vec3& out_point);

    // Function to find a point on the line of intersection of two planes
    Point findPointOnIntersection(const Plane& a, const Plane& b);

    // Checks if a point is within a circle (in 3D space)
    bool isPointInCircle( const kege::vec3& point, const Circle& circle );
    

    /**
     * The ClipToPlane function checks whether a line intersects a plane and
     * if it does, the function returns the point of intersection.
     */
    bool clipLine(const Plane& plane, const Line& edge, vec3& intersection);

    bool testPointBox(const Point& point, const OBB* box);

    void projectOBB( const OBB* obb, const vec3& axis, Interval& interval );

    float computePenetration( const Interval& a, const Interval& b );

    bool overlaps( const Interval& a, const Interval& b );

    void getBoxPlanes(const OBB* box, Plane (&planes)[ 6 ]);

    void getBoxCorners(const OBB* box, vec3 (&corners)[8]);

    // Function to get edges of an OBB as pairs of points
    void getBoxEdges(vec3 (&corners)[8], Line (&edges)[12]);

    void getBoxEdges( const OBB* box, Line (&edges)[12] );

    void getOBBSidePlanes( const OBB* box, const vec3& normal, int face_index, Plane (&planes)[4] );

    bool getOBBFacePlane( const OBB* box, const vec3& normal, int face_index, Plane* plane );

    void getOBBFaceEdges( const OBB* box, const vec3& normal, int face_index, Line edges[4] );

    bool getOBBSideEdges( const OBB* box, const vec3& normal, int axis_index, Line edges[4] );

    // Helper function to find closest point on triangle
    vec3 closestPointOnTriangle(const Triangle& triangle, const vec3& sphereCenter);

    /**
     * Projects a cylinder onto an arbitrary axis.
     *
     * @param cylinder Pointer to the cylinder.
     * @param axis The axis to project onto (should be normalized).
     * @param interval Output interval representing the min and max projection.
     */
    void projectCylinder(const Cylinder* cylinder, const vec3& axis, Interval& interval );

    /**
     * Projects a capsule onto an arbitrary axis.
     *
     * @param capsule Pointer to the capsule to project.
     * @param axis The axis to project onto (should be normalized).
     * @param interval Output interval representing the min and max projection.
     */
    void projectCapsule(const Capsule* capsule, const vec3& axis, Interval& interval);

    Point closestPointPointBox( const Point& point, const OBB* box );

    Sphere getSphere(const OBB* box);

    AABB obb_to_aabb(const OBB& obb);

    float computePenetrationDepth(const Line& line, const Plane& plane);

}

#endif /* utils_hpp */

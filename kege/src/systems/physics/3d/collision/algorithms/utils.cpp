//
//  utils.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 4/15/25.
//

#include "utils.hpp"
namespace kege::algo{

    // Solves the intersection between three planes
    bool intersectThreePlanes(const Plane& p1, const Plane& p2, const Plane& p3, kege::vec3& out_point)
    {
        float denom = kege::dot(p1.normal, kege::cross(p2.normal, p3.normal));
        if (std::abs(denom) < 1e-6f) // Planes don't intersect at a single point
        {
            return false;
        }

        out_point = (kege::dot(p1.point, p1.normal) * kege::cross(p2.normal, p3.normal) +
                     kege::dot(p2.point, p2.normal) * kege::cross(p3.normal, p1.normal) +
                     kege::dot(p3.point, p3.normal) * kege::cross(p1.normal, p2.normal)) / denom;
        return true;
    }

    // Function to find a point on the line of intersection of two planes
    Point findPointOnIntersection(const Plane& a, const Plane& b)
    {
        Point point;
        double det;

        // Try setting z = 0
        det = a.normal.x * b.normal.y - a.normal.y * b.normal.x;
        if (std::abs(det) > 1e-6)
        {
            point.x = (a.distance * b.normal.y - a.normal.y * b.distance) / det;
            point.y = (a.normal.x * b.distance - a.distance * b.normal.x) / det;
            point.z = 0.0;
            return point;
        }

        // Try setting y = 0
        det = a.normal.x * b.normal.z - a.normal.z * b.normal.x;
        if (std::abs(det) > 1e-6)
        {
            point.x = (a.distance * b.normal.z - a.normal.z * b.distance) / det;
            point.y = 0.0;
            point.z = (a.normal.x * b.distance - a.distance * b.normal.x) / det;
            return point;
        }

        // Try setting x = 0
        det = a.normal.y * b.normal.z - a.normal.z * b.normal.y;
        if (std::abs(det) > 1e-6)
        {
            point.x = 0.0;
            point.y = (a.distance * b.normal.z - a.normal.z * b.distance) / det;
            point.z = (a.normal.y * b.distance - a.distance * b.normal.y) / det;
            return point;
        }

        // If all determinants are close to zero, the planes are likely parallel or the same.
        // In a true intersection line scenario, this should not happen.
        return {0.0, 0.0, 0.0}; // Return origin as a fallback, but it might not be on the line
    }

    // Checks if a point is within a circle (in 3D space)
    bool isPointInCircle( const kege::vec3& point, const Circle& circle )
    {
        /**
         To determine if the point lies on the circle, we just need to measure the distance
         between this intersection point and the circle's center. If this distance equals
         the circle's radius, the point lies exactly on the circle. This transforms our
         original 3D problem into a straightforward 2D circle test.
         */
        kege::vec3 diff = point - circle.center;
        float distance_squared = kege::dot(diff, diff);
        return distance_squared <= (circle.radius * circle.radius);
    }

    
    bool clipLine(const Plane& plane, const Line& edge, vec3& intersection)
    {
        // Check for parallel line and plane
        vec3 direction = edge.end - edge.start;
        if (dot( plane.normal, direction ) == 0.0f)
        {
            return false; // Parallel line
        }

        float d[2];
        // Calculate the signed distances of p0 and p1 from the plane
        d[0] = dot(plane.normal, edge.start) - plane.distance;
        d[1] = dot(plane.normal, edge.end) - plane.distance;

        // If both distances have the same sign, the segment does not cross the plane
        if (d[0] * d[1] > 0.0f)
        {
            return false; // No intersection
        }

        // Find the intersection t parameter
        d[0] = d[0] / (d[0] - d[1]);
        if (d[0] < 0.0f || d[0] > 1.0f)
        {
            return false; // Intersection is not within the segment
        }

        // Calculate intersection point
        intersection = edge.start + d[0] * direction;
        return true;
    }

    bool testPointBox(const Point& point, const OBB* box)
    {
        float distance;
        vec3 dir = box->center - point;

        distance = abs( dot(dir, box->axes[0]) );
        if (distance - box->extents[0] > 0.000005 ) return false;

        distance = abs( dot(dir, box->axes[1]) );
        if (distance - box->extents[1] > 0.000005 ) return false;

        distance = abs( dot(dir, box->axes[2]) );
        if (distance - box->extents[2] > 0.000005  ) return false;

        return true;
    }

    void projectOBB( const OBB* obb, const vec3& axis, Interval& interval )
    {
        float extent =
        (
            obb->extents.x * abs( dot(obb->axes[0], axis) ) +
            obb->extents.y * abs( dot(obb->axes[1], axis) ) +
            obb->extents.z * abs( dot(obb->axes[2], axis) )
        );
        float projection = dot( obb->center, axis );
        interval.min = projection - extent;
        interval.max = projection + extent;
    }

    float computePenetration( const Interval& a, const Interval& b )
    {
        return min( b.max - a.min, a.max - b.min );
    }

    bool overlaps( const Interval& a, const Interval& b )
    {
        return ((b.min <= a.max) && (a.min <= b.max));
    }

    void getBoxPlanes(const OBB* box, Plane (&planes)[ 6 ])
    {
        vec3 ex = box->extents.x * box->axes[0];
        vec3 ey = box->extents.y * box->axes[1];
        vec3 ez = box->extents.z * box->axes[2];
        planes[0] = Plane( box->axes[0], dot( box->axes[0], box->center + ex ) );
        planes[1] = Plane( box->axes[1], dot( box->axes[1], box->center + ey ) );
        planes[2] = Plane( box->axes[2], dot( box->axes[2], box->center + ez ) );
        planes[3] = Plane(-box->axes[0], dot(-box->axes[0], box->center - ex ) );
        planes[4] = Plane(-box->axes[1], dot(-box->axes[1], box->center - ey ) );
        planes[5] = Plane(-box->axes[2], dot(-box->axes[2], box->center - ez ) );
    }

    void getBoxCorners(const OBB* box, vec3 (&corners)[8])
    {
        vec3 ex = box->extents.x * box->axes[0];
        vec3 ey = box->extents.y * box->axes[1];
        vec3 ez = box->extents.z * box->axes[2];

        corners[0] = box->center + ex + ey + ez;
        corners[1] = box->center + ex + ey - ez;
        corners[2] = box->center + ex - ey - ez;
        corners[3] = box->center + ex - ey + ez;

        corners[4] = box->center - ex + ey + ez;
        corners[5] = box->center - ex + ey - ez;
        corners[6] = box->center - ex - ey - ez;
        corners[7] = box->center - ex - ey + ez;
    }

    // Function to get edges of an OBB as pairs of points
    void getBoxEdges(vec3 (&corners)[8], Line (&edges)[12])
    {
        // Define the 12 edges based on corner indices
        edges[ 0] = { corners[0], corners[1] };
        edges[ 1] = { corners[1], corners[2] };
        edges[ 2] = { corners[2], corners[3] };
        edges[ 3] = { corners[3], corners[0] };

        edges[ 4] = { corners[4], corners[5] };
        edges[ 5] = { corners[5], corners[6] };
        edges[ 6] = { corners[6], corners[7] };
        edges[ 7] = { corners[7], corners[4] };

        edges[ 8] = { corners[0], corners[4] };
        edges[ 9] = { corners[1], corners[5] };
        edges[10] = { corners[2], corners[6] };
        edges[11] = { corners[3], corners[7] };
    }

    void getBoxEdges( const OBB* box, Line (&edges)[12] )
    {
        vec3 corners[8];
        getBoxCorners( box, corners );
        getBoxEdges( corners, edges );
    }

    void getOBBSidePlanes( const OBB* box, const vec3& normal, int face_index, Plane (&planes)[4] )
    {
        vec3 ex = box->extents.x * box->axes[0];
        vec3 ey = box->extents.y * box->axes[1];
        vec3 ez = box->extents.z * box->axes[2];
        switch ( face_index )
        {
            case 0: // the reference face is aligns with the x-axis
            {
                planes[0] = Plane(-box->axes[1], box->center + ey );
                planes[1] = Plane( box->axes[1], box->center - ey );
                planes[2] = Plane(-box->axes[2], box->center + ez );
                planes[3] = Plane( box->axes[2], box->center - ez );
                break;
            }

            case 1: // the reference face is aligns with the y-axis
            {
                planes[0] = Plane(-box->axes[0], box->center + ex );
                planes[1] = Plane( box->axes[0], box->center - ex );
                planes[2] = Plane(-box->axes[2], box->center + ez );
                planes[3] = Plane( box->axes[2], box->center - ez );
                break;
            }

            case 2: // the reference face is aligns with the z-axis
            {
                planes[0] = Plane(-box->axes[0], box->center + ex );
                planes[1] = Plane( box->axes[0], box->center - ex );
                planes[2] = Plane(-box->axes[1], box->center + ey );
                planes[3] = Plane( box->axes[1], box->center - ey );
                break;
            }

            default: break;
        }
    }

    bool getOBBFacePlane( const OBB* box, const vec3& normal, int face_index, Plane* plane )
    {
        switch ( face_index )
        {
            case 0: // the face is aligns with the x-axis
            {
                *plane = Plane( normal, box->center + box->extents.x * normal );
                break;
            }

            case 1: // the face is aligns with the y-axis
            {
                *plane = Plane( normal, box->center + box->extents.y * normal );
                break;
            }

            case 2: // the face is aligns with the z-axis
            {
                *plane = Plane( normal, box->center + box->extents.z * normal );
                break;
            }

            default: return false;
        }
        return true;
    }

    void getOBBFaceEdges( const OBB* box, const vec3& normal, int face_index, Line edges[4] )
    {
        // compute the edges of the incident face
        vec3 ex = box->extents.x * box->axes[0];
        vec3 ey = box->extents.y * box->axes[1];
        vec3 ez = box->extents.z * box->axes[2];

        switch ( face_index )
        {
            case 0:
            {
                vec3 corners[4];
                vec3 ex = box->extents.x * normal;
                corners[0] = box->center + ex + ey + ez;
                corners[1] = box->center + ex + ey - ez;
                corners[2] = box->center + ex - ey - ez;
                corners[3] = box->center + ex - ey + ez;
                edges[0] = { corners[0], corners[1] };
                edges[1] = { corners[1], corners[2] };
                edges[2] = { corners[2], corners[3] };
                edges[3] = { corners[3], corners[0] };
                break;
            }

            case 1:
            {
                vec3 corners[4];
                vec3 ey = box->extents.y * normal;
                corners[0] = box->center + ey + ex + ez;
                corners[1] = box->center + ey + ex - ez;
                corners[2] = box->center + ey - ex - ez;
                corners[3] = box->center + ey - ex + ez;
                edges[0] = { corners[0], corners[1] };
                edges[1] = { corners[1], corners[2] };
                edges[2] = { corners[2], corners[3] };
                edges[3] = { corners[3], corners[0] };
                break;
            }

            case 2:
            {
                vec3 corners[4];
                vec3 ez = box->extents.z * normal;
                corners[0] = box->center + ez + ex + ey;
                corners[1] = box->center + ez + ex - ey;
                corners[2] = box->center + ez - ex - ey;
                corners[3] = box->center + ez - ex + ey;
                edges[0] = { corners[0], corners[1] };
                edges[1] = { corners[1], corners[2] };
                edges[2] = { corners[2], corners[3] };
                edges[3] = { corners[3], corners[0] };
                break;
            }

            default:
                break;
        }
    }

    bool getOBBSideEdges( const OBB* box, const vec3& normal, int axis_index, Line edges[4] )
    {
        // compute the edges of the incident face
        vec3 ex = box->extents.x * box->axes[0];
        vec3 ey = box->extents.y * box->axes[1];
        vec3 ez = box->extents.z * box->axes[2];
        vec3 corners[8];

        switch ( axis_index )
        {
            case 0:
            {
                vec3 ex = box->extents.x * normal;
                // top
                corners[0] = box->center + ex + ey + ez;
                corners[1] = box->center + ex + ey - ez;
                corners[2] = box->center + ex - ey - ez;
                corners[3] = box->center + ex - ey + ez;
                // bottom
                corners[4] = box->center - ex + ey + ez;
                corners[5] = box->center - ex + ey - ez;
                corners[6] = box->center - ex - ey - ez;
                corners[7] = box->center - ex - ey + ez;
                // set side edges
                edges[0] = { corners[0], corners[4] };
                edges[1] = { corners[1], corners[5] };
                edges[2] = { corners[2], corners[6] };
                edges[3] = { corners[3], corners[7] };
                break;
            }

            case 1:
            {
                vec3 ey = box->extents.y * normal;
                // top
                corners[0] = box->center + ey + ex + ez;
                corners[1] = box->center + ey + ex - ez;
                corners[2] = box->center + ey - ex - ez;
                corners[3] = box->center + ey - ex + ez;
                // bottom
                corners[4] = box->center - ey + ex + ez;
                corners[5] = box->center - ey + ex - ez;
                corners[6] = box->center - ey - ex - ez;
                corners[7] = box->center - ey - ex + ez;
                // set side edges
                edges[0] = { corners[0], corners[4] };
                edges[1] = { corners[1], corners[5] };
                edges[2] = { corners[2], corners[6] };
                edges[3] = { corners[3], corners[7] };
                break;
            }

            case 2:
            {
                vec3 ez = box->extents.z * normal;
                // top
                corners[0] = box->center + ez + ex + ey;
                corners[1] = box->center + ez + ex - ey;
                corners[2] = box->center + ez - ex - ey;
                corners[3] = box->center + ez - ex + ey;
                // bottom
                corners[4] = box->center - ez + ex + ey;
                corners[5] = box->center - ez + ex - ey;
                corners[6] = box->center - ez - ex - ey;
                corners[7] = box->center - ez - ex + ey;
                // set side edges
                edges[0] = { corners[0], corners[4] };
                edges[1] = { corners[1], corners[5] };
                edges[2] = { corners[2], corners[6] };
                edges[3] = { corners[3], corners[7] };
                break;
            }

            default: return false;
        }
        return true;
    }

    vec3 closestPointOnTriangle(const Triangle& triangle, const vec3& sphereCenter)
    {
        // Triangle vertices
        vec3 a = triangle.points[0];
        vec3 b = triangle.points[1];
        vec3 c = triangle.points[2];

        // Check if point is in the vertex region outside A
        vec3 ab = b - a;
        vec3 ac = c - a;
        vec3 ap = sphereCenter - a;

        float d1 = dot(ab, ap);
        float d2 = dot(ac, ap);

        // Check if point is outside vertex A
        if (d1 <= 0.0f && d2 <= 0.0f)
            return a;

        // Check if point is outside vertex B
        vec3 bp = sphereCenter - b;
        float d3 = dot(ab, bp);
        float d4 = dot(ac, bp);

        if (d3 >= 0.0f && d4 <= d3)
            return b;

        // Check if point is outside vertex C
        vec3 cp = sphereCenter - c;
        float d5 = dot(ab, cp);
        float d6 = dot(ac, cp);

        if (d6 >= 0.0f && d5 <= d6)
            return c;

        // Check if point is outside edge AB
        float vc = d1 * d4 - d3 * d2;
        if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
        {
            float v = d1 / (d1 - d3);
            return a + v * ab;
        }

        // Check if point is outside edge BC
        float va = d3 * d2 - d1 * d4;
        if (va <= 0.0f && (d4 - d3) >= 0.0f && (d1 - d2) >= 0.0f)
        {
            float v = (d4 - d3) / ((d4 - d3) + (d1 - d2));
            return b + v * (c - b);
        }

        // Check if point is outside edge CA
        float vb = d1 * d6 - d5 * d2;
        if (vb <= 0.0f && d6 >= 0.0f && d2 <= 0.0f)
        {
            float v = d6 / (d6 - d2);
            return c + v * (a - c);
        }

        // Point is inside triangle
        float denom = 1.0f / (va + vb + vc);
        float v = vb * denom;
        float w = vc * denom;

        return a + ab * v + ac * w;
    }

    void projectCylinder(const Cylinder* cylinder, const vec3& axis, Interval& interval )
    {
        // Axis representing the cylinder's central direction (normalized)
        vec3 up = normalize(cylinder->axes[0]);

        // The extent due to height along the axis
        float half_height = cylinder->height * 0.5f;
        float height_extent = fabs(dot(axis, up)) * half_height;

        // The extent due to circular radius perpendicular to up axis
        vec3 axis_proj = axis - dot(axis, up) * up;
        float len_sq = dot(axis_proj, axis_proj);

        float radius_extent = 0.0f;
        if (len_sq > 1e-6f)
        {
            vec3 radial = normalize(axis_proj);
            radius_extent = fabs(dot(radial, axis)) * cylinder->radius;
        }
        else
        {
            // If axis is parallel to the up vector, the radial extent is full
            radius_extent = cylinder->radius;
        }

        float center_proj = dot(cylinder->center, axis);
        float total_extent = height_extent + radius_extent;

        interval.min = center_proj - total_extent;
        interval.max = center_proj + total_extent;
    }

    void projectCapsule(const Capsule* capsule, const vec3& axis, Interval& interval)
    {
        // Direction along the capsule's height
        vec3 up = capsule->axes[0];

        // Calculate the top and bottom center points of the capsule
        vec3 half_height_vec = up * (capsule->height * 0.5f);
        vec3 top    = capsule->center + half_height_vec;
        vec3 bottom = capsule->center - half_height_vec;

        // Project both end points onto the axis
        float proj_top    = dot(top, axis);
        float proj_bottom = dot(bottom, axis);

        // Determine the interval by taking min and max, expanded by radius
        float min_proj = fminf(proj_top, proj_bottom);
        float max_proj = fmaxf(proj_top, proj_bottom);

        interval.min = min_proj - capsule->radius;
        interval.max = max_proj + capsule->radius;
    }

    Point closestPointPointBox( const Point& point, const OBB* box )
    {
        float distance;
        Point result = box->center;
        vec3 d = point - box->center;

        distance = dot(d, box->axes[0]);
        if (distance >  box->extents[0]) distance =  box->extents[0];
        if (distance < -box->extents[0]) distance = -box->extents[0];
        result += distance * box->axes[0];

        distance = dot(d, box->axes[1]);
        if (distance >  box->extents[1]) distance =  box->extents[1];
        if (distance < -box->extents[1]) distance = -box->extents[1];
        result += distance * box->axes[1];

        distance = dot(d, box->axes[2]);
        if (distance >  box->extents[2]) distance =  box->extents[2];
        if (distance < -box->extents[2]) distance = -box->extents[2];
        result += distance * box->axes[2];
        return result;
    }

    Sphere getSphere(const OBB* box)
    {
        vec3 corner = box->center + box->axes[0] * box->extents[0] + box->axes[1] * box->extents[1] + box->axes[2] * box->extents[2];
        return { box->center, magn(corner - box->center) };
    }

    AABB obb_to_aabb(const OBB& obb)
    {
        AABB aabb;

        // World axes
        vec3 world_axes[3] =
        {
            vec3(1, 0, 0), // X
            vec3(0, 1, 0), // Y
            vec3(0, 0, 1)  // Z
        };

        // Start with the center
        vec3 center = obb.center;

        // Build the rotation matrix from the OBB's axes
        vec3 right   = obb.axes[0] * obb.extents.x;
        vec3 up      = obb.axes[1] * obb.extents.y;
        vec3 forward = obb.axes[2] * obb.extents.z;

        // For each world axis, compute projection of all extents
        vec3 half_size(0.0f);

        half_size[0] = fabs(dot(world_axes[0], right)) + fabs(dot(world_axes[0], up)) + fabs(dot(world_axes[0], forward));
        half_size[1] = fabs(dot(world_axes[1], right)) + fabs(dot(world_axes[1], up)) + fabs(dot(world_axes[1], forward));
        half_size[2] = fabs(dot(world_axes[2], right)) + fabs(dot(world_axes[2], up)) + fabs(dot(world_axes[2], forward));

        aabb.min = center - half_size;
        aabb.max = center + half_size;

        return aabb;
    }

    float computePenetrationDepth(const Line& line, const Plane& plane)
    {
        // Compute signed distances
        float dA = dot(plane.normal, line.start - plane.point);
        float dB = dot(plane.normal, line.end - plane.point);

        // Check if segment crosses the plane
        if (dA * dB >= 0) {
            return 0.0f; // No penetration
        }

        // Compute penetration depth (minimum absolute distance)
        return ( dA < dB ) ? abs(dA) : abs(dB);
    }

}

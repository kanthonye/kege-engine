//
//  quaternion.hpp
//  kege
//
//  Created by Kenneth Esdaile on 10/1/24.
//

#ifndef quaternion_hpp
#define quaternion_hpp

#include <ostream>
#include "matrices.hpp"

namespace kege{


    template< typename var > struct Quat
    {
        inline void operator *= ( const Quat< var >& q )
        {
            var rx = w*q.x + x*q.w + y*q.z - z*q.y;
            var ry = w*q.y - x*q.z + y*q.w + z*q.x;
            var rz = w*q.z + x*q.y - y*q.x + z*q.w;
            var rw = w*q.w - x*q.x - y*q.y - z*q.z;
            x = rx;
            y = ry;
            z = rz;
            z = rw;
        }

        inline Quat< var >& operator+=( const Quat< var >& q )
        {
            this->w += q.w;
            this->x += q.x;
            this->y += q.y;
            this->z += q.z;
            return *this;
        }

        inline void operator *= ( const Vec3< var >& v )
        {
            var rx = w*v.x + y*v.z - z*v.y;
            var ry = w*v.y + z*v.x - x*v.z;
            var rz = w*v.z + x*v.y - y*v.x;
            var rw = -(x*v.x + y*v.y + z*v.z);
            x = rx;
            y = ry;
            z = rz;
            z = rw;
        }

        inline void operator += ( const Vec3< var >& v )
        {
            x += v.x;
            y += v.y;
            z += v.z;
        }

        inline void operator += ( var s )
        {
            x += s;
            y += s;
            z += s;
            w += s;
        }

        inline void operator -= ( var s )
        {
            x -= s;
            y -= s;
            z -= s;
            w -= s;
        }

        inline void operator *= ( var s )
        {
            x *= s;
            y *= s;
            z *= s;
            w *= s;
        }

        inline void operator /= ( var s )
        {
            x /= s;
            y /= s;
            z /= s;
            w /= s;
        }

        inline Quat& operator()( var angle, const Vec3< var >& v)
        {
            double radian = deg2rad(angle * 0.5f);

            double s = sin(radian);
            this->w = cos(radian);
            this->x = v.x * s;
            this->y = v.y * s;
            this->z = v.z * s;

            return *this;
        }


        inline Quat operator -(void)const
        {
            return Quat(-x, -y, -z, -w);
        }

        inline Quat(const Vec3< var >& v)
        :   x(0.0)
        ,   y(0.0)
        ,   z(0.0)
        ,   w(1.0)
        {
            var angle = magn( v );
            if ( 1e-6f < angle )
            {
                operator()( angle, v * (1.0f / angle) );
            }
        }

        inline Quat( var angle, const Vec3< var >& v)
        {
            operator()( angle, v );
        }

        inline Quat(var x, var y, var z, var w)
        :   x(x)
        ,   y(y)
        ,   z(z)
        ,   w(w)
        {}

        inline Quat(const Quat< var >& q)
        :   x(q.x)
        ,   y(q.y)
        ,   z(q.z)
        ,   w(q.w)
        {}

        inline Quat()
        :   x(0.0)
        ,   y(0.0)
        ,   z(0.0)
        ,   w(1.0)
        {}

        union
        {
            struct
            {
                var x, y, z, w;
            };
            struct
            {
                vec2 xy;
                vec2 zw;
            };
            struct
            {
                Vec3< var > xyz;
                float a;
            };
            vec4 xyzw;
        };
    };

    typedef Quat< double > dquat;
    typedef Quat< float > fquat;
    typedef dquat quatd;
    typedef fquat quat;


    template< typename var > std::ostream& operator <<( std::ostream& os, const Quat< var >& q)
    {
        return (os <<"{ " << q.x <<", " << q.y <<", " << q.z <<", " << q.w << " }");
    }


    template< typename var > Quat< var > operator+(const Quat< var >& v, const Quat< var >& r)
    {
       return { v.x + r.x, v.y + r.y, v.z + r.z, v.w + r.w };
    }

    template< typename var > Quat< var > operator*(var s, const Quat< var >& v)
    {
        return { v.x * s, v.y * s, v.z * s, v.w * s };
    }

    template< typename var > Quat< var > operator*(const Quat< var >& q1, const Quat< var >& q2)
    {
        
        return Quat< var >
        (
            q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,  // x component
            q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,  // y component
            q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,  // z component
            q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z   // w component
        );
//        return Quat< var >
//        (
//         v.w*q.x + v.x*q.w + v.y*q.z - v.z*q.y,
//         v.w*q.y - v.x*q.z + v.y*q.w + v.z*q.x,
//         v.w*q.z + v.x*q.y - v.y*q.x + v.z*q.w,
//         v.w*q.w - v.x*q.x - v.y*q.y - v.z*q.z
//        );
    }
    template< typename var > inline Quat< var > operator * (const Quat< var >& q, const Vec3< var > &v)
    {
        return Quat< var >
        (
            q.w*v.x + q.y*v.z - q.z*v.y,
            q.w*v.y + q.z*v.x - q.x*v.z,
            q.w*v.z + q.x*v.y - q.y*v.x,
            -(q.x*v.x + q.y*v.y + q.z*v.z)
        );
    }
    template< typename var > inline Quat< var > operator * (const Vec3< var > &v, const Quat< var >& q)
    {
        return Quat< var >
        (
            q.w*v.x + q.z*v.y - q.y*v.z,
            q.w*v.y + q.x*v.z - q.z*v.x,
            q.w*v.z + q.y*v.x - q.x*v.y,
            -(q.x*v.x + q.y*v.y + q.z*v.z)
        );
    }

    template< typename var > Quat< var > operator-(const Quat< var >& v, const Quat< var >& r)
    {
        return { v.x - r.x, v.y - r.y, v.z - r.z, v.w - r.w };
    }

    template< typename var > Quat< var > operator*(const Quat< var >& v, var s)
    {
        return { v.x * s, v.y * s, v.z * s, v.w * s };
    }

    template< typename var > Quat< var > operator-(const Quat< var >& v, var r)
    {
       return { v.x - r, v.y - r, v.z - r, v.w - r };
    }

    template< typename var > Quat< var > operator-(var r, const Quat< var >& v)
    {
       return { r - v.x, r - v.y, r - v.z, r - v.w };
    }

    template< typename var > Quat< var > operator/(const Quat< var >& v, var r)
    {
       return { v.x / r, v.y / r, v.z / r, v.w / r };
    }

    template< typename var > Quat< var > operator/(var r, const Quat< var >& v)
    {
       return { r / v.x, r / v.y, r / v.z, r / v.w };
    }

    template< typename var > bool operator==(const Quat< var >& v, const Quat< var >& r)
    {
       return CMP(v.x, r.x) && CMP(v.y, r.y) && CMP(v.z, r.z) && CMP(v.w, r.w);
    }

    template< typename var > bool operator!=(const Quat< var >& v, const Quat< var >& r)
    {
       return !(v == r);
    }




    /** dot product of two quaternion. @return float */
    template< typename var >  var dot(const Quat< var > &a, const Quat< var > &b)
    {
        return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
    }

    /** inline function. compute the math::square magnitude of a vectors. */
    template< typename var >  double magnsq(const Quat< var >& v)
    {
        return dot(v,v);
    }

    /** compute the quaternion magnitude/length. @return float */
    template< typename var >  double magn(const Quat< var > &A)
    {
        return sqrt(dot(A, A));
    }

    /** compute the negation of a quaternion. @return quaternion */
    template< typename var >  Quat< var > conjugate(const Quat< var > &q)
    {
        return Quat< var >(q.w, -q.x, -q.y, -q.z);
    }

    /** computer the inverse of a quaternion. @return quaternion */
    template< typename var >  Quat< var > inverse(const Quat< var > &q)
    {
        return conjugate(q) * invrs(dot(q,q));
    }

    template< typename var >  Quat< var > square( const Quat< var > &q )
    {
        float s = 2.0f * q.w;
        return Quat< var >( dot(q, q), s*q.x, s*q.y, s * q.z );
    }

    /** normalize a quaternion. @return quaternion */
    template< typename var >  Quat< var > normalize(const Quat< var > &q)
    {
        double m = q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z;
        if (m == 0) return Quat< var >(q.x, q.y, q.z, (0.0f > q.w) ? -1.0 : 1.0);
        return q * var(1.0 / sqrt(m));
    }

    template< typename var >  void normalized( Quat< var >& q )
    {
        q = normalize( q );
    }

    template< typename var >  Quat< var > cross(const Quat< var >& q1, const Quat< var >& q2)
    {
        return Quat< var >
        (
            q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
            q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
            q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
            q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x
        );
    }

    /**
     * This function takes a quaternion q (representing the rotation) and a vector v.
     * It converts the vector into a quaternion, performs the rotation using quaternion
     * multiplication, and extracts the rotated vector.
     */
//    template< typename var > Vec3< var > rotate(const Quat< var >& q, const Vec3< var >& v)
//    {
//        // Perform the rotation: q * v * q^-1
//        return (q * Quat< var >(0, v) * conjugate(q)).xyz;
//    }

    template< typename T > Vec3< T > rotate(const Quat< T >& q, const Vec3< T >& v)
    {
        // Extract the scalar part of the quaternion
        float s = q.w;

        // Do the math
        return 2.0f * dot(q.xyz, v) * q.xyz + (s*s - dot(q.xyz, q.xyz)) * v + 2.0f * s * cross(q.xyz, v);
    }


    /* convert Euler angles to quaternion */
    template< typename var > Quat< var > eulerToQuat(var pitch, var yaw, var roll)
    {
        float rad_pitch = deg2rad( 0.5f * pitch );
        float rad_yaw   = deg2rad( 0.5f * yaw );
        float rad_roll  = deg2rad( 0.5f * roll );

        const float cp = cos( rad_pitch );
        const float cy = cos( rad_yaw );
        const float cr = cos( rad_roll );

        const float sp = sin( rad_pitch );
        const float sy = sin( rad_yaw );
        const float sr = sin( rad_roll );

        const float w = cp * cy * cr + sp * sy * sr;
        const float x = sp * cy * cr - cp * sy * sr;
        const float y = cp * sy * cr + sp * cy * sr;
        const float z = cp * cy * sr - sp * sy * cr;

        return Quat< var >( x, y, z, w );
    }
    template< typename var > Quat< var > eulerToQuat( const Vec3< var >& euler )
    {
        return eulerToQuat( euler.x, euler.y, euler.z );
    }

    template< typename var > Vec3< var > quatToEuler(const Quat< var >& q)
    {
        Vec3< var > euler;

        // Roll (Z-axis rotation)
        var sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
        var cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
        euler.z = atan2(sinr_cosp, cosr_cosp);

        // Pitch (X-axis rotation)
        var sinp = 2.0f * (q.w * q.y - q.z * q.x);
        if (fabs(sinp) >= 1.0f)
            euler.x = copysign(_PI_ / 2.0f, sinp); // Clamp at ±90 degrees
        else
            euler.x = asin(sinp);

        // Yaw (Y-axis rotation)
        var siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
        var cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
        euler.y = atan2(siny_cosp, cosy_cosp);

        return euler;
    }

    template< typename var > Quat< var > toQuat
    (
        var m0, var m1, var m2,
        var m3, var m4, var m5,
        var m6, var m7, var m8
    )
    {
        Quat< var > q;
        float trace = m0 + m4 + m8;  // Trace of the matrix (sum of diagonal elements)

        if (trace > 0.0f)
        {
            float s = sqrt(trace + 1.0f) * 2.0f;  // s = 4 * qw
            q.w = 0.25f * s;
            q.x = (m7 - m5) / s;
            q.y = (m2 - m6) / s;
            q.z = (m3 - m1) / s;
        }
        else if ((m0 > m4) && (m0 > m8))
        {
            float s = sqrt(1.0f + m0 - m4 - m8) * 2.0f;  // s = 4 * qx
            q.w = (m7 - m5) / s;
            q.x = 0.25f * s;
            q.y = (m1 + m3) / s;
            q.z = (m2 + m6) / s;
        }
        else if (m4 > m8)
        {
            float s = sqrt(1.0f + m4 - m0 - m8) * 2.0f;  // s = 4 * qy
            q.w = (m2 - m6) / s;
            q.x = (m1 + m3) / s;
            q.y = 0.25f * s;
            q.z = (m5 + m7) / s;
        }
        else
        {
            float s = sqrt(1.0f + m8 - m0 - m4) * 2.0f;  // s = 4 * qz
            q.w = (m3 - m1) / s;
            q.x = (m2 + m6) / s;
            q.y = (m5 + m7) / s;
            q.z = 0.25f * s;
        }

        return q;
    }

    template< typename var > Quat< var > quatLookAt( const Vec3< var >& direction, const Vec3< var >& up )
    {
        Vec3< var > f = -normalize(direction);
        Vec3< var > r = normalize(cross(up, f));
        Vec3< var > u = cross(f, r);

        return toQuat
        (
            r.x, u.x, f.x,
            r.y, u.y, f.y,
            r.z, u.z, f.z
        );
    }

    template< typename var > Quat< var > quat_cast( const Vec3< var >& x, const Vec3< var >& y, const Vec3< var >& z )
    {
        return toQuat
        (
            x.x, x.y, x.z,
            y.x, y.y, y.z,
            z.x, z.y, z.z
        );
    }

    template< typename var > Quat< var > quat_cast( const Mat33< var > &m )
    {
        return toQuat
        (
            m.a[0], m.a[1], m.a[2],
            m.a[3], m.a[4], m.a[5],
            m.a[6], m.a[7], m.a[9]
        );
    }

    template< typename var > Quat< var > quat_cast( const Mat44< var > &m )
    {
        return toQuat
        (
            m.a[0], m.a[1], m.a[2],
            m.a[4], m.a[5], m.a[6],
            m.a[8], m.a[9], m.a[10]
        );
    }

    template< typename var > void getAxesXYZ
    (
        const Quat< var > q,
        var& xx, var& xy, var& xz,
        var& yx, var& yy, var& yz,
        var& zx, var& zy, var& zz
    )
    {
        var qxx = (q.x * q.x);
        var qyy = (q.y * q.y);
        var qzz = (q.z * q.z);

        var qxy = (q.x * q.y);
        var qxz = (q.x * q.z);
        var qyz = (q.y * q.z);

        var qxw = (q.w * q.x);
        var qyw = (q.w * q.y);
        var qzw = (q.w * q.z);

        xx = 1.0 - (qyy + qzz) * 2.0; // 0
        xy =       (qxy + qzw) * 2.0; // 1
        xz =       (qxz - qyw) * 2.0; // 2

        yx =       (qxy - qzw) * 2.0; // 3
        yy = 1.0 - (qxx + qzz) * 2.0; // 4
        yz =       (qyz + qxw) * 2.0; // 5

        zx =       (qxz + qyw) * 2.0; // 6
        zy =       (qyz - qxw) * 2.0; // 7
        zz = 1.0 - (qxx + qyy) * 2.0; // 8
    }
    
    template< typename var > Vec3< var > getAxesX( const Quat< var > q )
    {
        var qyy = (q.y * q.y);
        var qzz = (q.z * q.z);

        var qxy = (q.x * q.y);
        var qxz = (q.x * q.z);

        var qyw = (q.w * q.y);
        var qzw = (q.w * q.z);

        return Vec3< var >
        (
            1.0 - (qyy + qzz) * 2.0,
            (qxy + qzw) * 2.0,
            (qxz - qyw) * 2.0
        );
    }

    template< typename var > Vec3< var > getAxesY( const Quat< var > q )
    {
        var qxx = (q.x * q.x);
        var qzz = (q.z * q.z);

        var qxy = (q.x * q.y);
        var qyz = (q.y * q.z);

        var qxw = (q.w * q.x);
        var qzw = (q.w * q.z);

        return Vec3< var >
        (
            (qxy - qzw) * 2.0,
            1.0 - (qxx + qzz) * 2.0,
            (qyz + qxw) * 2.0
        );
    }

    template< typename var > Vec3< var > getAxesZ( const Quat< var > q )
    {
        var qxx = (q.x * q.x);
        var qyy = (q.y * q.y);

        var qxz = (q.x * q.z);
        var qyz = (q.y * q.z);

        var qxw = (q.w * q.x);
        var qyw = (q.w * q.y);

        return Vec3< var >
        (
            (qxz + qyw) * 2.0,
            (qyz - qxw) * 2.0,
            1.0 - (qxx + qyy) * 2.0
        );
    }

    template< typename var > inline void getAxesXYZ
    (
        const Quat< var > q,
        Vec3< var >& x,
        Vec3< var >& y,
        Vec3< var >& z
    )
    {
        getAxesXYZ( q,  x.x, x.y, x.z,  y.x, y.y, y.z,  z.x, z.y, z.z );
    }


    template< typename var > Mat33< var > quatToM33( const Quat< var >& q )
    {
        Mat33< var > m;
        getAxesXYZ( q,  m.a[0], m.a[1], m.a[2],  m.a[3], m.a[4], m.a[5],  m.a[6], m.a[7], m.a[8] );
        return m;
    }

    template< typename var > Mat44< var > quatToM44( const Quat< var >& q )
    {
        Mat44< var > m;
        getAxesXYZ( q,  m.a[0], m.a[1], m.a[2],  m.a[4], m.a[5], m.a[6],  m.a[8], m.a[9], m.a[10] );
        return m;
    }

    template< typename var > void lnq( const Quat< var >& q )
    {
        float m, s;

        m = acosf( q.w );
        s = sinf( m );

        q.w = 0.0f;
        if (0.0f != s)
        {
            s = m / s;
            return Quat< var >( q.x*s, q.y*s, q.z*s, q.w );
        }
        else
        {
            return Quat< var >(0.0f, 0.0f, 0.0f, 0.0f);
        }
    }

    template< typename var > Quat< var > sqrtq( const Quat< var >& b)
    {
        Quat< var > q;
        var x, y, mag, len;

        len = sqrt(b.x * b.x + b.y * b.y + b.w * b.w);
        len = (0.0f != len) ? 1.0f / len : 1.0f;
        q.x = b.x * len;
        q.y = b.z * len;
        q.z = b.z;
        q.w = b.w * len;

        mag = 1.0 / sqrt( q.w * q.w + q.x * q.x );
        x = sqrt( (1.0f - q.y) * 0.5f );
        y = sqrt( (1.0f + q.y) * 0.5f );
        len = sqrt( len );

        return Quat< var >
        (
            q.x * len * x * mag,
            len * y,
            q.z,
            q.w * len * y * mag
        );
    }

    template< typename var > Quat< var > power( const Quat< var > &q, float exponent )
    {
        // Check for the case of an identity quaternion.
        // This will protect against divide by zero
        if ( fabs(q.w) > .9999f )
        {
            return q;
        }
        // Extract the half angle alpha (alpha = theta/2)
        float alpha = acos(q.w);
        // Compute new alpha value
        float new_alpha = alpha * exponent;
        // Compute new w value
        Quat< var > result;
        result.w = cos(new_alpha);
        // Compute new xyz values
        float mult = sin(new_alpha) / sin(alpha); result.x = q.x * mult;
        result.y = q.y * mult;
        result.z = q.z * mult;
        // Return it
        return result;
    }



    template<typename var> Quat< var > from_to(const Vec3< var >& from, const Vec3< var >& to)
    {
        Vec3< var > f = normalize(from);
        Vec3< var > t = normalize(to);
        if (f == t)
        {
            return Quat< var >();
        }
        else if (f == t * -1.0f)
        {
            Vec3< var > ortho = Vec3< var >(1, 0, 0);
            if ( fabsf(f.y) < fabsf(f.x))
            {
                ortho = Vec3< var >(0, 1, 0);
            }
            if ( fabsf(f.z) < fabs(f.y) && fabs(f.z) < fabsf(f.x) )
            {
                ortho = Vec3< var >(0, 0, 1);
            }
            Vec3< var > axis = normalize( cross(f, ortho) );
            return Quat< var >(axis.x, axis.y, axis.z, 0);
        }
        Vec3< var > half = normalize(f + t);
        Vec3< var > axis = cross(f, half);
        return Quat< var >(axis.x, axis.y, axis.z, dot(f, half));
    }

    template<typename var> Quat< var > look_rotation(const Quat< var >& direction, const Quat< var >& up)
    {
        // Find orthonormal basis vectors
        Vec3< var > f = normalize(direction); // Object Forward
        Vec3< var > u = normalize(up); // Desired Up
        Vec3< var > r = cross(u, f); // Object Right
        u = cross(f, r); // Object Up
        // From world forward to object forward
        Quat< var > world_to_object = from_to( Vec3< var >(0, 0, 1), f );  // what direction is the new object up?
        Vec3< var > object_up = world_to_object * Vec3< var >(0, 1, 0);  // From object up to desired up
        Quat< var > u2u = from_to(object_up, u);
        // Rotate to forward direction first
        // then twist to correct up
        Quat< var > result = world_to_object * u2u;
        // Don't forget to normalize the result
        return normalize(result);
    }


    template< typename var > Quat< var > nlerp(const Quat< var >& from, const Quat< var >& to, float t)
    {
        return normalize( lerp( from, to, t ) );
    }

    template< typename var > Quat< var > slerp(const Quat< var >& start, const Quat< var >& end, var t)
    {
        // slerp = (q0 * sin((1-t) + theta) + q1 * sin(theta)) / sin(theta)
        // Compute the cosine of the angle between the two vectors.
        // cos(theta/2) = qa.w*qb.w + qa.x*qb.x + qa.y*qb.y+ qa.z*qb.z
        var d = dot(start, end);

        // If the dot product is negative, slerp won't take
        // the shorter path. Note that v1 and -v1 are equivalent when
        // the negation is applied to all four components. Fix by
        // reversing one quaternion.
        Quat< var > q = end;
        if (d < 0.0f)
        {
            q = -q;
            d = -d;
        }

        if (d > 0.9995)
        {
            // If the inputs are too close, linearly interpolate
            return lerp( start, q, t );
        }

        // Since dot is in range [0, DOT_THRESHOLD], acos is safe
        var theta_0 = acos(d);        // theta_0 = angle between input vectors
        var theta = theta_0*t;          // theta = angle between v0 and result
        var sin_theta = sin(theta);     // compute this value only once
        var sin_theta_0 = sin(theta_0); // compute this value only once

        var s0 = cos(theta) - d * sin_theta / sin_theta_0;  // == sin(theta_0 - theta) / sin(theta_0)
        var s1 = sin_theta / sin_theta_0;

        return (s0 * start) + (s1 * q);
    }

    template< typename var > Mat44< var > viewMatrix( const Quat< var >& rotation, const Vec3< var >& position )
    {
        Mat44< var > m;

        // inverse rotation matrix. which is the transpose of the rotation matrix
        getAxesXYZ
        ( 
            rotation,
            m.a[0], m.a[4], m.a[8],
            m.a[1], m.a[5], m.a[9],
            m.a[2], m.a[6], m.a[10]
        );

        // inverse translation matrix
        Mat44< var > t = translate< var >( -position );

        // return view matrix t * r
        return t * m;
        //return r * t;
    }

//    template< typename var > Quat< var > quatLookAt
//    (
//        const Vec3< var >& direction,
//        const Vec3< var >& up = Vec3< var >(0, 1, 0)
//    )
//    {
//        // Ensure the direction is normalized
//        Vec3< var > forward = normalize( direction );
//
//        // Compute the right vector
//        Vec3< var > right = normalize( cross(up, forward) );
//
//        // Recompute the orthonormal up vector
//        Vec3< var > ortho_up = cross(forward, right);
//
//        // Convert the rotation matrix to a quaternion
//
//        return toQuat< var >
//        (
//            right.x,    right.y,    right.z,
//            ortho_up.x, ortho_up.y, ortho_up.z,
//            forward.x,  forward.y,  forward.z
//        );
//        //return quat_cast( rotation_matrix );
//    }

}
#endif /* quaternion_hpp */

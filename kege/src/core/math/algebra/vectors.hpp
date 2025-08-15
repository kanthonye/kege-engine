//
//  vectors.hpp
//  vulkan-renderer
//
//  Created by Kenneth Esdaile on 5/30/24.
//

#ifndef vectors_hpp
#define vectors_hpp

#include <cmath>
#include <cfloat>
#include <iomanip>
#include <iostream>

namespace kege{

#define _CLOSE_TO_ZERO_ 1e-06
#define _1_DIV_255      0.0039215686
#define _1_DIV_765      0.0013071895
#define KEGE_EPSILON_F 1e-6f

#define _E_             2.71828182845904509079559829843
#define _GOLDEN_RATIO_  1.61803398874989490252573887119
#define _PI_            3.14159265358979311599796346854
#define _2PI_           6.28318530717958623199592693709
#define _PI_OVER_180_   0.01745329251994329547437168059
#define _180_OVER_PI_   57.2957795130823228646477218717
#define _PI_OVER_360_   0.00872664625
#define ONE_DEG_IN_RAD  0.01745329251

#define _sqrt_2_        1.4142135623730951454746218587
#define _sqrt_3_        0.7320508075688772935274463415
#define _sqrt_5_        2.2360679774997898050514777423
#define _sqrt_6_        2.4494897427831778813356322643
#define _sqrt_7_        2.6457513110645907161710965738
#define _sqrt_8_        2.8284271247461902909492437174
#define _sqrt_10_       3.1622776601683795227870632516
#define _sqrt_13_       3.6055512754639891248586991423
#define _sqrt_14_       3.7416573867739413294941641652
#define _sqrt_15_       3.8729833462074170213895740744
#define _1_OVER_sqrt2_  0.7071067811865474617150084668
#define _1_OVER_sqrt5_  0.4472135954999579277036048097

#define ABSOLUTE(x, y) (fabsf((x)–(y)) <= FLT_EPSILON)
#define RELATIVE(x, y) (fabsf((x) – (y)) <= FLT_EPSILON * Max(fabsf(x), fabsf(y)))
#define CMP(x, y) (fabsf((x) - (y)) <= FLT_EPSILON * fmaxf(1.0f, fmaxf( fabsf(x), fabsf(y) ) ))
#define RAD2DEG(x) ((x) * 57.295754f)
#define DEG2RAD(x) ((x) * 0.0174533f)


    typedef float real;

    template< typename var > struct Vec2
    {
        inline const var& operator[](int i)const{ return xy[i]; }
        inline var& operator[](int i) { return xy[i]; }

        inline Vec2 operator -()const
        {
            return Vec2( -x, -y );
        }

        inline Vec2< var >(var x, var y) 
        : x(x)
        , y(y)
        {}

        //inline Vec2< var >( var v ) : x(v), y(v) { }

        inline Vec2< var >()
        : x(0.0f)
        , y(0.0f)
        { }

        union
        {
            struct
            {
                var x;
                var y;
            };
            var xy[2];
        };
    };

    template< typename var > struct Vec3
    {
        inline const var& operator[](int i)const{ return xyz[i]; }
        inline var& operator[](int i) { return xyz[i]; }

        /**
         * Overloading Conversion Operators, to convert from one vector to another.
         * e.g float vect3 to a double vec3
         */
        template< typename J > inline operator Vec3< J >()const
        {
            return Vec3< J >( x, y, z );
        }

        inline void operator += ( const Vec3< var >& v )
        {
            x += v.x;
            y += v.y;
            z += v.z;
        }
        inline void operator += ( var v )
        {
            x += v;
            y += v;
            z += v;
        }

        inline void operator *= ( const Vec3< var >& v )
        {
            x *= v.x;
            y *= v.y;
            z *= v.z;
        }
        inline void operator *= ( var v )
        {
            x *= v;
            y *= v;
            z *= v;
        }

        inline void operator -= ( const Vec3< var >& v )
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
        }
        inline void operator -= ( var v )
        {
            x -= v;
            y -= v;
            z -= v;
        }

        inline void operator /= ( const Vec3< var >& v )
        {
            x /= v.x;
            y /= v.y;
            z /= v.z;
        }
        inline void operator /= ( var v )
        {
            x /= v;
            y /= v;
            z /= v;
        }

        inline Vec3 operator -()const
        {
            return Vec3( -x, -y, -z );
        }

        inline Vec3( const Vec2< var >& v, var z)
        :   x(v.x)
        ,   y(v.y)
        ,   z(z)
        {}

//        inline Vec3( const Vec3< var >& v )
//        :   x(v.x)
//        ,   y(v.y)
//        ,   z(v.z)
//        {}

        inline Vec3(var x, var y, var z) : x(x), y(y), z(z) { }
        inline Vec3( var v ) : x(v), y(v), z(v) { }
        inline Vec3() : x(0.0f), y(0.0f), z(0.0f) { }

        union
        {
            struct
            {
                var x;
                var y;
                var z;
            };
            var xyz[3];
        };
    };

    template< typename var > struct Vec4
    {
        inline void operator += ( const Vec4< var >& v )
        {
            x += v.x;
            y += v.y;
            z += v.z;
            w += v.w;
        }
        inline void operator += ( var v )
        {
            x += v;
            y += v;
            z += v;
            w += v;
        }

        inline void operator *= ( const Vec4< var >& v )
        {
            x *= v.x;
            y *= v.y;
            z *= v.z;
            w *= v.w;
        }
        inline void operator *= ( var v )
        {
            x *= v;
            y *= v;
            z *= v;
            w *= v;
        }

        inline void operator -= ( const Vec4< var >& v )
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            w -= v.w;
        }
        inline void operator -= ( var v )
        {
            x -= v;
            y -= v;
            z -= v;
            w -= v;
        }

        inline void operator /= ( const Vec4< var >& v )
        {
            x /= v.x;
            y /= v.y;
            z /= v.z;
            w /= v.w;
        }
        inline void operator /= ( var v )
        {
            x /= v;
            y /= v;
            z /= v;
            w /= v;
        }

        inline const var& operator[](int i)const{ return xyzw[i]; }
        inline var& operator[](int i) { return xyzw[i]; }

        inline Vec4 operator -()const
        {
            return Vec4( -x, -y, -z, -w );
        }

        inline Vec4< var >( const Vec3< var >& v, var w)
        :   x(v.x)
        ,   y(v.y)
        ,   z(v.z)
        ,   w(w)
        {}

        inline Vec4(var x, var y, var z, var w) : x(x), y(y), z(z), w(w) { }
        inline Vec4( var v ) : x(v), y(v), z(v), w(v) { }
        inline Vec4() : x(0.0), y(0.0), z(0.0), w(0.0) { }

        union
        {
            struct
            {
                Vec3< var > xyz;
                var a;
            };
            struct
            {
                var x;
                var y;
                var z;
                var w;
            };
            var xyzw[4];
        };
    };

    /** converts degree to radian @return radian */
    template < typename var > inline var deg2rad( var deg ){ return deg * (var(_PI_) / 180.0); }

    /** converts radian to degree @return degree */
    template < typename var > inline var rad2deg( var deg ){ return deg * _180_OVER_PI_; }

    /** linear interpulate between values a and b using t. t being {0 <= t <= 1} */
    template< typename var, typename v > inline var lerp( const var& a, const var& b, v t) { return (b - a) * t + a; }

    template< typename var > inline var min( var a, var b){ return ( a < b) ? a : b; }
    template< typename var > inline var max( var a, var b){ return ( a < b) ? b : a; }
    template< typename var > inline var min( var a, var b, var c){ return ( a < b) ? (( a < c ) ? a : c) : (( b < c ) ? b : c); }
    template< typename var > inline var max( var a, var b, var c){ return ( a > b) ? (( a > c ) ? a : c) : (( b > c ) ? b : c); }

    // clamp n to lie within the range [min, max]
    template< typename var > var clamp(var n, var min, var max){ return (n < min) ? min : (n > max) ? max : n; }


//    template< typename var > void swap(var& a, var& b)
//    {
//        var t = a;
//        a = b;
//        b = t;
//    }


    template< typename var > Vec2< var > operator+(const Vec2< var >& v, const Vec2< var >& r)
    {
       return Vec2< var >( v.x + r.x, v.y + r.y );
    }
    template< typename var > Vec3< var > operator+(const Vec3< var >& v, const Vec3< var >& r)
    {
       return Vec3< var >( v.x + r.x, v.y + r.y, v.z + r.z );
    }
    template< typename var > Vec4< var > operator+(const Vec4< var >& v, const Vec4< var >& r)
    {
       return Vec4< var >( v.x + r.x, v.y + r.y, v.z + r.z, v.w + r.w );
    }


    template< typename var > Vec2< var > operator-(const Vec2< var >& v, const Vec2< var >& r)
    {
       return Vec2< var >( v.x - r.x, v.y - r.y );
    }
    template< typename var > Vec3< var > operator-(const Vec3< var >& v, const Vec3< var >& r)
    {
       return Vec3< var >( v.x - r.x, v.y - r.y, v.z - r.z );
    }
    template< typename var > Vec4< var > operator-(const Vec4< var >& v, const Vec4< var >& r)
    {
       return Vec4< var >( v.x - r.x, v.y - r.y, v.z - r.z, v.w - r.w );
    }


    template< typename var > Vec2< var > operator*(const Vec2< var >& v, const Vec2< var >& r)
    {
       return Vec2< var >( v.x * r.x, v.y * r.y );
    }
    template< typename var > Vec3< var > operator*(const Vec3< var >& v, const Vec3< var >& r)
    {
       return Vec3< var >( v.x * r.x, v.y * r.y, v.z * r.z );
    }
    template< typename var > Vec4< var > operator*(const Vec4< var >& v, const Vec4< var >& r)
    {
       return Vec4< var >( v.x * r.x, v.y * r.y, v.z * r.z, v.w * r.w );
    }


    template< typename var, typename T > Vec2< var > operator*(const Vec2< var >& v, T r)
    {
       return Vec2< var >( v.x * r, v.y * r );
    }
    template< typename var, typename T > Vec3< var > operator*(const Vec3< var >& v, T r)
    {
       return Vec3< var >( v.x * r, v.y * r, v.z * r );
    }
    template< typename var, typename T > Vec4< var > operator*(const Vec4< var >& v, T r)
    {
       return Vec4< var >( v.x * r, v.y * r, v.z * r, v.w * r );
    }


    template< typename var, typename T > Vec2< var > operator*(T r, const Vec2< var >& v)
    {
       return Vec2< var >( v.x * r, v.y * r );
    }
    template< typename var, typename T > Vec3< var > operator*(T r, const Vec3< var >& v)
    {
       return Vec3< var >( v.x * r, v.y * r, v.z * r );
    }
    template< typename var , typename T> Vec4< var > operator*(T r, const Vec4< var >& v)
    {
       return Vec4< var >( r * v.x, r * v.y, r * v.z, r * v.w );
    }



    template< typename var, typename T > Vec2< var > operator+(const Vec2< var >& v, T r)
    {
        return Vec2< var >( v.x + r, v.y + r );
    }
    template< typename var, typename T > Vec3< var > operator+(const Vec3< var >& v, T r)
    {
        return Vec3< var >( v.x + r, v.y + r, v.z + r );
    }
    template< typename var, typename T > Vec4< var > operator+(const Vec4< var >& v, T r)
    {
        return Vec4< var >( v.x + r, v.y + r, v.z + r, v.w + r );
    }


    template< typename var, typename T > Vec2< var > operator+(T r, const Vec2< var >& v)
    {
        return Vec2< var >( r + v.x, r + v.y );
    }
    template< typename var , typename T> Vec3< var > operator+(T r, const Vec3< var >& v)
    {
        return Vec3< var >( r + v.x, r + v.y, r + v.z );
    }
    template< typename var , typename T> Vec4< var > operator+(T r, const Vec4< var >& v)
    {
        return Vec4< var >( r + v.x, r + v.y, r + v.z, r + v.w );
    }



    template< typename var, typename T > Vec2< var > operator-(const Vec2< var >& v, T r)
    {
       return Vec2< var >( v.x - r, v.y - r );
    }
    template< typename var, typename T > Vec3< var > operator-(const Vec3< var >& v, T r)
    {
       return Vec3< var >( v.x - r, v.y - r, v.z - r );
    }
    template< typename var, typename T > Vec4< var > operator-(const Vec4< var >& v, T r)
    {
       return Vec4< var >( v.x - r, v.y - r, v.z - r, v.w - r );
    }


    template< typename var, typename T > Vec2< var > operator-(T r, const Vec2< var >& v)
    {
       return Vec2< var >( r - v.x, r - v.y );
    }
    template< typename var , typename T> Vec3< var > operator-(T r, const Vec3< var >& v)
    {
       return Vec3< var >( r - v.x, r - v.y, r - v.z );
    }
    template< typename var , typename T> Vec4< var > operator-(T r, const Vec4< var >& v)
    {
       return Vec4< var >( r - v.x, r - v.y, r - v.z, r - v.w );
    }


    template< typename var, typename T > Vec2< var > operator/(const Vec2< var >& v, T r)
    {
       return Vec2< var >( v.x / r, v.y / r );
    }
    template< typename var, typename T > Vec3< var > operator/(const Vec3< var >& v, T r)
    {
       return Vec3< var >( v.x / r, v.y / r, v.z / r );
    }
    template< typename var, typename T > Vec4< var > operator/(const Vec4< var >& v, T r)
    {
       return Vec4< var >( v.x / r, v.y / r, v.z / r, v.w / r );
    }


    template< typename var, typename T > Vec2< var > operator/(T r, const Vec2< var >& v)
    {
       return Vec2< var >( r / v.x, r / v.y );
    }
    template< typename var, typename T > Vec3< var > operator/(T r, const Vec3< var >& v)
    {
       return Vec3< var >( r / v.x, r / v.y, r / v.z );
    }
    template< typename var , typename T> Vec4< var > operator/(T r, const Vec4< var >& v)
    {
       return Vec4< var >( r / v.x, r / v.y, r / v.z, r / v.w );
    }


    template< typename var > Vec2< var > operator/(const Vec2< var >& l, const Vec2< var >& r)
    {
        return {l.x / r.x, l.y / r.y};
    }
    template< typename var > Vec3< var > operator/(const Vec3< var >& l, const Vec3< var >& r)
    {
        return {l.x / r.x, l.y / r.y, l.z / r.z};
    }
    template< typename var > Vec4< var > operator/(const Vec4< var >& l, const Vec4< var >& r)
    {
        return {l.x / r.x, l.y / r.y, l.z / r.z, l.w / r.w};
    }




    template< typename var > bool operator==(const Vec2< var >& v, const Vec2< var >& r)
    {
       return CMP(v.x, r.x) && CMP(v.y, r.y);
    }
    template< typename var > bool operator==(const Vec3< var >& v, const Vec3< var >& r)
    {
       return CMP(v.x, r.x) && CMP(v.y, r.y) && CMP(v.z, r.z);
    }
    template< typename var > bool operator==(const Vec4< var >& v, const Vec4< var >& r)
    {
       return CMP(v.x, r.x) && CMP(v.y, r.y) && CMP(v.z, r.z) && CMP(v.w, r.w);
    }


    template< typename var > bool operator!=(const Vec2< var >& v, const Vec2< var >& r)
    {
       return !(v == r);
    }
    template< typename var > bool operator!=(const Vec3< var >& v, const Vec3< var >& r)
    {
       return !(v == r);
    }
    template< typename var > bool operator!=(const Vec4< var >& v, const Vec4< var >& r)
    {
       return !(v == r);
    }




    template< typename var > var dot(const Vec2< var >& v, const Vec2< var >& r)
    {
       return v.x * r.x + v.y * r.y;
    }

    template< typename var > var dot(const Vec3< var >& v, const Vec3< var >& r)
    {
       return v.x * r.x + v.y * r.y + v.z * r.z;
    }

    template< typename var > var magn(const Vec2< var >& v)
    {
       return sqrtf( dot(v, v) );
    }

    template< typename var > var magn(const Vec3< var >& v)
    {
       return sqrtf( dot(v, v) );
    }

    template< typename var > var magnSq(const Vec2< var >& v)
    {
       return dot(v, v);
    }

    template< typename var > var magnSq(const Vec3< var >& v)
    {
       return dot(v, v);
    }

    template< typename var > var distance(const Vec2< var >& p1, const Vec2< var >& p2)
    {
       return magn( p1 - p2 );
    }

    template< typename var > var distance(const Vec3< var >& p1, const Vec3< var >& p2)
    {
       return magn( p1 - p2 );
    }

    template< typename var > void normalized(Vec2< var >& v)
    {
       v = v * (1.0f / magn(v));
    }

    template< typename var > void normalized(Vec3< var >& v)
    {
       v = v * (1.0f / magn(v));
    }

    template< typename var > Vec2< var > normalize(const Vec2< var >& v)
    {
       return v * (1.0f / magn(v));
    }

    template< typename var > Vec3< var > normalize(const Vec3< var >& v)
    {
       return v * (1.0f / magn(v));
    }

    template< typename var > var cross(const Vec2< var >& u, const Vec2< var >& v)
    {
        return u.y * v.x - u.x * v.y;
    }

    template< typename var > Vec3< var > cross(const Vec3< var >& v, const Vec3< var >& r)
    {
        return Vec3< var >
        (
            v.y * r.z - v.z * r.y,
            v.z * r.x - v.x * r.z,
            v.x * r.y - v.y * r.x
        );
    }

    template< typename var > var angle(const Vec2< var >& v, const Vec2< var >& r)
    {
        var m = sqrtf( magnSq(v) * magnSq(r) );
        return acos(dot(v, r) / m);
    }

    template< typename var > var angle(const Vec3< var >& v, const Vec3< var >& r)
    {
        var m = sqrtf( magnSq(v) * magnSq(r) );
        return acos(dot(v, r) / m);
    }

    template< typename var > Vec2< var > project(const Vec2< var >& length, const Vec2< var >& direction)
    {
       var d = dot(length, direction);
       var magSq = magnSq(direction);
       return direction * (d / magSq);
    }

    template< typename var > Vec3< var > project(const Vec3< var >& length, const Vec3< var >& direction)
    {
       var d = dot(length, direction);
       var mag_sq = magnSq(direction);
       return direction * (d / mag_sq);
    }

    template< typename var > Vec2< var > perpendicular(const Vec2< var >& len, const Vec2< var >& dir)
    {
       return len - project(len, dir);
    }

    template< typename var > Vec3< var > perpendicular(const Vec3< var >& len, const Vec3< var >& dir)
    {
       return len - project(len, dir);
    }

    template< typename var > Vec2< var > reflection(const Vec2< var >& vec,const Vec2< var >& normal)
    {
       var d = dot(vec, normal);
       return vec - normal * (d * 2.0f );
    }

    template< typename var > Vec3< var > reflection(const Vec3< var >& vec, const Vec3< var >& normal)
    {
       var d = dot(vec, normal);
       return vec - normal * (d * 2.0f);
    }


    template< typename var > bool isnan( const Vec3< var >& v )
    {
        return std::isnan( v.x ) || std::isnan( v.y ) || std::isnan( v.z );
    }



    template< typename var > inline std::ostream& operator <<(std::ostream& os, const Vec2< var >& v )
    {
        std::cout << "[ " <<std::setw(8) << v.x <<", " <<std::setw(8) << v.y <<" ]";
        return os;
    }

    template< typename var > inline std::ostream& operator <<(std::ostream& os, const Vec3< var >& v )
    {
        std::cout << "[ " <<std::setw(8) << v.x <<", " <<std::setw(8) << v.y <<", " <<std::setw(8) << v.z <<" ]";
        return os;
    }

    template< typename var > inline std::ostream& operator <<(std::ostream& os, const Vec4< var >& v )
    {
        std::cout << "[ " <<std::setw(8) << v.x <<", " <<std::setw(8) << v.y <<", " <<std::setw(8) << v.z <<", " <<std::setw(8) << v.w <<" ]";
        return os;
    }
    


    template< typename var > inline var zeroSmallValue( const var& v, const float threshold = 1e-6f )
    {
        return ( fabs( v ) < threshold ) ? var(0x0) : v;
    }

    template< typename var > Vec2< var > zeroSmallComponents( const Vec2< var >& v, const float threshold = 1e-6f )
    {
        return Vec2< var >
        {
            zeroSmallValue( v.x, threshold ),
            zeroSmallValue( v.y, threshold )
        };
    }

    template< typename var > Vec3< var > zeroSmallComponents( const Vec3< var >& v, const float threshold = 1e-6f )
    {
        return Vec3< var >
        {
            zeroSmallValue( v.x, threshold ),
            zeroSmallValue( v.y, threshold ),
            zeroSmallValue( v.z, threshold )
        };
    }

    template< typename var > void zeroSmallComponents( Vec4< var >& v, const float threshold = 1e-6f )
    {
        return Vec4< var >
        {
            zeroSmallValue( v.x, threshold ),
            zeroSmallValue( v.y, threshold ),
            zeroSmallValue( v.z, threshold ),
            zeroSmallValue( v.w, threshold )
        };
    }



    template< typename var > var sq( const var& v )
    {
        return v * v;
    }



    typedef Vec2< int8_t   > sbyte2;
    typedef Vec2< uint8_t  > ubyte2;
    typedef Vec2< int32_t  > sint2;
    typedef Vec2< uint32_t > uint2;
    typedef Vec2< int16_t  > short2;
    typedef Vec2< uint16_t > ushort2;
    typedef Vec2< int64_t  > slong2;
    typedef Vec2< uint64_t > ulong2;

    typedef Vec3< int8_t   > sbyte3;
    typedef Vec3< uint8_t  > ubyte3;
    typedef Vec3< int32_t  > sint3;
    typedef Vec3< uint32_t > uint3;
    typedef Vec3< int16_t  > short3;
    typedef Vec3< uint16_t > ushort3;
    typedef Vec3< int64_t  > slong3;
    typedef Vec3< uint64_t > ulong3;

    typedef Vec4< int8_t   > sbyte4;
    typedef Vec4< uint8_t  > ubyte4;
    typedef Vec4< int32_t  > sint4;
    typedef Vec4< uint32_t > uint4;
    typedef Vec4< int16_t  > short4;
    typedef Vec4< uint16_t > ushort4;
    typedef Vec4< int64_t  > slong4;
    typedef Vec4< uint64_t > ulong4;


    typedef Vec2< double > vec2d;
    typedef Vec2< float  > vec2f;

    typedef Vec3< double > vec3d;
    typedef Vec3< float  > vec3f;

    typedef Vec4< double > vec4d;
    typedef Vec4< float  > vec4f;


    typedef sint2 vec2i;
    typedef sint3 vec3i;
    typedef sint4 vec4i;

    typedef vec2d point2d;
    typedef vec2f point2f;
    typedef vec2i point2i;

    typedef vec2d dvec2;
    typedef vec2f fvec2;
    typedef vec2i ivec2;

    typedef vec2f point2;
    typedef vec2f vec2;

    typedef vec3d point3d;
    typedef vec3f point3f;
    typedef vec3i point3i;

    typedef vec3d dvec3;
    typedef vec3f fvec3;
    typedef vec3i ivec3;

    typedef vec3f point3;
    typedef vec3f vec3;

    typedef vec4d point4d;
    typedef vec4f point4f;
    typedef vec4i point4i;

    typedef vec4d dvec4;
    typedef vec4f fvec4;
    typedef vec4i ivec4;

    typedef vec4f point4;
    typedef vec4f vec4;

    typedef int32_t     sint1;
    typedef uint32_t    uint1;
    typedef int8_t      sbyte1;
    typedef uint8_t     ubyte1;
    typedef int16_t     sshort1;
    typedef uint16_t    ushort1;
}
#endif /* vectors_hpp */

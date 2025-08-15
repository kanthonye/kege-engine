//
//  matrices.hpp
//  kege
//
//  Created by Kenneth Esdaile on 9/30/24.
//

#ifndef matrices_hpp
#define matrices_hpp

#include "vectors.hpp"
namespace kege{

    template< typename var > struct Mat22
    {
        Mat22
        (
            var m00, var m10,
            var m01, var m11
        )
        :   a00( m00 ), a10( m10 )
        ,   a01( m01 ), a11( m11 )
        {}

        Mat22( var val )
        :   a00( val ), a10( 0.f )
        ,   a01( 0.f ), a11( val )
        {}

        Mat22
        (
            Vec2< var > v1, 
            Vec2< var > v2
        )
        :   a00( v1.x ), a10( v2.x )
        ,   a01( v1.y ), a11( v2.y )
        {}

        Mat22()
        {}

        inline const Vec2< var >& operator[](int i)const
        {
            return v[ i ];
        }
        inline Vec2< var >& operator[](int i)
        {
            return v[ i ];
        }

        union
        {
            struct
            {
                var a00, a01;
                var a10, a11;
                //var a00, a10;
                //var a01, a11;
            };
            var a[4];
            Vec2< var > v[2];
        };
    };

    typedef Mat22< double > mat22d;
    typedef Mat22< float > mat22f;
    typedef Mat22< int > mat22i;
    typedef mat22d dmat22;
    typedef mat22f fmat22;
    typedef mat22i imat22;
    typedef mat22f mat22;




    template< typename var > struct Mat33
    {
        Mat33
        (
            var m00, var m10, var m20,
            var m01, var m11, var m21,
            var m02, var m12, var m22
        )
        :   a00( m00 ), a10( m10 ), a20( m20 )
        ,   a01( m01 ), a11( m11 ), a21( m21 )
        ,   a02( m02 ), a12( m12 ), a22( m22 )
        {}

        Mat33
        (
            const Vec3< var >& x,
            const Vec3< var >& y,
            const Vec3< var >& z
        )
        :   a00( x.x ), a10( y.x ), a20( z.x )
        ,   a01( x.y ), a11( y.y ), a21( z.y )
        ,   a02( x.z ), a12( y.z ), a22( z.z )
        {}

        Mat33( const Mat33& val )
        {
            memcpy(a, val.a, 9 * sizeof(var));
        }

        Mat33( const Vec3< var >& v )
        :   a00( v.x ), a10( 0.f ), a20( 0.f )
        ,   a01( 0.f ), a11( v.y ), a21( 0.f )
        ,   a02( 0.f ), a12( 0.f ), a22( v.z )
        {}

        Mat33( var val )
        :   a00( val ), a10( 0.f ), a20( 0.f )
        ,   a01( 0.f ), a11( val ), a21( 0.f )
        ,   a02( 0.f ), a12( 0.f ), a22( val )
        {}

        Mat33()
        :   a00( 1.f ), a10( 0.f ), a20( 0.f )
        ,   a01( 0.f ), a11( 1.f ), a21( 0.f )
        ,   a02( 0.f ), a12( 0.f ), a22( 1.f )
        {}

        inline const Vec3< var >& operator[](int i)const
        {
            return v[i];
        }
        inline Vec3< var >& operator[](int i)
        {
            return v[i];
        }

        union
        {
            struct
            {
                var a00, a01, a02;
                var a10, a11, a12;
                var a20, a21, a22;
            };
            
            Vec3< var > v[3];
            
            var a[9];
        };
    };

    typedef Mat33< double > mat33d;
    typedef Mat33< float > mat33f;
    typedef Mat33< int > mat33i;
    typedef mat33d dmat33;
    typedef mat33f fmat33;
    typedef mat33i imat33;
    typedef mat33f mat33;



    template< typename var > struct Mat44
    {
        Mat44
        (
            var m00, var m10, var m20, var m30,
            var m01, var m11, var m21, var m31,
            var m02, var m12, var m22, var m32,
            var m03, var m13, var m23, var m33
        )
        :   a00( m00 ), a10( m10 ), a20( m20 ), a30( m30 )
        ,   a01( m01 ), a11( m11 ), a21( m21 ), a31( m31 )
        ,   a02( m02 ), a12( m12 ), a22( m22 ), a32( m32 )
        ,   a03( m03 ), a13( m13 ), a23( m23 ), a33( m33 )
        {}

        Mat44
        (
            const Vec4< var >& x,
            const Vec4< var >& y,
            const Vec4< var >& z,
            const Vec4< var >& w
        )
        :   a00( x.x ), a10( y.x ), a20( z.x ), a30( w.x )
        ,   a01( x.y ), a11( y.y ), a21( z.y ), a31( w.y )
        ,   a02( x.z ), a12( y.z ), a22( z.z ), a32( w.z )
        ,   a03( x.w ), a13( y.w ), a23( z.w ), a33( w.w )
        {}

        Mat44( var val )
        :   a00( val ), a10( 0.f ), a20( 0.f ), a30( 0.f )
        ,   a01( 0.f ), a11( val ), a21( 0.f ), a31( 0.f )
        ,   a02( 0.f ), a12( 0.f ), a22( val ), a32( 0.f )
        ,   a03( 0.f ), a13( 0.f ), a23( 0.f ), a33( val )
        {}

        Mat44()
        :   a00( 1.f ), a10( 0.f ), a20( 0.f ), a30( 0.f )
        ,   a01( 0.f ), a11( 1.f ), a21( 0.f ), a31( 0.f )
        ,   a02( 0.f ), a12( 0.f ), a22( 1.f ), a32( 0.f )
        ,   a03( 0.f ), a13( 0.f ), a23( 0.f ), a33( 1.f )
        {}

        inline const Vec4< var >& operator[](int i)const
        {
            return v[ i ];
        }
        inline Vec4< var >& operator[](int i)
        {
            return v[ i ];
        }

        union
        {
            var a[16];

            struct
            {
                var a00, a01, a02, a03; // right
                var a10, a11, a12, a13; // up
                var a20, a21, a22, a23; // forward
                var a30, a31, a32, a33; // translation
            };

            struct
            {
                var xx, xy, xz, xw; // right
                var yx, yy, yz, yw; // up
                var zx, zy, zz, zw; // forward
                var wx, wy, wz, ww; // translation
            };

            Vec4< var > v[4];
        };
    };

    typedef Mat44< double > mat44d;
    typedef Mat44< float > mat44f;
    typedef Mat44< int > mat44i;
    typedef mat44d dmat44;
    typedef mat44f fmat44;
    typedef mat44i imat44;
    typedef mat44f mat44;










    template< typename var > Mat22< var > operator*(const Mat22< var >& m, const Mat22< var >& n)
    {
        return Mat22< var >
        (
            m.a00 * n.a00 + m.a01 * n.a10, // r00
            m.a00 * n.a01 + m.a01 * n.a11, // r01

            m.a10 * n.a00 + m.a11 * n.a10, // r10
            m.a10 * n.a01 + m.a11 * n.a11  // r11
        );
    }
    template< typename var > Mat33< var > operator*(const Mat33< var >& m, const Mat33< var >& n)
    {
        return Mat33< var >
        (
            m.a00 * n.a00 + m.a01 * n.a10 + m.a02 * n.a20, // r00
            m.a00 * n.a01 + m.a01 * n.a11 + m.a02 * n.a21, // r01
            m.a00 * n.a02 + m.a01 * n.a12 + m.a02 * n.a22, // r02

            m.a10 * n.a00 + m.a11 * n.a10 + m.a12 * n.a20, // r10
            m.a10 * n.a01 + m.a11 * n.a11 + m.a12 * n.a21, // r11
            m.a10 * n.a02 + m.a11 * n.a12 + m.a12 * n.a22, // r12

            m.a20 * n.a00 + m.a21 * n.a10 + m.a22 * n.a20, // r20
            m.a20 * n.a01 + m.a21 * n.a11 + m.a22 * n.a21, // r21
            m.a20 * n.a02 + m.a21 * n.a12 + m.a22 * n.a22  // r22
        );
    }
    template< typename var > Mat44< var > operator*(const Mat44< var >& m, const Mat44< var >& n)
    {
        return Mat44< var >
        (
        m.a00 * n.a00 + m.a01 * n.a10 + m.a02 * n.a20 + m.a03 * n.a30, // r00
        m.a10 * n.a00 + m.a11 * n.a10 + m.a12 * n.a20 + m.a13 * n.a30, // r10
        m.a20 * n.a00 + m.a21 * n.a10 + m.a22 * n.a20 + m.a23 * n.a30, // r20
        m.a30 * n.a00 + m.a31 * n.a10 + m.a32 * n.a20 + m.a33 * n.a30, // r30

        m.a00 * n.a01 + m.a01 * n.a11 + m.a02 * n.a21 + m.a03 * n.a31, // r01
        m.a10 * n.a01 + m.a11 * n.a11 + m.a12 * n.a21 + m.a13 * n.a31, // r11
        m.a20 * n.a01 + m.a21 * n.a11 + m.a22 * n.a21 + m.a23 * n.a31, // r21
        m.a30 * n.a01 + m.a31 * n.a11 + m.a32 * n.a21 + m.a33 * n.a31, // r31

        m.a00 * n.a02 + m.a01 * n.a12 + m.a02 * n.a22 + m.a03 * n.a32, // r02
        m.a10 * n.a02 + m.a11 * n.a12 + m.a12 * n.a22 + m.a13 * n.a32, // r12
        m.a20 * n.a02 + m.a21 * n.a12 + m.a22 * n.a22 + m.a23 * n.a32, // r22
        m.a30 * n.a02 + m.a31 * n.a12 + m.a32 * n.a22 + m.a33 * n.a32, // r32

        m.a00 * n.a03 + m.a01 * n.a13 + m.a02 * n.a23 + m.a03 * n.a33, // r03
        m.a10 * n.a03 + m.a11 * n.a13 + m.a12 * n.a23 + m.a13 * n.a33, // r13
        m.a20 * n.a03 + m.a21 * n.a13 + m.a22 * n.a23 + m.a23 * n.a33, // r23
        m.a30 * n.a03 + m.a31 * n.a13 + m.a32 * n.a23 + m.a33 * n.a33  // r33

           /*
            m.a00 * n.a00 + m.a01 * n.a10 + m.a02 * n.a20 + m.a03 * n.a30, // r00
            m.a00 * n.a01 + m.a01 * n.a11 + m.a02 * n.a21 + m.a03 * n.a31, // r01
            m.a00 * n.a02 + m.a01 * n.a12 + m.a02 * n.a22 + m.a03 * n.a32, // r02
            m.a00 * n.a03 + m.a01 * n.a13 + m.a02 * n.a23 + m.a03 * n.a33, // r03

            m.a10 * n.a00 + m.a11 * n.a10 + m.a12 * n.a20 + m.a13 * n.a30, // r10
            m.a10 * n.a01 + m.a11 * n.a11 + m.a12 * n.a21 + m.a13 * n.a31, // r11
            m.a10 * n.a02 + m.a11 * n.a12 + m.a12 * n.a22 + m.a13 * n.a32, // r12
            m.a10 * n.a03 + m.a11 * n.a13 + m.a12 * n.a23 + m.a13 * n.a33, // r13

            m.a20 * n.a00 + m.a21 * n.a10 + m.a22 * n.a20 + m.a23 * n.a30, // r20
            m.a20 * n.a01 + m.a21 * n.a11 + m.a22 * n.a21 + m.a23 * n.a31, // r21
            m.a20 * n.a02 + m.a21 * n.a12 + m.a22 * n.a22 + m.a23 * n.a32, // r22
            m.a20 * n.a03 + m.a21 * n.a13 + m.a22 * n.a23 + m.a23 * n.a33, // r23

            m.a30 * n.a00 + m.a31 * n.a10 + m.a32 * n.a20 + m.a33 * n.a30, // r30
            m.a30 * n.a01 + m.a31 * n.a11 + m.a32 * n.a21 + m.a33 * n.a31, // r31
            m.a30 * n.a02 + m.a31 * n.a12 + m.a32 * n.a22 + m.a33 * n.a32, // r32
            m.a30 * n.a03 + m.a31 * n.a13 + m.a32 * n.a23 + m.a33 * n.a33  // r33
            */
        );
    }


    template< typename var > Vec2< var > operator*(const Mat22< var >& m, const Vec2< var >& v)
    {
        return Vec2< var >
        (
            m.a00 * v.x + m.a10 * v.y, // x
            m.a01 * v.x + m.a11 * v.y  // y
        );
    }
    template< typename var > Vec3< var > operator*(const Mat33< var >& m, const Vec3< var >& v)
    {
        return Vec3< var >
        (
            m.a00 * v.x + m.a10 * v.y + m.a20 * v.z, // x
            m.a01 * v.x + m.a11 * v.y + m.a21 * v.z, // y
            m.a02 * v.x + m.a12 * v.y + m.a22 * v.z  // z
        );
    }
    template< typename var > Vec4< var > operator*(const Mat44< var >& m, const Vec4< var >& v)
    {
        return Vec4< var >
        (
            m.a00 * v.x + m.a10 * v.y + m.a20 * v.z + m.a30 * v.w, // x
            m.a01 * v.x + m.a11 * v.y + m.a21 * v.z + m.a31 * v.w, // y
            m.a02 * v.x + m.a12 * v.y + m.a22 * v.z + m.a32 * v.w, // z
            m.a03 * v.x + m.a13 * v.y + m.a23 * v.z + m.a33 * v.w  // w
        );
    }


    template< typename var > Vec2< var > operator*(const Vec2< var >& v, const Mat22< var >& m)
    {
        return Vec2< var >
        (
            v.x * m.a00 + v.y * m.a01, // x
            v.x * m.a10 + v.y * m.a11  // y
        );
    }
    template< typename var > Vec3< var > operator*(const Vec3< var >& v, const Mat33< var >& m)
    {
        return Vec3< var >
        (
            v.x * m.a00 + v.y * m.a01 + v.z * m.a02, // x
            v.x * m.a10 + v.y * m.a11 + v.z * m.a12, // y
            v.x * m.a20 + v.y * m.a21 + v.z * m.a22  // z
        );
    }
    template< typename var > Vec4< var > operator*(const Vec4< var >& v, const Mat44< var >& m)
    {
        return Vec4< var >
        (
            v.x * m.a00 + v.y * m.a01 + v.z * m.a02 + v.w * m.a03, // x
            v.x * m.a10 + v.y * m.a11 + v.z * m.a12 + v.w * m.a13, // y
            v.x * m.a20 + v.y * m.a21 + v.z * m.a22 + v.w * m.a23, // z
            v.x * m.a30 + v.y * m.a31 + v.z * m.a32 + v.w * m.a33  // w
        );
    }


    template< typename var > Mat22< var > operator*(const Mat22< var >& m, var scalar)
    {
        return Mat22< var >
        (
            m.a00 * scalar, m.a10 * scalar,
            m.a01 * scalar, m.a11 * scalar
        );
    }
    template< typename var > Mat33< var > operator*(const Mat33< var >& m, var scalar)
    {
        return Mat33< var >
        (
            m.a00 * scalar, m.a10 * scalar, m.a20 * scalar,
            m.a01 * scalar, m.a11 * scalar, m.a21 * scalar,
            m.a02 * scalar, m.a12 * scalar, m.a22 * scalar
        );
    }
    template< typename var > Mat44< var > operator*(const Mat44< var >& m, var scalar)
    {
        return Mat44< var >
        (
            m.a00 * scalar, m.a10 * scalar, m.a20 * scalar, m.a30 * scalar,
            m.a01 * scalar, m.a11 * scalar, m.a21 * scalar, m.a31 * scalar,
            m.a02 * scalar, m.a12 * scalar, m.a22 * scalar, m.a32 * scalar,
            m.a03 * scalar, m.a13 * scalar, m.a23 * scalar, m.a33 * scalar
        );
    }


    template< typename var > Mat22< var > operator*(var scalar, const Mat22< var >& matrix)
    {
        return operator*( matrix, scalar);
    }
    template< typename var > Mat33< var > operator*(var scalar, const Mat33< var >& matrix)
    {
        return operator*( matrix, scalar);
    }
    template< typename var > Mat44< var > operator*(var scalar, const Mat44< var >& matrix)
    {
        return operator*( matrix, scalar);
    }


    template< typename var > Mat22< var > operator+(const Mat22< var >& m, var scalar)
    {
        return Mat22< var >
        (
            m.a00 + scalar, m.a10 + scalar,
            m.a01 + scalar, m.a11 + scalar
        );
    }
    template< typename var > Mat33< var > operator+(const Mat33< var >& m, var scalar)
    {
        return Mat33< var >
        (
            m.a00 + scalar, m.a10 + scalar, m.a20 + scalar,
            m.a01 + scalar, m.a11 + scalar, m.a21 + scalar,
            m.a02 + scalar, m.a12 + scalar, m.a22 + scalar
        );
    }
    template< typename var > Mat44< var > operator+(const Mat44< var >& m, var scalar)
    {
        return Mat44< var >
        (
            m.a00 + scalar, m.a10 + scalar, m.a20 + scalar, m.a30 + scalar,
            m.a01 + scalar, m.a11 + scalar, m.a21 + scalar, m.a31 + scalar,
            m.a02 + scalar, m.a12 + scalar, m.a22 + scalar, m.a32 + scalar,
            m.a03 + scalar, m.a13 + scalar, m.a23 + scalar, m.a33 + scalar
        );
    }


    template< typename var > Mat22< var > operator+(var scalar, const Mat22< var >& matrix)
    {
        return operator+( matrix, scalar);
    }
    template< typename var > Mat33< var > operator+(var scalar, const Mat33< var >& matrix)
    {
        return operator+( matrix, scalar);
    }
    template< typename var > Mat44< var > operator+(var scalar, const Mat44< var >& matrix)
    {
        return operator+( matrix, scalar);
    }


    template< typename var > Mat22< var > operator-(const Mat22< var >& m, var scalar)
    {
        return Mat22< var >
        (
            m.a00 - scalar, m.a10 - scalar,
            m.a01 - scalar, m.a11 - scalar
        );
    }
    template< typename var > Mat33< var > operator-(const Mat33< var >& m, var scalar)
    {
        return Mat33< var >
        (
            m.a00 - scalar, m.a10 - scalar, m.a20 - scalar,
            m.a01 - scalar, m.a11 - scalar, m.a21 - scalar,
            m.a02 - scalar, m.a12 - scalar, m.a22 - scalar
        );
    }
    template< typename var > Mat44< var > operator-(const Mat44< var >& m, var scalar)
    {
        return Mat44< var >
        (
            m.a00 - scalar, m.a10 - scalar, m.a20 - scalar, m.a30 - scalar,
            m.a01 - scalar, m.a11 - scalar, m.a21 - scalar, m.a31 - scalar,
            m.a02 - scalar, m.a12 - scalar, m.a22 - scalar, m.a32 - scalar,
            m.a03 - scalar, m.a13 - scalar, m.a23 - scalar, m.a33 - scalar
        );
    }


    template< typename var > Mat22< var > operator-(var scalar, const Mat22< var >& m)
    {
        return Mat22< var >
        (
            scalar - m.a00, scalar - m.a10,
            scalar - m.a01, scalar - m.a11
        );
    }
    template< typename var > Mat33< var > operator-(var scalar, const Mat33< var >& m)
    {
        return Mat33< var >
        (
            scalar - m.a00, scalar - m.a10, scalar - m.a20,
            scalar - m.a01, scalar - m.a11, scalar - m.a21,
            scalar - m.a02, scalar - m.a12, scalar - m.a22
        );
    }
    template< typename var > Mat44< var > operator-(var scalar, const Mat44< var >& m)
    {
        return Mat44< var >
        (
            scalar - m.a00, scalar - m.a10, scalar - m.a20, scalar - m.a30,
            scalar - m.a01, scalar - m.a11, scalar - m.a21, scalar - m.a31,
            scalar - m.a02, scalar - m.a12, scalar - m.a22, scalar - m.a32,
            scalar - m.a03, scalar - m.a13, scalar - m.a23, scalar - m.a33
        );
    }


    template< typename var > Mat22< var > operator/(const Mat22< var >& m, var scalar)
    {
        return Mat22< var >
        (
            m.a00 / scalar, m.a10 / scalar,
            m.a01 / scalar, m.a11 / scalar
        );
    }
    template< typename var > Mat33< var > operator/(const Mat33< var >& m, var scalar)
    {
        return Mat33< var >
        (
            m.a00 / scalar, m.a10 / scalar, m.a20 / scalar,
            m.a01 / scalar, m.a11 / scalar, m.a21 / scalar,
            m.a02 / scalar, m.a12 / scalar, m.a22 / scalar
        );
    }
    template< typename var > Mat44< var > operator/(const Mat44< var >& m, var scalar)
    {
        return Mat44< var >
        (
            m.a00 / scalar, m.a10 / scalar, m.a20 / scalar, m.a30 / scalar,
            m.a01 / scalar, m.a11 / scalar, m.a21 / scalar, m.a31 / scalar,
            m.a02 / scalar, m.a12 / scalar, m.a22 / scalar, m.a32 / scalar,
            m.a03 / scalar, m.a13 / scalar, m.a23 / scalar, m.a33 / scalar
        );
    }


    template< typename var > Mat22< var > operator/(var scalar, const Mat22< var >& m)
    {
        return Mat22< var >
        (
            scalar / m.a00, scalar / m.a10,
            scalar / m.a01, scalar / m.a11
        );
    }
    template< typename var > Mat33< var > operator/(var scalar, const Mat33< var >& m)
    {
        return Mat33< var >
        (
            scalar / m.a00, scalar / m.a10, scalar / m.a20,
            scalar / m.a01, scalar / m.a11, scalar / m.a21,
            scalar / m.a02, scalar / m.a12, scalar / m.a22
        );
    }
    template< typename var > Mat44< var > operator/(var scalar, const Mat44< var >& m)
    {
        return Mat44< var >
        (
            scalar / m.a00, scalar / m.a10, scalar / m.a20, scalar / m.a30,
            scalar / m.a01, scalar / m.a11, scalar / m.a21, scalar / m.a31,
            scalar / m.a02, scalar / m.a12, scalar / m.a22, scalar / m.a32,
            scalar / m.a03, scalar / m.a13, scalar / m.a23, scalar / m.a33
        );
    }








    template< typename var > inline var trace(const Mat22< var >& m)
    {
        return m[ 0 ][ 0 ] + m[ 1 ][ 1 ];
    }
    template< typename var > inline var trace(const Mat33< var >& m)
    {
        return m[ 0 ][ 0 ] + m[ 1 ][ 1 ] + m[ 2 ][ 2 ];
    }
    template< typename var > inline var trace(const Mat44< var >& m)
    {
        return m[ 0 ][ 0 ] + m[ 1 ][ 1 ] + m[ 2 ][ 2 ] + m[ 3 ][ 3 ];
    }


    template< typename var > Mat22< var > scale( var v )
    {
        return Mat22< var >
        (
            v  , 0.0,
            0.0, 1.0
        );
    }
    template< typename var > Mat33< var > scale( const Vec2< var >& v )
    {
        return Mat33< var >
        (
            v.x , 0.0f, 0.0f,
            0.0f, v.y , 0.0f,
            0.0f, 0.0f, 1.0f
        );
    }
    template< typename var > Mat44< var > scale( const Vec3< var >& v )
    {
        return Mat44< var >
        (
            v.x, 0.0, 0.0, 0.0,
            0.0, v.y, 0.0, 0.0,
            0.0, 0.0, v.z, 0.0,
            0.0, 0.0, 0.0, 1.0
        );
    }


    template< typename var > Mat22< var > diagonal( const Vec3< var >& v )
    {
        return Mat22< var >
        (
            v.x , 0.0f,
            0.0f, v.y
        );
    }
    template< typename var > Mat33< var > diagonal( const Vec3< var >& v )
    {
        return Mat33< var >
        (
            v.x , 0.0f, 0.0f,
            0.0f, v.y , 0.0f,
            0.0f, 0.0f, v.z
        );
    }
    template< typename var > Mat44< var > diagonal( const Vec4< var >& v )
    {
        return Mat44< var >
        (
            v.x , 0.0f, 0.0f, 0.0f,
            0.0f, v.y , 0.0f, 0.0f,
            0.0f, 0.0f, v.z , 0.0f,
            0.0f, 0.0f, 0.0f, v.w
        );
    }


    template< typename var > Mat22< var > upper22( const Mat33< var >& m )
    {
        return Mat22< var >
        (
            m.a00, m.a10,
            m.a01, m.a11
        );
    }
    template< typename var > Mat22< var > upper22( const Mat44< var >& m )
    {
        return Mat22< var >
        (
            m.a00, m.a10,
            m.a01, m.a11
        );
    }
    template< typename var > Mat33< var > upper33( const Mat44< var >& m )
    {
        return Mat33< var >
        (
            m.a00, m.a10, m.a20,
            m.a01, m.a11, m.a21,
            m.a02, m.a12, m.a22
        );
    }


    template< typename var > Mat22< var > transpose(const Mat22< var >& m)
    {
        return Mat22< var >
        (
            m.a00, m.a01,
            m.a10, m.a11
        );
    }
    template< typename var > Mat33< var > transpose(const Mat33< var >& m)
    {
        return Mat33< var >
        (
            m.a00, m.a01, m.a02,
            m.a10, m.a11, m.a12,
            m.a20, m.a21, m.a22
        );
    }
    template< typename var > Mat44< var > transpose(const Mat44< var >& m)
    {
        return Mat44< var >
        (
            m.a00, m.a01, m.a02, m.a03,
            m.a10, m.a11, m.a12, m.a13,
            m.a20, m.a21, m.a22, m.a23,
            m.a30, m.a31, m.a32, m.a33
            //m.a00, m.a10, m.a20, m.a30,
            //m.a01, m.a11, m.a21, m.a31,
            //m.a02, m.a12, m.a22, m.a32,
            //m.a03, m.a13, m.a23, m.a33
        );
    }


    template< typename var > Mat22< var > rot2D( var theta )
    {
        return Mat22< var >
        (
            cos(theta),-sin(theta),
            sin(theta), cos(theta)
        );
    }


    template< typename var > inline Mat33< var > rotm33X( var deg )
    {
        double sin0 = sin( deg2rad( deg ) );
        double cos0 = cos( deg2rad( deg ) );
        return Mat33< var >
        (
            1.0f, 0.0f, 0.0f,
            0.0f, cos0,-sin0,
            0.0f, sin0, cos0
        );
    }
    template< typename var > inline Mat33< var > rotm33Y( var deg )
    {
        double sin0 = sin( deg2rad( deg ) );
        double cos0 = cos( deg2rad( deg ) );
        return Mat33< var >
        (
            cos0 , 0.0, sin0,
            0.0  , 1.0,  0.0,
            -sin0, 0.0, cos0
        );
    }
    template< typename var > inline Mat33< var > rotm33Z( var deg )
    {
        double sin0 = sin( deg2rad( deg ) );
        double cos0 = cos( deg2rad( deg ) );
        return Mat33< var >
        (
            cos0,-sin0, 0.0f,
            sin0, cos0, 0.0f,
            0.0f, 0.0f, 1.0f
        );
    }


    template< typename var > inline Mat44< var > rotm44X( var deg )
    {
        double sin0 = sin( deg2rad( deg ) );
        double cos0 = cos( deg2rad( deg ) );
        return Mat44< var >
        (
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, cos0,-sin0, 0.0f,
            0.0f, sin0, cos0, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }
    template< typename var > inline Mat44< var > rotm44Y( var deg )
    {
        double sin0 = sin( deg2rad( deg ) );
        double cos0 = cos( deg2rad( deg ) );
        return Mat44< var >
        (
            cos0 , 0.0, sin0, 0.0,
            0.0  , 1.0,  0.0, 0.0,
            -sin0, 0.0, cos0, 0.0,
            0.0  , 0.0,  0.0, 1.0
        );
    }
    template< typename var > inline Mat44< var > rotm44Z( var deg )
    {
        double sin0 = sin( deg2rad( deg ) );
        double cos0 = cos( deg2rad( deg ) );
        return Mat44< var >
        (
            cos0,-sin0, 0.0f, 0.0f,
            sin0, cos0, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }


    template< typename var > inline Mat33< var > translate( const Vec2< var >& v )
    {
        return Mat33< var >
        (
            1.f, 0.f, 0.f,
            0.f, 1.f, 0.f,
            v.x, v.y, 0.f
        );
    }
    
    template< typename var > inline Mat44< var > translate( var tx, var ty, var tz )
    {
        return Mat44< var >
        (
            1.0, 0.0, 0.0, tx,
            0.0, 1.0, 0.0, ty,
            0.0, 0.0, 1.0, tz,
            0.0, 0.0, 0.0, 1.0
        );
    }
    template< typename var > inline Mat44< var > translate( const Vec3< var >& t )
    {
        return translate< var >( t.x, t.y, t.z );
    }

    // Function to create a look-at matrix
    template< typename var > Mat44< var > lookAt(const Vec3< var >& eye, const Vec3< var >& target, const Vec3< var >& up)
    {
        Mat44< var > result;

        // Compute the forward, right, and up vectors
        Vec3< var > forward = normalize(target - eye);
        Vec3< var > right = normalize( cross( forward, normalize(up)) );
        Vec3< var > newUp = cross( right, forward);

        // Set the rotation part of the matrix
        result[0][0] = right.x;
        result[1][0] = right.y;
        result[2][0] = right.z;

        result[0][1] = newUp.x;
        result[1][1] = newUp.y;
        result[2][1] = newUp.z;

        result[0][2] = -forward.x;
        result[1][2] = -forward.y;
        result[2][2] = -forward.z;

        // Set the translation part of the matrix
        result[3][0] =  -right.x * eye.x -   right.y * eye.y -   right.z * eye.z;
        result[3][1] =  -newUp.x * eye.x -   newUp.y * eye.y -   newUp.z * eye.z;
        result[3][2] = forward.x * eye.x + forward.y * eye.y + forward.z * eye.z;

        return result;
    }
    
    /**
     build perspective projection matrix
     @return mat44
     */
    template< typename var > inline Mat44< var > perspproj(var l, var r, var b, var t, var n, var f)
    {
        var n2 = 2 * n;
        var rl = r - l;
        var tb = t - b;
        var fn = f - n;
//        Mat44< var > m;
//        m.a00 = n2 / rl;
//        m.a11 = n2 / tb;
//        m.a20 = (r+l)/rl;
//        m.a21 = (t+b)/tb;
//        m.a22 = -(f+n)/fn;
//        m.a23 = -1;
//        m.a33 = 1;
//        return m;
        return Mat44< var >
        (
            n2 / rl, 0.0    ,  (r+l)/rl, 0.0,
            var( 0 ), n2 / tb,  (t+b)/tb, 0.0,
            0.0    , 0.0    , -(f+n)/fn, -(n2*f)/fn,
            0.0    , 0.0    , -1       , 1.0
        );
//        return Mat44< var >
//        (   
//            n2 / rl, 0.0    ,  0.0, 0.0,
//            0.0    , n2 / tb,  0.0, 0.0,
//            (r+l)/rl, (t+b)/tb, -(f+n)/fn, -1,
//            0.0    , 0.0    , -(n2*f)/fn       , 1.0
//        );
    }

    /** @brief build perspective projection matrix */
    template< typename var > Mat44< var > perspproj(var fov, var aspect, var znear, var zfar)
    {
        var ymax = znear * tan(deg2rad(fov/2.0));
        var xmax = ymax * aspect;
        return perspproj(-xmax, xmax, -ymax, ymax, znear, zfar);
    }

    /** @brief build orthographic projection matrix */
    template< typename var > inline Mat44< var > orthoproj(var l, var r, var t, var b, var n, var f)
    {
        return Mat44< var >
        (   
            2.0 / (r-l), 0.0        ,  0.0        , -(r+l)/(r-l),
            0.0        , 2.0 / (t-b),  0.0        , -(t+b)/(t-b),
            0.0        , 0.0        , -2.0 / (f-n), -(f+n)/(f-n),
            0.0        , 0.0        ,  0.0f       , 1.0
        );
    }



    template< typename var > Vec3< var > getEulerAngles( const Mat44< var >& m )
    {
        //check for gimbal lock
        if (KEGE_EPSILON_F > abs(m[ 0 ][ 2 ] - (-1.0f)))
        {
            var x = 0; //gimbal lock, value of x doesn't matter
            var y = _PI_ / 2;
            var z = x + atan2(m[ 1 ][ 0 ], m[ 2 ][ 0 ]);
            return { x, y, z };
        }
        else if (KEGE_EPSILON_F > abs(m[ 0 ][ 2 ] - 1.0f))
        {
            var x = 0;
            var y = -_PI_ / 2;
            var z = -x + atan2(-m[ 1 ][ 0 ], -m[ 2 ][ 0 ]);
            return { x, y, z };
        }
        else
        { //two solutions exist
            var x1  = -asin(m[ 0 ][ 2 ]);
            var x2  = _PI_ - x1;

            var cx1 = cos(x2);
            var cx2 = cos(x2);

            var y1 = atan2(m[ 1 ][ 2 ] / cx1, m[ 2 ][ 2 ] / cx1);
            var y2 = atan2(m[ 1 ][ 2 ] / cx2, m[ 2 ][ 2 ] / cx2);
            var z1 = atan2(m[ 0 ][ 1 ] / cx1, m[ 0 ][ 0 ] / cx1);
            var z2 = atan2(m[ 0 ][ 1 ] / cx2, m[ 0 ][ 0 ] / cx2);

            //choose one solution to return
            //for example the "shortest" rotation
            if ((abs(x1) + abs(y1) + abs(z1)) <=
                (abs(x2) + abs(y2) + abs(z2)))
            {
                return Vec3< var >( x1, y1, z1 );
            }
            else
            {
                return Vec3< var >( x2, y2, z2 );
            }
        }
    }



    /** @brief Determinant of 2x2 matrix components */
    template< typename var > inline var det
    (
        var m0, var m1,
        var m2, var m3
    )
    {
        return m0 * m3 - m1 * m2;
    }

    /** @brief Determinant of 3x3 matrix components */
    template< typename var > inline var det
    (
        var m0, var m1, var m2,
        var m3, var m4, var m5,
        var m6, var m7, var m8
    )
    {
        return m0 * det< var >( m4, m5, m7, m8 )
             - m1 * det< var >( m3, m5, m6, m8 )
             + m2 * det< var >( m3, m4, m6, m7 );
    }

    /** @brief Determinant of 4x4 matrix components */
    template< typename var > inline var det
    (
        var m00, var m01, var m02, var m03,
        var m04, var m05, var m06, var m07,
        var m08, var m09, var m10, var m11,
        var m12, var m13, var m14, var m15
    )
    {
        return
            m00 * det< var >( m05, m06, m07,  m09, m10, m11,  m13, m14, m15 ) -
            m01 * det< var >( m04, m06, m07,  m08, m10, m11,  m12, m14, m15 ) +
            m02 * det< var >( m04, m05, m07,  m08, m09, m11,  m12, m13, m15 ) -
            m03 * det< var >( m04, m05, m06,  m08, m09, m10,  m12, m13, m14 );
    }

    /** @brief Determinant of 3x3 matrix */
    template< typename var >  inline var det (const Mat22< var >& m)
    {
        return det< var >
        (
            m.a00, m.a01,
            m.a10, m.a11
        );
    }

    /** @brief Determinant of 3x3 matrix */
    template< typename var >  inline var det (const Mat33< var >& m)
    {
        return det< var >
        (
            m.a00, m.a01, m.a02,
            m.a10, m.a11, m.a12,
            m.a20, m.a21, m.a22
        );
    }

    /** @brief Determinant of 4x4 matrix */
    template< typename var > inline var det( const Mat44< var >& m )
    {
        return det< var >
        (
            m.a[ 0], m.a[ 1], m.a[ 2], m.a[ 3],
            m.a[ 4], m.a[ 5], m.a[ 6], m.a[ 7],
            m.a[ 8], m.a[ 9], m.a[10], m.a[11],
            m.a[12], m.a[13], m.a[14], m.a[15]
        );
    }


    /** @brief Cofactor matrix of 3x3 matrix */
    template< typename var > inline Mat33< var > cof( const Mat33< var >& m )
    {
        return Mat33< var >
        (
             det< var >( m.a[4], m.a[5], m.a[7],  m.a[8] ), // m00
            -det< var >( m.a[1], m.a[2], m.a[7],  m.a[8] ), // m10
             det< var >( m.a[1], m.a[2], m.a[4],  m.a[5] ), // m20

            -det< var >( m.a[3], m.a[5], m.a[6],  m.a[8] ), // m01
             det< var >( m.a[0], m.a[2], m.a[6],  m.a[8] ), // m11
            -det< var >( m.a[0], m.a[2], m.a[3],  m.a[5] ), // m21

             det< var >( m.a[3], m.a[4], m.a[6],  m.a[7] ), // m02
            -det< var >( m.a[0], m.a[1], m.a[6],  m.a[7] ), // m12
             det< var >( m.a[0], m.a[1], m.a[3],  m.a[4] )  // m22
        );
    }

    /** @brief Cofactor matrix of 4x4 matrix */
    template< typename var > inline Mat44< var > cof( const Mat44< var >& m )
    {
        return Mat44< var >
        (
             det< var >( m.a[ 5], m.a[ 6], m.a[ 7],  m.a[ 9], m.a[10], m.a[11],  m.a[13], m.a[14], m.a[15] ), // m00
            -det< var >( m.a[ 1], m.a[ 2], m.a[ 3],  m.a[ 9], m.a[10], m.a[11],  m.a[13], m.a[14], m.a[15] ), // m10
             det< var >( m.a[ 1], m.a[ 2], m.a[ 3],  m.a[ 5], m.a[ 6], m.a[ 7],  m.a[13], m.a[14], m.a[15] ), // m20
            -det< var >( m.a[ 1], m.a[ 2], m.a[ 3],  m.a[ 5], m.a[ 6], m.a[ 7],  m.a[ 9], m.a[10], m.a[11] ), // m30

            -det< var >( m.a[ 4], m.a[ 6], m.a[ 7],  m.a[ 8], m.a[10], m.a[11],  m.a[12], m.a[14], m.a[15] ), // m01
             det< var >( m.a[ 0], m.a[ 2], m.a[ 3],  m.a[ 8], m.a[10], m.a[11],  m.a[12], m.a[14], m.a[15] ), // m11
            -det< var >( m.a[ 0], m.a[ 2], m.a[ 3],  m.a[ 4], m.a[ 6], m.a[ 7],  m.a[12], m.a[14], m.a[15] ), // m21
             det< var >( m.a[ 0], m.a[ 2], m.a[ 3],  m.a[ 4], m.a[ 6], m.a[ 7],  m.a[ 8], m.a[10], m.a[11] ), // m31

             det< var >( m.a[ 4], m.a[ 5], m.a[ 7],  m.a[ 8], m.a[ 9], m.a[11],  m.a[12], m.a[13], m.a[15] ), // m02
            -det< var >( m.a[ 0], m.a[ 1], m.a[ 3],  m.a[ 8], m.a[ 9], m.a[11],  m.a[12], m.a[13], m.a[15] ), // m12
             det< var >( m.a[ 0], m.a[ 1], m.a[ 3],  m.a[ 4], m.a[ 5], m.a[ 7],  m.a[12], m.a[13], m.a[15] ), // m22
            -det< var >( m.a[ 0], m.a[ 1], m.a[ 3],  m.a[ 4], m.a[ 5], m.a[ 7],  m.a[ 8], m.a[ 9], m.a[11] ), // m32

            -det< var >( m.a[ 4], m.a[ 5], m.a[ 6],  m.a[ 8], m.a[ 9], m.a[10],  m.a[12], m.a[13], m.a[14] ), // m03
             det< var >( m.a[ 0], m.a[ 1], m.a[ 2],  m.a[ 8], m.a[ 9], m.a[10],  m.a[12], m.a[13], m.a[14] ), // m13
            -det< var >( m.a[ 0], m.a[ 1], m.a[ 2],  m.a[ 4], m.a[ 5], m.a[ 6],  m.a[12], m.a[13], m.a[14] ), // m23
             det< var >( m.a[ 0], m.a[ 1], m.a[ 2],  m.a[ 4], m.a[ 5], m.a[ 6],  m.a[ 8], m.a[ 9], m.a[10] )  // m33
        );
    }



    /** @brief Adjoint of 4x4 matrix */
    template< typename var >  Mat33< var > adj (const Mat33< var >& m)
    {
        return transpose< var >( cof< var >( m ) );
    }

    /** @brief Adjoint of 4x4 matrix */
    template< typename var > inline Mat44< var > adj( const Mat44< var >& m )
    {
        return transpose< var >( cof< var >( m ) );
    }



    /** @brief Inverse of 3x3 matrix */
    template< typename var >  inline Mat33< var > inverse(const Mat33< var >& m)
    {
        return adj(m) / det(m);
    }

    /** @brief Inverse of 4x4 matrix */
    template< typename var > inline Mat44< var > inverse(const Mat44< var >& m)
    {
        return adj(m) / det(m);
    }


    template< typename var > inline std::ostream& operator <<(std::ostream& os, const Mat22< var >& m )
    {
        std::cout << "\n{\n";
        std::cout <<std::setw(5) << m[0]  <<"\n";
        std::cout <<std::setw(5) << m[1]  <<"\n";
        std::cout <<"}\n";
        return os;
    }
    template< typename var > inline std::ostream& operator <<(std::ostream& os, const Mat33< var >& m )
    {
        std::cout << "\n{\n";
        std::cout <<std::setw(5) << m[0]  <<"\n";
        std::cout <<std::setw(5) << m[1]  <<"\n";
        std::cout <<std::setw(5) << m[2]  <<"\n";
        std::cout <<"}\n";
        return os;
    }
    template< typename var > inline std::ostream& operator <<(std::ostream& os, const Mat44< var >& m )
    {
        std::cout << "\n{\n";
        std::cout <<std::setw(5) << m[0]  <<"\n";
        std::cout <<std::setw(5) << m[1]  <<"\n";
        std::cout <<std::setw(5) << m[2]  <<"\n";
        std::cout <<std::setw(5) << m[3]  <<"\n";
        std::cout <<"}\n";
        return os;
    }

}
#endif /* matrices_hpp */


//
//  transform.hpp
//  kege
//
//  Created by Kenneth Esdaile on 11/20/24.
//

#ifndef transform_hpp
#define transform_hpp

#include "vectors.hpp"
#include "quaternion.hpp"

namespace kege
{

    template< typename T > struct Transf
    {
        kege::Vec3< T > position;
        kege::Quat< T > orientation;
        kege::Vec3< T > scale = {T(1),T(1),T(1)};
    };

    typedef Transf< float  > Transform32;
    typedef Transf< double > Transform64;
    typedef Transform32 Transform;



    template< typename T > inline Transf< T > operator /(const Transf< T >& b, T s)
    {
        return{ b.position / s, b.orientation / s, b.scale / s };
    }
    template< typename T > inline Transf< T > operator /(T s, const Transf< T >& b)
    {
        return{ s / b.position, s / b.orientation, s/ b.scale };
    }

    
    template< typename T > inline Transf< T > operator +(const Transf< T >& l, const Transf< T >& r)
    {
        return
        {
            l.position + r.position,
            l.orientation + r.orientation,
            l.scale + r.scale
        };
    }
    template< typename T > inline Transf< T > operator -(const Transf< T >& l, const Transf< T >& r)
    {
        return
        {
            l.position - r.position,
            l.orientation - r.orientation,
            l.scale - r.scale
        };
    }


    template< typename T > inline Transf< T > operator *(const Transf< T >& l, const Transf< T >& r)
    {
        return
        {
            l.position + rotate( l.orientation, l.scale * r.position),
            l.orientation * r.orientation,
            l.scale * r.scale
        };
    }


    template< typename T > inline Transf< T > operator *(const Transf< T >& b, T s)
    {
        return{ b.position * s, b.orientation * s, b.scale * s };
    }
    template< typename T > inline Transf< T > operator *(T s, const Transf< T >& b)
    {
        return operator *(b, s);
    }


    template< typename T > inline kege::vec3 operator * (const Transf< T >& t, const kege::vec3& point)
    {
        return t.position + kege::rotate( t.orientation, t.scale * point);
    }


    template< typename T > inline Transf< T > operator / (const Transf< T >& l, const Transf< T >& r)
    {
        kege::quat q = kege::conjugate( r.orientation );
        return
        {
            rotate( q, l.position - r.orientation) / l.scale,
            l.orientation * r.orientation,
            rotate( q, l.scale / r.scale )
        };
    }


    template< typename T > inline Transf< T > inverse( const Transf< T >& t )
    {
        return
        {
            kege::rotate( t.orientation, -t.position) / t.scale,
            kege::conjugate( t.orientation ),
            kege::rotate( t.orientation, kege::vec3(1.f, 1.f, 1.f) / t.scale)
        };
    }

    template< typename T > inline std::ostream& operator <<(std::ostream& os, const Transf< T >& t )
    {
        os << "{\n";
        os <<std::setw(8) << "   rotation : [ " <<std::setw(10) << t.orientation.x <<", " <<std::setw(10) << t.orientation.y <<", " <<std::setw(10) << t.orientation.z <<", " <<std::setw(10) << t.orientation.z <<" ]\n";
        os <<std::setw(8) << "   position : [ " <<std::setw(10) << t.position.x <<", " <<std::setw(10) << t.position.y <<", " <<std::setw(10) << t.position.z <<", " <<std::setw(13) <<" ]\n";
        os <<std::setw(8) << "   scale    : [ " <<std::setw(10) << t.scale.x    <<", " <<std::setw(10) << t.scale.y    <<", " <<std::setw(10) << t.scale.z    <<", " <<std::setw(13) <<" ]\n";
        os << "}\n";
        return os;
    }
}
#endif /* transform_hpp */

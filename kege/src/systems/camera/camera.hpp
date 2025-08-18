//
//  camera.hpp
//  kege
//
//  Created by Kenneth Esdaile on 12/12/24.
//

#ifndef camera_hpp
#define camera_hpp

#include "../../core/memory/ref.hpp"
#include "../../core/math/algebra/vmath.hpp"

namespace kege{

    struct Projection : public kege::RefCounter
    {
        virtual kege::mat44 get()const{ return kege::mat44(); }

        enum Type{ PERSPECTIVE, ORTHOGRAPHIC };
        Projection( Type t ): type( t ) {}

        Type type;
    };

    struct Perspective : public Projection
    {
        kege::mat44 get()const
        {
            return kege::perspproj< float >
            (
                fov,
                aspect_ratio,
                znear,
                zfar
            );
        }

        Perspective
        (
            double      aspect_ratio,
            double      fov,
            double      znear,
            double      zfar
        )
        :   Projection( PERSPECTIVE )
        ,   aspect_ratio( aspect_ratio )
        ,   fov( fov )
        ,   znear( znear )
        ,   zfar( zfar )
        {}

        Perspective()
        :   Projection( PERSPECTIVE )
        ,   aspect_ratio( 0.0 )
        ,   fov( 0.0 )
        ,   znear( 0.0 )
        ,   zfar( 0.0 )
        {}

        double      aspect_ratio;
        double      fov;
        double      znear;
        double      zfar;
    };

    struct Orthographic : public Projection
    {
        kege::mat44 get()const
        {
            return kege::orthoproj< float >
            (
                left,
                right,
                below,
                above,
                znear,
                zfar
            );
        }

        Orthographic
        (
            double left,
            double right,
            double below,
            double above,
            double znear,
            double zfar
        )
        :   Projection( ORTHOGRAPHIC )
        ,   left( left )
        ,   right( right )
        ,   above( above )
        ,   below( below )
        ,   znear( znear )
        ,   zfar( zfar )
        {}

        Orthographic()
        :   Projection( ORTHOGRAPHIC )
        ,   left( 0.0 )
        ,   right( 0.0 )
        ,   above( 0.0 )
        ,   below( 0.0 )
        ,   znear( 0.0 )
        ,   zfar( 0.0 )
        {}

        double left;
        double right;
        double above;
        double below;
        double znear;
        double zfar;
    };

    struct CameraData
    {
        mat44 projection;
        mat44 transform;
        vec3  position;
    };

    struct Camera
    {
        kege::Ref< Projection > projection;
        CameraData matrices;
        bool modified = true;
    };

    struct CameraControls
    {
        kege::vec3 angles = {0.f, 0.f, 0.f};
        kege::vec3 euler = {0.f, 0.f, 0.f};
        float sensitivity = 0.125f;
        float smoothness = 10.0f;
    };

}
#endif /* camera_hpp */

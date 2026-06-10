//
//  camera.hpp
//  kege
//
//  Created by Kenneth Esdaile on 12/12/24.
//

#ifndef camera_hpp
#define camera_hpp

#include "../../../math/algebra/vmath.hpp"

namespace kege{

    struct Perspective// : public Projection
    {
        kege::mat44 get()const
        {
            return kege::perspproj< float >
            (
                aspect_ratio,
                fov,
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
        :   aspect_ratio( aspect_ratio )
        ,   fov( fov )
        ,   znear( znear )
        ,   zfar( zfar )
        {}

        Perspective()
        :   aspect_ratio( 1.0 )
        ,   fov( 45.0 )
        ,   znear( 0.1 )
        ,   zfar( 1000.0 )
        {}

        double      aspect_ratio;
        double      fov;
        double      znear;
        double      zfar;
    };

    struct Orthographic// : public Projection
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
        :   left( left )
        ,   right( right )
        ,   above( above )
        ,   below( below )
        ,   znear( znear )
        ,   zfar( zfar )
        {}

        Orthographic()
        :   left( 0.0 )
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
        vec4  position;
        vec4  screen_info;
    };

    struct Camera
    {
        CameraData matrices;
        bool modified = true;
    };

    struct CameraControls
    {
        kege::vec3 angles = {0.f, 0.f, 0.f};
        kege::vec3 euler = {0.f, 0.f, 0.f};
        float sensitivity = 0.125f;
        float stiffness = 10.0f;
    };

}
#endif /* camera_hpp */

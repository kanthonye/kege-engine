//
//  landscape-layer.hpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 12/28/24.
//

#ifndef landscape_layer_hpp
#define landscape_layer_hpp

//#include "terrain-topography.hpp"
//#include "image-layer-manager.hpp"
//#include "flat-landscape-patch.hpp"
#include "graphics.h"

namespace kege{

    class Landscape;

    class LandscapeLayer : public kege::RefCounter
    {
    public:

        virtual void beginRender( kege::CommandBuffer& command_buffer ){}
        virtual void endRender( kege::CommandBuffer& command_buffer ){}
        virtual void render( kege::CommandBuffer& command_buffer ){}

        virtual void update( const kege::dvec3& eye ){}
        virtual void notify( int event, void* data ){}

        virtual bool initialize(){return _init;}

        LandscapeLayer(): _landscape( nullptr ), _init(false) {}
        virtual ~LandscapeLayer(){}

    protected:

        Landscape* _landscape;
        friend Landscape;
        bool _init;
    };

}

#endif /* landscape_layer_hpp */

//
//  material.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/21/25.
//

#ifndef material_hpp
#define material_hpp

#include <cstdint>
#include "../../memory/ref.hpp"
#include "../render/core/graphics.hpp"
#include "../render/graph/render-stage.hpp"

namespace kege{

    struct MaterialSource : public kege::RefCounter
    {
        MaterialSource
        (
            RenderPassType pass,
            const kege::ref::ShaderPipeline& p,
            bool receive_shadows,
            bool casts_shadows,
            ref::ShaderSet resource
        )
        :   pipeline(p)
        ,   receive_shadows( receive_shadows )
        ,   casts_shadows( casts_shadows )
        ,   resource( resource )
        ,   pass( pass )
        {}

        MaterialSource
        (
            RenderPassType pass,
            const kege::ref::ShaderPipeline& p,
            bool receive_shadows,
            bool casts_shadows
        )
        :   pipeline(p)
        ,   receive_shadows( receive_shadows )
        ,   casts_shadows( casts_shadows )
        ,   pass( pass )
        {}

        MaterialSource()
        {}

        //std::vector< ShaderResource > resources;
        //std::vector< RenderPassType > pass;

        ref::ShaderSet resource;
        kege::ref::ShaderPipeline pipeline;
        kege::RenderPassType pass;

        bool receive_shadows = true;
        bool casts_shadows = true;
    };


    struct Material : public kege::RefCounter
    {
        Material( const std::vector< Ref< MaterialSource > >& s ): sources(s) {}
        Material(){}

        std::vector< Ref< MaterialSource > > sources;
    };
    


    struct PBRProtperties
    {
        vec4  albedo;        // Base color (RGBA)
        float roughness;    // Roughness (0 = smooth, 1 = rough)
        float metallic;     // Metallic (0 = dielectric, 1 = metallic)
        float reflectance;  // Reflectance (F0 for dielectrics)
        float ao;           // Ambient occlusion property
    };

    struct PBRTextures
    {
        ref::Image albedo;
        ref::Image normal;
        ref::Image roughness;
        ref::Image metallic;
        ref::Image ambient_occlusion;
        ref::Image displacement;
    };

    struct PBRMaterialSource : public kege::MaterialSource
    {
        void operator()( kege::Graphics* graphics );

        PBRProtperties properties;
        PBRTextures textures;
    };

}

#endif /* material_hpp */

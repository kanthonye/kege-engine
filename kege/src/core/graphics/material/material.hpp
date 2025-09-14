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
#include "../../renderer/core/graphics.hpp"
#include "../render-graph/render-pass.hpp"

namespace kege{

    struct MaterialSource : public kege::RefCounter
    {
        std::vector< ShaderResource > descriptors;
        std::vector< RenderPassType > pass;

        PipelineHandle pipeline;
        
        bool receive_shadows = true;
        bool casts_shadows = true;
    };


    struct Material : public kege::RefCounter
    {
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
        kege::ImageHandle albedo;
        kege::ImageHandle normal;
        kege::ImageHandle roughness;
        kege::ImageHandle metallic;
        kege::ImageHandle ambient_occlusion;
        kege::ImageHandle displacement;
    };

    struct PBRMaterialSource : public kege::MaterialSource
    {
        void operator()( kege::Graphics* graphics );

        PBRProtperties properties;
        PBRTextures textures;
    };

}

#endif /* material_hpp */

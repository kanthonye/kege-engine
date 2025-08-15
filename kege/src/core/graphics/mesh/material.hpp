//
//  material.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/21/25.
//

#ifndef material_hpp
#define material_hpp

#include <cstdint>
#include "ref.hpp"
#include "graphics.hpp"

namespace kege{

    struct Material : public kege::RefCounter
    {
        kege::DescriptorSetHandle shader_resource;
        uint32_t pipeline;
        uint32_t layer;         // the render layer
        uint32_t pass;          // the render pass
    };

    struct PBRMaterial : public kege::Material
    {
        struct Protperties
        {
            vec4  albedo;        // Base color (RGBA)
            float roughness;    // Roughness (0 = smooth, 1 = rough)
            float metallic;     // Metallic (0 = dielectric, 1 = metallic)
            float reflectance;  // Reflectance (F0 for dielectrics)
            float ao;           // Ambient occlusion property
        };

        struct Textures
        {
            kege::ImageHandle albedo;
            kege::ImageHandle normal;
            kege::ImageHandle roughness;
            kege::ImageHandle metallic;
            kege::ImageHandle ambient_occlusion;
            kege::ImageHandle displacement;
        };

        Protperties properties;
        Textures textures;
    };

}

#endif /* material_hpp */

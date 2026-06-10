//
//  material.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/21/25.
//

#ifndef material_hpp
#define material_hpp

#include <cstdint>
#include "../../../memory/ref.hpp"
#include "../../core/device/core/device.hpp"

namespace kege{

    enum MaterialFeature : uint64_t
    {
        FEATURE_NORMAL_MAP   = 1 << 0,
        FEATURE_EMISSIVE_MAP = 1 << 1,
        FEATURE_CLEAR_COAT   = 1 << 2,
        FEATURE_TRANSMISSION = 1 << 3,
        FEATURE_SSS          = 1 << 4,
        FEATURE_ANISOTROPY   = 1 << 5,
        FEATURE_ALPHA_TEST   = 1 << 6,
    };

    /**
     * Core Material (ALWAYS PRESENT).
     * This is the “guaranteed data” every shader can rely on.
     */
    struct MaterialParams
    {
        kege::vec4 albedo;     // rgb + alpha
        kege::vec3 emissive;

        float metallic;
        float roughness;
        float ao;

        float alpha_cutoff;

        uint32_t model;      // enum MaterialModel
        uint64_t features;   // bitmask
    };

    struct TextureRef
    {
        int texture_id;   // index into bindless array
        int uv_set;       // 0,1,...
        int channel;      // or mask (your choice)
        int padding;      // alignment
    };

    struct MaterialTextures
    {
        TextureRef albedo;
        TextureRef normal;
        TextureRef mra;       // metallic-roughness-ao packed
        TextureRef emissive;
    };

    /**
     * Optional Feature Block.
     * Only meaningful when enabled via features.
     */
    struct MaterialExtensions
    {
        // clear coat
        float clear_coat;
        float clear_coat_roughness;

        // transmission
        float transmission;
        float ior;

        // subsurface
        kege::vec3 subsurface_color;
        float subsurface_thickness;

        // sheen
        kege::vec3 sheen_color;
        float sheen_roughness;

        // anisotropy
        float anisotropy;
    };

    struct Material : public kege::RefCounter
    {
        MaterialParams params;
        MaterialTextures textures;
        MaterialExtensions ext;
        uint64_t pipeline_handle = 0;
        uint32_t id = 0;
    };




//    class Material : public kege::ShaderData
//    {
//    public:
//
//        struct Parameters
//        {
//            vec4  albedo;       // Base color (RGBA)
//            float roughness;    // Roughness (0 = smooth, 1 = rough)
//            float metallic;     // Metallic (0 = dielectric, 1 = metallic)
//            float reflectance;  // Reflectance (F0 for dielectrics)
//            float ao;           // Ambient occlusion property
//        };
//
//        struct Textures
//        {
//            ref::Image albedo;
//            ref::Image normal;
//            ref::Image roughness;
//            ref::Image metallic;
//            ref::Image ambient_occlusion;
//        };
//
//    public:
//
//        const ref::ShaderPipeline& getShaderPipeline()const;
//        kege::RenderPassType getPass()const;
//        
//        Material
//        (
//            kege::RenderPassType pass,
//            const ref::ShaderPipeline& pipeline,
//            const SetNames& binding_names
//        );
//
//    private:
//
//        kege::Material::Parameters _parameters;
//        kege::Material::Textures _textures;
//        kege::ref::ShaderPipeline _pipeline;
//        kege::RenderPassType _pass;
//    };

}

namespace kege::ref{
    using Material = kege::Ref< kege::Material >;
}

namespace kege{

    using Materials = std::vector< kege::ref::Material >;

    struct MaterialSet : public kege::RefCounter
    {
        Materials materials;
    };

}
#endif /* material_hpp */

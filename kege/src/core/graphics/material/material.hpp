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
#include "../render/pipeline/shader-layout.hpp"

namespace kege{

    class Material : public kege::ShaderData
    {
    public:

        struct Parameters
        {
            vec4  albedo;       // Base color (RGBA)
            float roughness;    // Roughness (0 = smooth, 1 = rough)
            float metallic;     // Metallic (0 = dielectric, 1 = metallic)
            float reflectance;  // Reflectance (F0 for dielectrics)
            float ao;           // Ambient occlusion property
        };

        struct Textures
        {
            ref::Image albedo;
            ref::Image normal;
            ref::Image roughness;
            ref::Image metallic;
            ref::Image ambient_occlusion;
        };

    public:

        const ref::ShaderPipeline& getShaderPipeline()const;
        kege::RenderPassType getPass()const;
        Material
        (
            kege::RenderPassType pass,
            const ref::ShaderPipeline& pipeline,
            const SetNames& binding_names
        );

    private:

        kege::Material::Parameters _parameters;
        kege::Material::Textures _textures;
        kege::ref::ShaderPipeline _pipeline;
        kege::RenderPassType _pass;
    };

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



#ifndef kege_pipeline_key_hpp
#define kege_pipeline_key_hpp

#include "blend-state.hpp"
#include "depth-state.hpp"
#include "dynamic-state.hpp"
#include "rasterizer-state.hpp"
#include "feature-flags.hpp"
#include "../core/vertex-format.hpp"

namespace kege{

    enum class ShadingModel : uint8_t
    {
        Unlit = 0,          // Color = BaseColor (no lighting)
        LitPBR_Metallic,    // Metallic-Roughness
        LitPBR_Clearcoat,   // PBR with clearcoat layer (car paint)
        LitPBR_Subsurface,  // PBR with subsurface scattering
        LitPBR_Cloth,       // Fabric-specific BRDF
        LitPBR_Hair,        // Hair-specific BSDF (Marschner/Kajiya-Kay)
        LitPBR_Skin,        // Multi-layer skin model
        Toon_Cel,           // Stylized cel-shading
        Toon_UnlitOutline,  // Outline + unlit fill
        Emissive,           // Self-illuminating
        Skybox,             // Cube/equirectangular mapping
        DepthOnly,
        // etc.
    };

    enum class MeshType
    {
        Point,      //
        ScreenRect,      //
        StaticMesh,      // Standard mesh rendering
        //SkinnedMesh,   // Animated characters
        Particle,        // GPU particles, billboards
        Terrain,         // Heightmap, LOD chunks
        Atmosphere,      // Volumetric, raymarched
        Ocean,           // FFT waves, tessellation
        Foliage,         // GPU instancing, wind
        GUI,
        FlatTerrain,
        SphericalTerrain,
        // etc.
    };

    // These define when/how things render
    enum class RenderPass
    {
        DepthPrePass,      // All opaque geometry
        GBuffer,           // Deferred geometry
        Lighting,          // Deferred lighting
        Shadow,            // Shadow maps
        Forward,           // Forward opaque
        Sky,               // Sky/atmosphere
        VolumetricFog,     // Volumetric effects
        Transparent,       // Sorted transparency
        Particles,         // Particle systems
        PostProcess,       // Screen-space effects
        UI,                // 2D overlay
        Debug,
    };

    struct PipelineKey
    {
        // Shader selection
        ShadingModel shading_model;
        MeshType renderer_type;
        RenderPass render_pass;
        FeaturesBitmask features = 0;

        VertexLayout vertex_layout;
        
        // Fixed-function states (CRITICAL for pipeline caching!)
        DepthStencil depth_state;
        RasterizerState raster_state;
        BlendState blend_state;

        // Additional render states
        PrimitiveTopology topology;     // TriangleList, TriangleStrip, LineList, etc.
        uint32_t sample_count;          // MSAA samples (1, 2, 4, 8)
        bool alpha_to_coverage_enable;  // For foliage transparency

        VertexSignature vertex_signature;

        bool operator==(const PipelineKey& other) const
        {
            return memcmp(this, &other, sizeof(PipelineKey)) == 0;
        }
    };

    // Predefined state combinations
    struct PipelineKeyPresets
    {
        // Opaque PBR object
        static PipelineKey opaquePBR()
        {
            return
            {
                //.shading_model = ShadingModel::LitPBR_Metallic,
                .renderer_type = MeshType::StaticMesh,
                .render_pass = RenderPass::GBuffer,
                .blend_state = BlendStatePreset::opaque(),
                .depth_state = DepthStencilPresets::depthTestWrite(),
                .raster_state = RasterizerStatePresets::cullBack(),
                .topology = PrimitiveTopology::TriangleList
            };
        }

        // Transparent object
        static PipelineKey transparentAlphaBlend()
        {
            return
            {
                //.shading_model = ShadingModel::LitPBR_Metallic,
                .renderer_type = MeshType::StaticMesh,
                .render_pass = RenderPass::Transparent,
                .blend_state = BlendStatePreset::alphaBlend(),
                .depth_state = DepthStencilPresets::depthTestNoWrite(), // Test but don't write!
                .raster_state = RasterizerStatePresets::cullBack(),
                .topology = PrimitiveTopology::TriangleList
            };
        }

        // Particle system
        static PipelineKey particleAdditive()
        {
            return
            {
                //.shading_model = ShadingModel::Unlit,
                .renderer_type = MeshType::Particle,
                .render_pass = RenderPass::Transparent,
                .blend_state = BlendStatePreset::additive(),
                .depth_state = DepthStencilPresets::depthTestNoWrite(),
                .raster_state = RasterizerStatePresets::cullNone(), // Particles are usually double-sided
                .topology = PrimitiveTopology::TriangleStrip // Or PointList for GPU particles
            };
        }

        // Shadow caster
        static PipelineKey shadowCaster()
        {
            return
            {
                //.shading_model = ShadingModel::DepthOnly, // Special shading model
                .renderer_type = MeshType::StaticMesh,
                .render_pass = RenderPass::Shadow,
                .blend_state = BlendStatePreset::opaque(),
                .depth_state = DepthStencilPresets::depthTestWrite(),
                .raster_state = RasterizerStatePresets::shadowCaster(), // With depth bias!
                .topology = PrimitiveTopology::TriangleList
            };
        }

        // UI element
        static PipelineKey uiElement()
        {
            return
            {
                //.shading_model = ShadingModel::Unlit,
                .renderer_type = MeshType::GUI,
                .render_pass = RenderPass::UI,
                .blend_state = BlendStatePreset::uiAlphaBlend(),
                .depth_state = DepthStencilPresets::uiOverlay(),
                .raster_state = RasterizerStatePresets::ui(), // No culling, scissor enabled
                .topology = PrimitiveTopology::TriangleList
            };
        }

        // Wireframe debug
        static PipelineKey wireframeDebug()
        {
            return
            {
                //.shading_model = ShadingModel::Unlit,
                .renderer_type = MeshType::StaticMesh,
                .render_pass = RenderPass::Debug,
                .blend_state = BlendStatePreset::opaque(),
                .depth_state = DepthStencilPresets::depthTestWrite(),
                .raster_state = RasterizerStatePresets::wireframe(),
                .topology = PrimitiveTopology::TriangleList
            };
        }
    };

}

#include <functional>
#include <cstring>

namespace std {

    template<> struct hash<kege::PipelineKey>
    {
        size_t operator()(const kege::PipelineKey& key) const
        {
            size_t h = 0;

            // Hash basic enum values (convert to underlying type)
            //hash_combine(h, static_cast<uint8_t>(key.shading_model));
            hash_combine(h, static_cast<uint8_t>(key.renderer_type));
            hash_combine(h, static_cast<uint8_t>(key.render_pass));
            hash_combine(h, key.features);

            // Hash composite structs
            hash_combine(h, hash_depth_stencil_state(key.depth_state));
            hash_combine(h, hash_rasterizer_state(key.raster_state));
            hash_combine(h, hash_blend_state(key.blend_state));

            // Additional render states
            hash_combine(h, static_cast<uint8_t>(key.topology));
            hash_combine(h, key.sample_count);
            hash_combine(h, key.alpha_to_coverage_enable);

            // Hash vertex signature (assuming it's a struct/bitmask)
            hash_combine(h, hash_vertex_signature(key.vertex_signature));

            return h;
        }

    private:
        // Combine hash values (boost::hash_combine style)
        static void hash_combine(size_t& seed, size_t value) {
            seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }

        static void hash_combine(size_t& seed, bool value) {
            hash_combine(seed, hash<bool>{}(value));
        }

        static void hash_combine(size_t& seed, uint8_t value) {
            hash_combine(seed, hash<uint8_t>{}(value));
        }

        static void hash_combine(size_t& seed, uint32_t value) {
            hash_combine(seed, hash<uint32_t>{}(value));
        }

        static void hash_combine(size_t& seed, uint64_t value) {
            hash_combine(seed, hash<uint64_t>{}(value));
        }

        // Hash DepthStencil
        static size_t hash_depth_stencil_state(const kege::DepthStencil& ds)
        {
            size_t h = 0;
            hash_combine(h, ds.depth.enable);
            hash_combine(h, ds.depth.write);
            hash_combine(h, static_cast<uint8_t>(ds.depth.compare_op));
            hash_combine(h, ds.depth.bounds_test);

            // Hash front face operations
            hash_combine(h, hash_face_operations(ds.stencil.front));
            hash_combine(h, hash_face_operations(ds.stencil.back));

            hash_combine(h, ds.stencil.reference);
            return h;
        }

        // Hash FaceOperations
        static size_t hash_face_operations(const kege::FaceOperations& fo)
        {
            size_t h = 0;
            hash_combine(h, static_cast<uint8_t>(fo.fail_op));
            hash_combine(h, static_cast<uint8_t>(fo.pass_op));
            hash_combine(h, static_cast<uint8_t>(fo.depth_fail_op));
            hash_combine(h, static_cast<uint8_t>(fo.compare_op));
            hash_combine(h, fo.compare_mask);
            hash_combine(h, fo.write_mask);
            return h;
        }

        // Hash RasterizerState (assuming you have one)
        static size_t hash_rasterizer_state(const kege::RasterizerState& rs)
        {
            // Implement based on your RasterizerState struct
            size_t h = 0;
            // Add all rasterizer state fields
            // hash_combine(h, rs.fill_mode);
            // hash_combine(h, rs.cull_mode);
            // etc.
            return h;
        }

        // Hash BlendState
        static size_t hash_blend_state(const kege::BlendState& bs)
        {
            size_t h = 0;
            hash_combine(h, bs.enable);
            hash_combine(h, static_cast<uint8_t>(bs.src_color_blend));
            hash_combine(h, static_cast<uint8_t>(bs.dst_color_blend));
            hash_combine(h, static_cast<uint8_t>(bs.color_blend_op));
            hash_combine(h, static_cast<uint8_t>(bs.src_alpha_blend));
            hash_combine(h, static_cast<uint8_t>(bs.dst_alpha_blend));
            hash_combine(h, static_cast<uint8_t>(bs.alpha_blend_op));
            hash_combine(h, static_cast<uint8_t>(bs.color_write_mask));
            return h;
        }

        // Hash VertexSignature
        static size_t hash_vertex_signature(const kege::VertexSignature& vs)
        {
            size_t h = 0;
            // Implement based on your VertexSignature struct
            // Could be bitmask or struct with vertex attributes
            return h;
        }
    };
}
#endif // !kege_pipeline_key_hpp

//
//  shader-common.cpp
//  editor
//
//  Created by Kenneth Esdaile on 5/3/26.
//

#include "shader-common.hpp"

namespace kege{

    const char* toString( const GlobalSemantic& semantic )
    {
        switch (semantic)
        {
            case GlobalSemantic::Camera: return "Camera";
            case GlobalSemantic::Shadow: return "Shadow";
            case GlobalSemantic::SceneDepth: return "SceneDepth";
            case GlobalSemantic::SceneColor: return "SceneColor";
            case GlobalSemantic::GBufferAlbedo: return "GBufferAlbedo";
            case GlobalSemantic::GBufferNormal: return "GBufferNormal";
            case GlobalSemantic::GBufferPosition: return "GBufferPosition";
            case GlobalSemantic::GBufferMaterial: return "GBufferMaterial";
            case GlobalSemantic::LightOutput: return "LightOutput";
            case GlobalSemantic::BRDFLut: return "BRDFLut";
            case GlobalSemantic::BloomInput: return "BloomInput";
            case GlobalSemantic::BloomOutput: return "BloomOutput";
            case GlobalSemantic::EnvironmentCubemap: return "EnvironmentCubemap";
            case GlobalSemantic::IrradianceCubemap: return "IrradianceCubemap";
            case GlobalSemantic::PostprocessHDR: return "PostprocessHDR";
            case GlobalSemantic::PostprocessLDR: return "PostprocessLDR";
            case GlobalSemantic::BRDF_LUT: return "BRDF_LUT";
            case GlobalSemantic::MotionVectors: return "MotionVectors";
            case GlobalSemantic::DofNearField: return "DofNearField";
            case GlobalSemantic::DofFarField: return "DofFarField";
            case GlobalSemantic::ViewUniforms: return "ViewUniforms";
            case GlobalSemantic::LightGrid: return "LightGrid";
            case GlobalSemantic::None: return "None";
            default: return "";
        }
    }

    kege::GlobalSemantic stringToSemantic( const std::string& name )
    {
        static std::map< std::string, kege::GlobalSemantic > types;

        if ( types.empty() )
        {
            types[ "None"  ] = kege::GlobalSemantic::None;
            types[ "Camera"  ] = kege::GlobalSemantic::Camera;
            types[ "ShadowMap"  ] = kege::GlobalSemantic::Shadow;
            types[ "SceneDepth"  ] = kege::GlobalSemantic::SceneDepth;
            types[ "SceneColor" ] = kege::GlobalSemantic::SceneColor;
            types[ "GBufferAlbedo" ] = kege::GlobalSemantic::GBufferAlbedo;
            types[ "GBufferNormal" ] = kege::GlobalSemantic::GBufferNormal;
            types[ "GBufferPosition" ] = kege::GlobalSemantic::GBufferPosition;
            types[ "GBufferMaterial" ] = kege::GlobalSemantic::GBufferMaterial;
            types[ "LightOutput" ] = kege::GlobalSemantic::LightOutput;
            types[ "BRDF_LUT" ] = kege::GlobalSemantic::BRDF_LUT;
            types[ "BloomInput" ] = kege::GlobalSemantic::BloomInput;
            types[ "BloomOutput" ] = kege::GlobalSemantic::BloomOutput;
            types[ "EnvironmentCubemap" ] = kege::GlobalSemantic::EnvironmentCubemap;
            types[ "IrradianceCubemap" ] = kege::GlobalSemantic::IrradianceCubemap;
            types[ "PostprocessHDR" ] = kege::GlobalSemantic::PostprocessHDR;
            types[ "PostprocessLDR" ] = kege::GlobalSemantic::PostprocessLDR;
            types[ "MotionVectors" ] = kege::GlobalSemantic::MotionVectors;
            types[ "DofNearField" ] = kege::GlobalSemantic::DofNearField;
            types[ "DofFarField" ] = kege::GlobalSemantic::DofFarField;
            types[ "ViewUniforms" ] = kege::GlobalSemantic::ViewUniforms;
            types[ "LightGrid" ] = kege::GlobalSemantic::LightGrid;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }
        if ( !name.empty() )
        {
            kege::Log::error << "unsupported GlobalSemantic -> " <<name <<kege::Log::nl;
        }
        return GlobalSemantic::None;
    }

    kege::BindingUsage stringToBindingUsage( const std::string& name )
    {
        static std::map< std::string, kege::BindingUsage > types;

        if ( types.empty() )
        {
            types[ "Invalid" ] = kege::BindingUsage::Invalid;
            types[ "Sampler" ] = kege::BindingUsage::Sampler;
            types[ "CombinedImageSampler" ] = kege::BindingUsage::CombinedImageSampler;
            types[ "SampledImage" ] = kege::BindingUsage::SampledImage;
            types[ "StorageImage" ] = kege::BindingUsage::StorageImage;
            types[ "UniformTexelBuffer" ] = kege::BindingUsage::UniformTexelBuffer;
            types[ "StorageTexelBuffer" ] = kege::BindingUsage::StorageTexelBuffer;
            types[ "UniformBuffer" ] = kege::BindingUsage::UniformBuffer;
            types[ "StorageBuffer" ] = kege::BindingUsage::StorageBuffer;
            types[ "UniformBufferDynamic" ] = kege::BindingUsage::UniformBufferDynamic;
            types[ "StorageBufferDynamic" ] = kege::BindingUsage::StorageBufferDynamic;
            types[ "InputAttachment" ] = kege::BindingUsage::InputAttachment;

            types[ "sampler" ] = kege::BindingUsage::Sampler;
            types[ "combined_image_sampler" ] = kege::BindingUsage::CombinedImageSampler;
            types[ "sampled_image" ] = kege::BindingUsage::SampledImage;
            types[ "storage_image" ] = kege::BindingUsage::StorageImage;
            types[ "uniform_texel_buffer" ] = kege::BindingUsage::UniformTexelBuffer;
            types[ "storage_texel_buffer" ] = kege::BindingUsage::StorageTexelBuffer;
            types[ "uniform_buffer" ] = kege::BindingUsage::UniformBuffer;
            types[ "storage_buffer" ] = kege::BindingUsage::StorageBuffer;
            types[ "uniform_buffer_dynamic" ] = kege::BindingUsage::UniformBufferDynamic;
            types[ "storage_buffer_dynamic" ] = kege::BindingUsage::StorageBufferDynamic;
            types[ "input_attachment" ] = kege::BindingUsage::InputAttachment;

            types[ "sampler"                ] = kege::BindingUsage::Sampler;
            types[ "sampled-image"          ] = kege::BindingUsage::SampledImage;
            types[ "combined-image-sampler" ] = kege::BindingUsage::CombinedImageSampler;
            types[ "storage-buffer"         ] = kege::BindingUsage::StorageBuffer;
            types[ "storage-image"          ] = kege::BindingUsage::StorageImage;
            types[ "storage-buffer-dynamic" ] = kege::BindingUsage::StorageBufferDynamic;
            types[ "storage-texel-buffer"   ] = kege::BindingUsage::StorageTexelBuffer;
            types[ "uniform-buffer"         ] = kege::BindingUsage::UniformBuffer;
            types[ "uniform-buffer-dynamic" ] = kege::BindingUsage::UniformBufferDynamic;
            types[ "uniform-texel-buffer"   ] = kege::BindingUsage::UniformTexelBuffer;

            types[ "SAMPLER"                ] = kege::BindingUsage::Sampler;
            types[ "SAMPLED_IMAGE"          ] = kege::BindingUsage::SampledImage;
            types[ "COMBINED_IMAGE_SAMPLER" ] = kege::BindingUsage::CombinedImageSampler;
            types[ "STORAGE_BUFFER"         ] = kege::BindingUsage::StorageBuffer;
            types[ "STROAGE_IMAGE"          ] = kege::BindingUsage::StorageImage;
            types[ "STORAGE_BUFFER_DYNAMIC" ] = kege::BindingUsage::StorageBufferDynamic;
            types[ "STORAGE_TEXEL_BUFFER"   ] = kege::BindingUsage::StorageTexelBuffer;
            types[ "UNIFORM_BUFFER"         ] = kege::BindingUsage::UniformBuffer;
            types[ "UNIFORM_BUFFER_DYNAMIC" ] = kege::BindingUsage::UniformBufferDynamic;
            types[ "UNIFORM_TEXEL_BUFFER"   ] = kege::BindingUsage::UniformTexelBuffer;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }
        kege::Log::error << "unsupported BindingUsage -> " <<name <<kege::Log::nl;
        return {};
    }

}

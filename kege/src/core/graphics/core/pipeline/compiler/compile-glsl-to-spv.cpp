//
//  compile-glsl-to-spv.cpp
//  editor
//
//  Created by Kenneth Esdaile on 5/3/26.
//

#include "shaderc/shaderc.hpp"
#include "compile-glsl-to-spv.hpp"

namespace kege{

    std::vector< uint32_t > compileGlslToSpv
    (
        kege::ShaderStageFlag shader_stage,
        const char* shader_name,
        const char* source,
        size_t source_size
    )
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        shaderc_shader_kind shaderc_shader_type;

        switch ( shader_stage )
        {
            case kege::ShaderStageFlag::Compute:
            {
                shaderc_shader_type = shaderc_compute_shader;
                break;
            }
            case kege::ShaderStageFlag::Vertex:
            {
                shaderc_shader_type = shaderc_vertex_shader;
                break;
            }
            case kege::ShaderStageFlag::Fragment:
            {
                shaderc_shader_type = shaderc_fragment_shader;
                break;
            }
            case kege::ShaderStageFlag::Geometry:
            {
                shaderc_shader_type = shaderc_geometry_shader;
                break;
            }
            case kege::ShaderStageFlag::TessellationEvaluation:
            {
                shaderc_shader_type = shaderc_tess_evaluation_shader;
                break;
            }
            case kege::ShaderStageFlag::TessellationControl:
            {
                shaderc_shader_type = shaderc_tess_control_shader;
                break;
            }
            default:
            {
                kege::Log::error <<"Invalid shader state in compileGlslTextToByteCode()" << Log::nl;
                return {};
            }
        }
        shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv
        (
            source,
            source_size - 1,
            shaderc_shader_type,
            shader_name,
            options
        );
        if (result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            kege::Log::error <<"error in compileGlslTextToByteCode(): "<< result.GetErrorMessage().data() << Log::nl;
            return {};
        }
        return std::vector< uint32_t >(result.cbegin(), result.cend());
    }


}

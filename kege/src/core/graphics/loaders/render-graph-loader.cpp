//
//  render-graph-loader.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/25/25.
//

#include "render-graph-loader.hpp"

namespace kege{

    RenderGraphLoader::FunctionMap RenderGraphLoader::_parsers;
    RenderGraphLoader::IntFunctMap RenderGraphLoader::_int_funct_maps;

    bool RenderGraphLoader::load( kege::RenderGraph& graph, const std::string& filename )
    {
        Json json = kege::JsonParser::load( filename.c_str() );
        if ( !json )
        {
            return false;
        }

        Json resources = json[ "resources" ];
        if ( !resources )
        {
            return false;
        }

        Json passes = json[ "passes" ];
        if ( !passes )
        {
            return false;
        }


        for( uint32_t i = 0; i < resources.count(); ++i )
        {
            Json res = resources[i];
            std::string type = res[ "type" ].value();
            FunctionMap::iterator parser = _parsers.find( type );
            if ( parser == _parsers.end() )
            {
                parser->second( graph, res );
            }
        }

        for( uint32_t i = 0; i < passes.count(); ++i )
        {
            Json pass = passes[i];
            std::string type = pass[ "type" ].value();
            FunctionMap::iterator parser = _parsers.find( type );
            if ( parser == _parsers.end() )
            {
                parser->second( graph, pass );
            }
        }

        return true;
    }

    void RenderGraphLoader::graphicsPass( kege::RenderGraph& graph, Json json )
    {}
    void RenderGraphLoader::computePass( kege::RenderGraph& graph, Json json )
    {}
    void RenderGraphLoader::shaderResource( kege::RenderGraph& graph, Json json )
    {
        uint32_t frames_in_flight = getInt( graph, json[ "frames_in_flight" ] );
        Json json_bindings = json[ "bindings" ];
        Json json_targets = json[ "targets" ];

        std::vector< kege::DescriptorSetLayoutBinding > bindings;
        std::vector< kege::RgShaderResource > targets;

        for( uint32_t i = 0; i < json_bindings.count(); ++i )
        {
            Json binding = json_bindings[ i ];

            kege::DescriptorSetLayoutBinding layout;
            layout.name = binding[ "name" ].value();
            layout.count = binding[ "count" ].getInt();
            layout.binding = binding[ "binding" ].getInt();
            layout.descriptor_type = getDescriptorType( binding[ "descriptor_type" ] );
            layout.stage_flags = getShaderStageFlag( binding[ "stage_flags" ] );

            bindings.push_back( layout );
        }

        for( uint32_t i = 0; i < json_targets.count(); ++i )
        {
            Json target = json_targets[ i ];
            targets.push_back( getRgShaderResource( target[ "type" ] ) );
        }

        kege::RgResrcHandle handle = graph.defineShaderResource( json[ "id" ].value(), frames_in_flight, bindings );
        graph.updateShaderResource( handle, targets );
    }

    void RenderGraphLoader::sampler( kege::RenderGraph& graph, Json json )
    {
        kege::SamplerDesc desc = {};

        desc.debug_name = json[ "id" ].value();
        desc.mag_filter = toFilter( json[ "mag_filter" ].value() );
        desc.min_filter = toFilter( json[ "min_filter" ].value() );
        desc.mipmap_mode = toMipmapMode( json[ "mipmap_mode" ].value() );
        desc.address_mode_u = toAddressMode( json[ "address_mode_u" ].value() );
        desc.address_mode_v = toAddressMode( json[ "address_mode_v" ].value() );
        desc.address_mode_w = toAddressMode( json[ "address_mode_w" ].value() );

        graph.defineSampler( desc.debug_name, desc );
    }

    void RenderGraphLoader::buffer( kege::RenderGraph& graph, Json json )
    {
        kege::BufferDefn desc = {};

        desc.frames_in_flight = getInt( graph, json[ "frames_in_flight" ] );
        desc.name = json[ "name" ].value();
        desc.info.data = nullptr;
        desc.info.size = json[ "info" ][ "size" ].getInt();
        desc.info.usage = getBufferUsage( json[ "usage" ] );
        desc.info.memory_usage = getMemoryUsage( json[ "memory_usage" ] );

        graph.defineBuffer( desc );
    }

    void RenderGraphLoader::image( kege::RenderGraph& graph, Json json )
    {
        kege::ImageDefn desc = {};

        desc.name = json[ "name" ].value();
        desc.frames_in_flight = getInt( graph, json[ "frames_in_flight" ] );
        desc.usages = getImageUsage( json[ "usages" ] );
        desc.info.width  = getInt( graph, json[ "info" ][ "width" ] );
        desc.info.height = getInt( graph, json[ "info" ][ "width" ] );
        desc.info.depth  = getInt( graph, json[ "info" ][ "depth" ] );
        desc.info.type   = getImageType( json[ "info" ][ "type" ] );
        desc.info.format = getFormat( graph, json[ "info" ][ "format" ] );

        Json physical_handle = json[ "physical_handle" ];
        if ( physical_handle )
        {
            if ( strcmp( physical_handle.value(), "getSwapchainColorImages()" ) == 0 )
            {
                //desc.physical_handle =
            }
        }

        graph.defineImage( desc );
    }

    RgShaderResource RenderGraphLoader::getRgShaderResource( Json json )
    {
        std::string name = json.value();
        
        return {};
    }

    int RenderGraphLoader::getInt( kege::RenderGraph& graph, Json json )
    {
        IntFunctMap::iterator m = _int_funct_maps.find( json.value() );
        if ( m != _int_funct_maps.end() )
        {
            return m->second( graph, json );
        }
        return 0;
    }

    Format RenderGraphLoader::getFormat( kege::RenderGraph& graph, Json json )
    {
        std::string name = json.value();

        if ( name == "getSwapchainDepthFormat()" )
        {
            return graph.getGraphics()->getSwapchainDepthFormat();
        }
        else if ( name == "getSwapchainColorFormat()" )
        {
            return graph.getGraphics()->getSwapchainColorFormat();
        }

        static std::map< std::string, Format > types;
        if ( types.empty() )
        {
            types[ "r_s8" ] = Format::r_s8;
            types[ "rg_s8" ] = Format::rg_s8;
            types[ "rgb_s8" ] = Format::rgb_s8;
            types[ "rgba_s8" ] = Format::rgba_s8;

            types[ "r_u8" ] = Format::r_u8;
            types[ "rg_u8" ] = Format::rg_u8;
            types[ "rgb_u8" ] = Format::rgb_u8;
            types[ "rgba_u8" ] = Format::rgba_u8;

            types[ "r_s16" ] = Format::r_s16;
            types[ "rg_s16" ] = Format::rg_s16;
            types[ "rgb_s16" ] = Format::rgb_s16;
            types[ "rgba_s16" ] = Format::rgba_s16;

            types[ "r_u16" ] = Format::r_u16;
            types[ "rg_u16" ] = Format::rg_u16;
            types[ "rgb_u16" ] = Format::rgb_u16;
            types[ "rgba_u16" ] = Format::rgba_u16;

            types[ "r_s32" ] = Format::r_s32;
            types[ "rg_s32" ] = Format::rg_s32;
            types[ "rgb_s32" ] = Format::rgb_s32;
            types[ "rgba_s23" ] = Format::rgba_s32;

            types[ "r_u32" ] = Format::r_u32;
            types[ "rg_u32" ] = Format::rg_u32;
            types[ "rgb_u32" ] = Format::rgb_u32;
            types[ "rgba_u32" ] = Format::rgba_u32;

            types[ "r_s64" ] = Format::r_s64;
            types[ "rg_s64" ] = Format::rg_s64;
            types[ "rgb_s46" ] = Format::rgb_s64;
            types[ "rgba_s64" ] = Format::rgba_s64;

            types[ "r_u64" ] = Format::r_u64;
            types[ "rg_u64" ] = Format::rg_u64;
            types[ "rgb_u46" ] = Format::rgb_u64;
            types[ "rgba_u64" ] = Format::rgba_u64;


            types[ "r_s8_norm" ] = Format::r_s8_norm;
            types[ "rg_s8_norm" ] = Format::rg_s8_norm;
            types[ "rgb_s8_norm" ] = Format::rgb_s8_norm;
            types[ "rgba_s8_norm" ] = Format::rgba_s8_norm;

            types[ "r_u8_norm" ] = Format::r_u8_norm;
            types[ "rg_u8_norm" ] = Format::rg_u8_norm;
            types[ "rgb_u8_norm" ] = Format::rgb_u8_norm;
            types[ "rgba_u8_norm" ] = Format::rgba_u8_norm;


            types[ "r_8_srgb" ] = Format::r_8_srgb;
            types[ "rg_8_srgb" ] = Format::rg_8_srgb;
            types[ "rgb_8_srgb" ] = Format::rgb_8_srgb;
            types[ "rgba_8_srgb" ] = Format::rgba_8_srgb;

            types[ "bgr_8_srgb" ] = Format::rgba_8_srgb;
            types[ "bgra_8_srgb" ] = Format::rgba_8_srgb;


            types[ "bgr_u8" ] = Format::bgr_u8;
            types[ "bgr_s8" ] = Format::bgr_s8;
            types[ "bgr_s8_norm" ] = Format::bgr_s8_norm;
            types[ "bgr_u8_norm" ] = Format::bgr_u8_norm;
            types[ "bgra_u8" ] = Format::bgra_u8;
            types[ "bgra_s8" ] = Format::bgra_s8;
            types[ "bgra_s8_norm" ] = Format::bgra_s8_norm;
            types[ "bgra_u8_norm" ] = Format::bgra_u8_norm;
            types[ "bgra_u8_norm_srbg" ] = Format::bgra_u8_norm_srbg;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }
        return {};
    }

    ImageType RenderGraphLoader::getImageType( Json json )
    {
        static std::map< std::string, ImageType > types;
        if ( types.empty() )
        {
            types[ "Type1D" ] = ImageType::Type1D;
            types[ "Type1DArray" ] = ImageType::Type1DArray;
            types[ "Type2D" ] = ImageType::Type2D;
            types[ "Type2DArray" ] = ImageType::Type2DArray;
            types[ "TypeCube" ] = ImageType::TypeCube;
            types[ "TypeCubeArray" ] = ImageType::TypeCubeArray;
            types[ "Type3D" ] = ImageType::Type3D;
        }
        auto m = types.find( json.value() );
        if ( m != types.end() )
        {
            return m->second;
        }
        return {};
    }

    BufferUsage RenderGraphLoader::getBufferUsage( Json json )
    {
        static std::map< std::string, BufferUsage > types;
        if ( types.empty() )
        {
            types[ "None" ] = BufferUsage::None;
            types[ "CopySrc" ] = BufferUsage::CopySrc;
            types[ "CopyDst" ] = BufferUsage::CopyDst;
            types[ "VertexBuffer" ] = BufferUsage::VertexBuffer;
            types[ "IndexBuffer" ] = BufferUsage::IndexBuffer;
            types[ "UniformBuffer" ] = BufferUsage::UniformBuffer;
            types[ "StorageBuffer" ] = BufferUsage::StorageBuffer;
            types[ "IndirectBuffer" ] = BufferUsage::IndirectBuffer;
            types[ "UniformTexelBuffer" ] = BufferUsage::UniformTexelBuffer;
            types[ "StorageTexelBuffer" ] = BufferUsage::StorageTexelBuffer;
            types[ "ShaderDeviceAddress" ] = BufferUsage::ShaderDeviceAddress;
        }
        auto m = types.find( json.value() );
        if ( m != types.end() )
        {
            return m->second;
        }
        return {};
    }

    MemoryUsage RenderGraphLoader::getMemoryUsage( Json json )
    {
        static std::map< std::string, MemoryUsage > types;
        if ( types.empty() )
        {
            types[ "GpuOnly" ] = MemoryUsage::GpuOnly;
            types[ "CpuToGpu" ] = MemoryUsage::CpuToGpu;
            types[ "GpuToCpu" ] = MemoryUsage::GpuToCpu;
            types[ "CpuOnly" ] = MemoryUsage::CpuOnly;
        }
        auto m = types.find( json.value() );
        if ( m != types.end() )
        {
            return m->second;
        }
        return {};
    }

    ImageUsageFlags RenderGraphLoader::getImageUsage( Json json )
    {
        static std::map< std::string, ImageUsageFlags > types;
        if ( types.empty() )
        {
            types[ "None" ] = ImageUsageFlags::None;
            types[ "CopySrc" ] = ImageUsageFlags::CopySrc;
            types[ "CopyDst" ] = ImageUsageFlags::CopyDst;
            types[ "ShaderResource" ] = ImageUsageFlags::ShaderResource;
            types[ "Storage" ] = ImageUsageFlags::Storage;
            types[ "ColorAttachment" ] = ImageUsageFlags::ColorAttachment;
            types[ "DepthStencilAttachment" ] = ImageUsageFlags::DepthStencilAttachment;
            types[ "TransientAttachment" ] = ImageUsageFlags::TransientAttachment;
            types[ "InputAttachment" ] = ImageUsageFlags::InputAttachment;
            types[ "ResolveSrc" ] = ImageUsageFlags::ResolveSrc;
            types[ "ResolveDst" ] = ImageUsageFlags::ResolveDst;
            types[ "Present" ] = ImageUsageFlags::Present;
        }
        auto m = types.find( json.value() );
        if ( m != types.end() )
        {
            return m->second;
        }

        return {};
    }

    DescriptorType RenderGraphLoader::getDescriptorType( Json json )
    {
        static std::map< std::string, DescriptorType > types;

        if ( types.empty() )
        {
            types[ "Invalid" ] = DescriptorType::Invalid;
            types[ "Sampler" ] = DescriptorType::Sampler;
            types[ "CombinedImageSampler" ] = DescriptorType::CombinedImageSampler;
            types[ "SampledImage" ] = DescriptorType::SampledImage;
            types[ "StorageImage" ] = DescriptorType::StorageImage;
            types[ "UniformTexelBuffer" ] = DescriptorType::UniformTexelBuffer;
            types[ "StorageTexelBuffer" ] = DescriptorType::StorageTexelBuffer;
            types[ "UniformBuffer" ] = DescriptorType::UniformBuffer;
            types[ "StorageBuffer" ] = DescriptorType::StorageBuffer;
            types[ "UniformBufferDynamic" ] = DescriptorType::UniformBufferDynamic;
            types[ "StorageBufferDynamic" ] = DescriptorType::StorageBufferDynamic;
            types[ "InputAttachment" ] = DescriptorType::InputAttachment;
        }
        auto m = types.find( json.value() );
        if ( m != types.end() )
        {
            return m->second;
        }

        return {};
    }

    ShaderStage RenderGraphLoader::getShaderStageFlag( Json json )
    {
        static std::map< std::string, ShaderStage > types;

        if ( types.empty() )
        {
            types[ "Invalid" ] = ShaderStage::Invalid;
            types[ "vertex" ] = ShaderStage::Vertex;
            types[ "fragment" ] = ShaderStage::Fragment;
            types[ "compute" ] = ShaderStage::Compute;
            types[ "geometry" ] = ShaderStage::Geometry;
            types[ "tessellation-control" ] = ShaderStage::TessellationControl;
            types[ "tessellation-evaluation" ] = ShaderStage::TessellationEvaluation;
            types[ "all-graphics" ] = ShaderStage::AllGraphics;
            types[ "all" ] = ShaderStage::All;
        }
        auto m = types.find( json.value() );
        if ( m != types.end() )
        {
            return m->second;
        }

        return {};
    }


    kege::Filter RenderGraphLoader::toFilter( const std::string& name )
    {
        static std::map< std::string, Filter > types;

        if ( types.empty() )
        {
            types[ "linear" ] = Filter::Linear;
            types[ "nearest" ] = Filter::Nearest;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }

        return {};
    }

    kege::MipmapMode RenderGraphLoader::toMipmapMode( const std::string& name )
    {
        static std::map< std::string, MipmapMode > types;

        if ( types.empty() )
        {
            types[ "linear" ] = MipmapMode::Linear;
            types[ "nearest" ] = MipmapMode::Nearest;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }

        return {};
    }

    kege::AddressMode RenderGraphLoader::toAddressMode( const std::string& name )
    {
        static std::map< std::string, AddressMode > types;

        if ( types.empty() )
        {
            types[ "repeat" ] = AddressMode::Repeat;
            types[ "mirrored-repeat" ] = AddressMode::MirroredRepeat;
            types[ "clamp-to-edge" ] = AddressMode::ClampToEdge;
            types[ "clamp-to-border" ] = AddressMode::ClampToBorder;
            types[ "mirror-clamp-to-edge" ] = AddressMode::MirrorClampToEdge;
        }
        auto m = types.find( name );
        if ( m != types.end() )
        {
            return m->second;
        }

        return {};
    }

    int RenderGraphLoader::getSwapchainImageCount( kege::RenderGraph& graph, Json json )
    {
        return graph.getGraphics()->getSwapchainImageCount();
    }

    int RenderGraphLoader::getSwapchainExtentHeight( kege::RenderGraph& graph, Json json )
    {
        return graph.getGraphics()->getSwapchainExtent().height;
    }

    int RenderGraphLoader::getSwapchainExtentWidth( kege::RenderGraph& graph, Json json )
    {
        return graph.getGraphics()->getSwapchainExtent().width;
    }

    int RenderGraphLoader::getFramesInFlight( kege::RenderGraph& graph, Json json )
    {
        return MAX_FRAMES_IN_FLIGHT;
    }

    RenderGraphLoader::RenderGraphLoader()
    {
        _parsers[ "graphics" ] = graphicsPass;
        _parsers[ "compute" ] = computePass;
        
        _parsers[ "shader_resource" ] = shaderResource;
        _parsers[ "sampler" ] = sampler;
        _parsers[ "buffer" ] = buffer;
        _parsers[ "image" ] = image;


        _int_funct_maps[ "getSwapchainImageCount()" ] = getSwapchainImageCount;
        _int_funct_maps[ "getSwapchainExtentHeight()" ] = getSwapchainExtentHeight;
        _int_funct_maps[ "getSwapchainExtentWidth()" ] = getSwapchainExtentWidth;
        _int_funct_maps[ "FRAMES_IN_FLIGHT" ] = getFramesInFlight;
    }

}

//
//  string-to-enum-types.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/3/25.
//

#ifndef string_to_enum_types_hpp
#define string_to_enum_types_hpp

#include <map>
#include <string>
#include <unordered_map>
#include "graphics-core.hpp"
#include "string-to-enum-types.hpp"
#include "image.hpp"
#include "buffer.hpp"
#include "../pipeline/shader-io.hpp"

namespace kege{

    ShaderVarType stringToShaderVarType( const std::string& str );

    kege::CompareOp stringToCompareOp( const std::string& type );

    StencilOp stringToStencilOp( const std::string& type );

    ColorBlendLogicOp stringToColorBlendLogicOp( const std::string& type );

    kege::BlendFactor stringToBlendFactor( const std::string& type );

    kege::BlendOp stringToBlendOp( const std::string& type );

    FrontFace stringToFrontFace( const std::string& type );

    kege::PolygonMode stringToPolygonMode( const std::string& type );

    kege::CullMode stringToCullMode( const std::string& type );

    kege::PrimitiveTopology stringToPrimitiveTopology( const std::string& type );

    kege::DescriptorType convertDescriptorType( const std::string& type );

    kege::ShaderVarType convertVertexInputType( const std::string& type );

    kege::Filter stringToFilter( const std::string& name );

    kege::MipmapMode stringToMipmapMode( const std::string& name );

    ImageType stringToImageType( const std::string& name );

    BufferUsages stringToBufferUsage( const std::string& name );

    MemoryUsage stringToMemoryUsage( const std::string& name );

    ImageUsage stringToImageUsage( const std::string& name );

    kege::BindingUsage stringToBindingUsage( const std::string& name );

    kege::ShaderStageFlag stringToShaderStageFlag( const std::string& name );

    kege::PipelineType stringToPipelineType( const std::string& s );

    kege::AddressMode stringToAddressMode( const std::string& name );

    QueueType stringToQueueType( const std::string& name );

    ImageLayout stringToImageLayout( const std::string& name );

    AccessFlags stringToAccessFlags( const std::string& name );

    PipelineStageFlag stringToPipelineStageFlags( const std::string& name );

    AttachmentLoadOp stringToAttachmentLoadOp( const std::string& name );

    Format stringToFormat( const std::string& name );
}

#endif /* string_to_enum_types_hpp */

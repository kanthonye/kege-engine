//
//  string-to-enum-types.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 5/3/25.
//

#ifndef string_to_enum_types_hpp
#define string_to_enum_types_hpp

#include <map>
#include "format.h"
#include "access-flags.h"
#include "blend-state.hpp"
#include "dynamic-state.hpp"
#include "feature-flags.hpp"
#include "pipeline-key.hpp"
#include "pipeline-stage-flag.h"
#include "memory-usage.h"
#include "rasterizer-state.hpp"
#include "buffer-usages.hpp"
#include "image-layout.h"
#include "vertex-format.hpp"
#include "shader-stage-flag.h"
#include "../../../../../utils/log.hpp"

namespace kege{

    /**
     * @brief Enumeration of queue types.
     *
     * Specifies the type of command queue that can execute commands.
     */
    enum class QueueType
    {
        Invalid,
        Graphics,  ///< Graphics queue capable of all operations
        Compute,   ///< Compute-only queue
        Transfer   ///< Transfer-only queue
    };

    kege::QueueType stringToQueueType( const std::string& name );

    
    DynamicState stringToDynamicState( const std::string& str );

    kege::ShaderVar stringToShaderVarType( const std::string& str );
    kege::VertexInputRate stringToVertexInputRate( const std::string& str );

    kege::ShaderVar convertVertexInputType( const std::string& type );

    kege::ComparisonFunc stringToCompareOp( const std::string& type );

    StencilOp stringToStencilOp( const std::string& type );

    ColorBlendLogicOp stringToColorBlendLogicOp( const std::string& type );

    kege::BlendFactor stringToBlendFactor( const std::string& type );

    kege::BlendOp stringToBlendOp( const std::string& type );

    FrontFace stringToFrontFace( const std::string& type );

    kege::FillMode stringToPolygonMode( const std::string& type );

    kege::CullMode stringToCullMode( const std::string& type );

    kege::PrimitiveTopology stringToPrimitiveTopology( const std::string& type );

    kege::Filter stringToFilter( const std::string& name );

    kege::MipmapMode stringToMipmapMode( const std::string& name );

    ImageType stringToImageType( const std::string& name );

    MemoryUsage stringToMemoryUsage( const std::string& name );

    ImageUsage stringToImageUsage( const std::string& name );

    kege::ShaderStageFlag stringToShaderStageFlag( const std::string& name );

    kege::AddressMode stringToAddressMode( const std::string& name );

    ImageLayout stringToImageLayout( const std::string& name );

    AccessFlags stringToAccessFlags( const std::string& name );

    PipelineStageFlag stringToPipelineStageFlags( const std::string& name );

    Format stringToFormat( const std::string& name );
}
#endif /* string_to_enum_types_hpp */

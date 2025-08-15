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
#include "../../graphics/core/graphics-core.hpp"

namespace kege{

    kege::CompareOp stringToCompareOp( const std::string& type );
    kege::StencilOp stringToStencilOp( const std::string& type );
    kege::ColorBlendLogicOp stringToColorBlendLogicOp( const std::string& type );
    kege::BlendFactor stringToBlendFactor( const std::string& type );
    kege::BlendOp stringToBlendOp( const std::string& type );

    kege::FrontFace stringToFrontFace( const std::string& type );

    kege::CullMode stringToCullMode( const std::string& type );
    
    kege::PolygonMode stringToPolygonMode( const std::string& type );
    
    kege::PrimitiveTopology stringToPrimitiveTopology( const std::string& type );

    kege::DescriptorType convertDescriptorType( const std::string& type );

    kege::ShaderStage convertShaderStage( const std::string& type );

    kege::Format convertVertexInputType( const std::string& type );

    kege::Format stringToFormat( const std::string& type );
}

#endif /* string_to_enum_types_hpp */

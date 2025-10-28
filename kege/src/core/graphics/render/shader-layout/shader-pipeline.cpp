//
//  shader-pipeline.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/21/25.
//

#include "shader-pipeline.hpp"

namespace kege{


    kege::PipelineType ShaderPipeline::getPipelineType()const
    {
        return _pipeline_type;
    }

    ShaderPipeline::ShaderPipeline( kege::PipelineType pipeline_type, kege::Ref< const kege::ShaderLayout >& shader_layout )
    :   _shader_layout( shader_layout )
    ,   _pipeline_type( pipeline_type )
    {}
}

//
//  shader-pipeline.cpp
//  physics
//
//  Created by Kenneth Esdaile on 9/10/25.
//

#include "shader-pipeline.hpp"

namespace kege{

    const kege::ShaderPipeline::States ShaderPipeline::states()const
    {
        return _states;
    }

    const kege::PipelineHandle& ShaderPipeline::handle()const
    {
        return _pipeline;
    }

    ShaderPipeline::ShaderPipeline()
    :   _states({})
    {}

    ShaderPipeline::~ShaderPipeline()
    {}
}

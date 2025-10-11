//
//  shader-pipeline.cpp
//  physics
//
//  Created by Kenneth Esdaile on 9/10/25.
//

#include "shader-pipeline.hpp"
#include "shader-pipeline-manager.hpp"

namespace kege{


    // Define equality comparison for VkDescriptorSetLayoutBinding
    bool operator==(const kege::ShaderPipeline& a, const kege::ShaderPipeline& b)
    {
        return a.id() == b.id();
    }

    // Define none equality comparison for VkDescriptorSetLayoutBinding
    bool operator!=(const kege::ShaderPipeline& a, const kege::ShaderPipeline& b)
    {
        return a.id() != b.id();
    }

    // Define less-than comparison (needed to resolve the compiler error)
    bool operator<(const kege::ShaderPipeline& a, const kege::ShaderPipeline& b)
    {
        return a.id() < b.id();
    }

    const PipelineContext* ShaderPipeline::operator ->()const
    {
        return &_manager->_pipeline_objects[ _index ].pipeline_contexts[ 0 ];
    }

    const kege::PipelineContext& ShaderPipeline::operator[]( int i ) const
    {
        return _manager->_pipeline_objects[ _index ].pipeline_contexts[ 0 ];
    }

    kege::PipelineContext& ShaderPipeline::operator[]( int i )
    {
        return _manager->_pipeline_objects[ _index ].pipeline_contexts[ 0 ];
    }

    ShaderPipeline::operator bool()const
    {
        return (_index < 0) ? false : _index < _manager->_pipeline_objects.size();
    }

    PipelineHandle ShaderPipeline::handle()const
    {
        return _manager->_pipeline_objects[ _index ].pipeline_contexts[0].pipeline;
    }
    
    int ShaderPipeline::id()const
    {
        return _index;
    }

    ShaderPipeline& ShaderPipeline::operator =( ShaderPipeline&& other)noexcept
    {
        _manager = other._manager;
        _index = other._index;
        other._manager = nullptr;
        other._index = -1;
        return *this;
    }

    ShaderPipeline& ShaderPipeline::operator =( const ShaderPipeline& other )
    {
        _manager = other._manager;
        _index = other._index;
        _manager->_pipeline_objects[ _index ].duplicates += 1;
        return *this;
    }

    ShaderPipeline::ShaderPipeline( const ShaderPipeline& other )
    :   _manager( other._manager )
    ,   _index( other._index )
    {
        _manager->_pipeline_objects[ _index ].duplicates += 1;
    }

    ShaderPipeline::ShaderPipeline( ShaderPipeline&& other )noexcept
    :   _manager( other._manager )
    ,   _index( other._index )
    {
        other._manager = nullptr;
        other._index = -1;
    }

    ShaderPipeline::~ShaderPipeline()
    {
        if ( _manager )
        {
            _manager->_pipeline_objects[ _index ].duplicates -= 1;
            _manager->destroy( *this );
            _manager = nullptr;
            _index = -1;
        }
    }
    ShaderPipeline::ShaderPipeline(ShaderPipelineManager* manager, int32_t index)
    :   _manager( manager )
    ,   _index( index )
    {
        _manager->_pipeline_objects[ _index ].duplicates += 1;
    }

    ShaderPipeline::ShaderPipeline()
    :   _manager( nullptr )
    ,   _index( -1 )
    {}

}

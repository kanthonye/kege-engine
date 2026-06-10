//
//  shader-pipeline-resource-layout.cpp
//  editor
//
//  Created by Kenneth Esdaile on 4/28/26.
//

#include "shader-pipeline-layout.hpp"

namespace kege{

    const kege::ShaderSetBindingDesc* ShaderPipelineLayout::getBindingDesc( uint32_t set, uint32_t binding )const
    {
        return &_shader_resource_layouts[ set ]->getDesc().at( binding );
    }

    const kege::ShaderResourceSetLayout* ShaderPipelineLayout::getShaderResourceSetLayout( uint32_t set )const
    {
        return _shader_resource_layouts[ set ].ref();
    }

    kege::SetIndex ShaderPipelineLayout::getBindLocationIndex( kege::UniformKey key )const
    {
        auto i = _uniform_key_map.find( key );
        return (i != _uniform_key_map.end()) ? i->second: 0;
    }

    ShaderPipelineLayout::ShaderPipelineLayout
    (
        const std::string& name,
        const kege::PushBlockDescs& push_block_desc,
        std::vector< kege::ref::ShaderResourceSetLayout >& layouts
    )
    :   _shader_resource_layouts( layouts )
    ,   _push_block_descs( push_block_desc )
    {
        /**
         * Maps shader resource bindings to both their string names and global semantic enums.
         *
         * This creates two distinct lookup paths for efficient resource binding and updates:
         * 1. String-based lookup: Maps a resource's name to its (set, binding) pair.
         * 2. Semantic-based lookup: Maps a global enum to its (set, binding) pair (if applicable).
         */
        for (int set_index = 0; set_index < layouts.size(); ++set_index)
        {
            const kege::ShaderSetDesc& set_desc = layouts[set_index]->getDesc();
            for (int binding_index = 0; binding_index < set_desc.size(); ++binding_index)
            {
                // 1. Map resource by its string name
                kege::ShaderResrcIndexPair& pair = _binding_map[ set_desc[ binding_index ].name ];
                pair.binding = set_desc[ binding_index ].binding_index;
                pair.set = set_index;
                
//                // 2. Map resource by its global semantic (if valid)
//                if ( set_desc[ binding_index ].sementic != kege::GlobalSemantic::None )
//                {
//                    _global_semantic_map[ set_desc[ binding_index ].sementic ] = pair;
//                }
            }

            /**
             * UniformKey: A unique identifier for ShaderResourceSetLayouts based on their binding signature.
             *
             * Problem: Different shader pipelines may use identical resource layouts but at different set indices.
             * Example:
             *   - Pipeline A: Camera at layout(set=0, binding=0) uniform Camera
             *   - Pipeline B: Camera at layout(set=2, binding=0) uniform Camera
             *
             * The camera is the first resource in Pipeline A's set array, but the second resource in Pipeline B's.
             *
             * Solution:
             *   1. Each distinct ShaderResourceSetLayout binding signature gets a unique UniformKey
             *   2. The UniformKey maps to a set index for correct resource placement
             *   3. A single descriptor set (created from the camera layout) can bind to both pipelines
             *
             * This ensures consistent binding order across pipelines that share identical resource layouts
             * but at different set positions.
             */
            _uniform_key_map[ layouts[ set_index ]->getUniformKey() ] = set_index;
        }
    }
}

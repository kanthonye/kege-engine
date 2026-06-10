//
//  shader-pipeline-resource-layout.hpp
//  editor
//
//  Created by Kenneth Esdaile on 4/28/26.
//

#ifndef shader_pipeline_resource_layout_hpp
#define shader_pipeline_resource_layout_hpp

#include <unordered_map>
#include "shader-resource-set.hpp"
#include "shader-resource-set-layout.hpp"


namespace kege
{
    class ShaderPipelineLayout;
}

namespace kege::vk
{
    class ShaderPipelineLayout;
}

namespace kege::ref
{
    typedef kege::Ref< kege::ShaderPipelineLayout > ShaderPipelineLayout;
}

namespace kege::cref
{
    typedef kege::Ref< const kege::ShaderPipelineLayout > ShaderPipelineLayout;
}

namespace kege{

    /**
     * ShaderPipelineLayout: Describes the complete resource binding layout for a shader pipeline.
     *
     * Purpose:
     *   This class serves as the backbone of a ShaderPipeline, defining how shader resources
     *   (uniform buffers, textures, storage buffers, etc.) are organized across descriptor sets
     *   and push constant blocks. It abstracts away the differences between how different
     *   shader pipelines declare the same logical resources at different set/binding indices.
     *
     * What it does:
     *   1. Stores a collection of ShaderResourceSetLayouts that define the structure of each
     *      descriptor set used by the pipeline.
     *   2. Maps uniform names to their actual set+binding locations via _binding_map.
     *   3. Maps GlobalSemantic enums (e.g., kCamera, kShadowMap) to binding locations
     *      for global resources not tied to specific meshes or materials.
     *   4. Maps UniformKeys (unique identifiers for binding signatures) to set indices,
     *      allowing the same descriptor set to be reused across pipelines with different
     *      set index assignments.
     *   5. Maintains a cached list of global resource binding pairs for efficient
     *      per-frame binding.
     *
     * How it's used:
     *   1. Create a ShaderPipelineLayout during shader compilation by parsing SPIR-V
     *      reflection data (or manual definition).
     *   2. Query binding locations using:
     *      - getShaderSetBindingDesc(pair) - Get full binding description
     *      - getBindLocationIndex(key) - Get set index for a UniformKey
     *   3. Control global resource behavior with setShaderBindingGlobal(name, state)
     *   4. Retrieve the cached global resource list via getGlobalResourceList() for
     *      efficient per-frame binding.
     *   5. Obtain the underlying Vulkan layout via vk() when creating a pipeline.
     *
     * Key design benefit:
     *   Two different shader pipelines can declare the same logical resource (e.g., "Camera")
     *   at different set indices. ShaderPipelineLayout handles the mapping so the rest of
     *   the engine can bind resources without caring about pipeline-specific set indices.
     *
     * Inherits: kege::RefCounter for reference-counted lifetime management.
     */
    class ShaderPipelineLayout : public kege::RefCounter
    {
    public:

        const kege::ShaderSetBindingDesc* getBindingDesc( uint32_t set, uint32_t binding )const;
        const kege::ShaderResourceSetLayout* getShaderResourceSetLayout( uint32_t set )const;
        kege::SetIndex getBindLocationIndex( kege::UniformKey key )const;

        virtual const vk::ShaderPipelineLayout* vk() const { return nullptr; }
        virtual vk::ShaderPipelineLayout* vk() { return nullptr; }

        ShaderPipelineLayout
        (
            const std::string& name,
            const kege::PushBlockDescs& push_block_desc,
            std::vector< kege::ref::ShaderResourceSetLayout >& layouts
        );
        virtual ~ShaderPipelineLayout(){}

    protected:

        ShaderPipelineLayout& operator =(const ShaderPipelineLayout&) = delete;
        ShaderPipelineLayout& operator =(ShaderPipelineLayout&&) = delete;
        ShaderPipelineLayout(const ShaderPipelineLayout&) = default;
        ShaderPipelineLayout(ShaderPipelineLayout&&) = delete;
        ShaderPipelineLayout() = default;

    protected:

        /**
         * _global_semantic_map: Maps a GlobalSemantic enum to a ShaderResrcIndexPair (set + binding indices).
         *
         * Problem: Global resources (e.g., render graph outputs, lighting data, camera matrices) are not
         * associated with specific meshes or materials, yet they need to be bound to the correct
         * binding points across different shader pipelines.
         *
         * Example:
         *   - A render graph output resource has a semantic like "SHADOW_MAP"
         *   - Each shader pipeline declares that semantic at potentially different set/binding locations
         *   - The semantic maps to: Pipeline A (set=1, binding=2), Pipeline B (set=3, binding=0)
         *
         * Solution:
         *   1. Each global resource is tagged with a GlobalSemantic value
         *   2. Use this map to look up where that semantic should be bound in the current shader
         *   3. Bind the resource to the retrieved set and binding index
         *
         * This provides a level of indirection, allowing the same logical resource to be placed at
         * different physical binding locations depending on the shader pipeline.
         */
        std::unordered_map< kege::GlobalSemantic, kege::ShaderResrcIndexPair > _global_semantic_map;

        /**
         * ShaderResrcIndexPair: A struct containing the set index and binding index.
         *
         * Problem: We need a way to look up binding information (set + binding indices)
         * using a human-readable identifier. A uniform like "Camera" may appear at different
         * set indices across different shader pipelines, but its string name remains consistent.
         *
         * Example:
         *   - Pipeline A: Camera at layout(set=0, binding=0) uniform Camera
         *   - Pipeline B: Camera at layout(set=2, binding=0) uniform Camera
         *
         * Solution:
         *   1. Map each uniform name to a ShaderResrcIndexPair containing its set and binding indices
         *
         * This ensures we can retrieve the correct binding location for each uniform in any shader.
         */
        std::unordered_map< std::string, kege::ShaderResrcIndexPair > _binding_map;

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
        std::unordered_map< kege::UniformKey, kege::SetIndex > _uniform_key_map;

        /**
         * _global_shader_resource_layouts: A list of all ShaderSetDesc designated as global resources.
         *
         * Problem: Querying which ShaderSetDesc are global for every shader pipeline each frame
         * is expensive. To avoid this overhead, I cache a list of pointers to them once at setup time.
         *
         * Note: Pointers are used to avoid copying and to maintain a single source of truth
         * if the same pair is referenced elsewhere.
         */
        std::vector< const kege::ShaderSetDesc* > _global_shader_resource_layouts;

        /**
         * _shader_resource_layouts: A list of all ShaderResourceSetLayout belonging
         * to this ShaderPipelineLayout.
         */
        std::vector< kege::ref::ShaderResourceSetLayout > _shader_resource_layouts;

        kege::PushBlockDescs _push_block_descs;
    };

}

namespace kege::ref
{
    using ShaderPipelineLayout = kege::Ref< kege::ShaderPipelineLayout >;
}
#endif /* shader_pipeline_resource_layout_hpp */

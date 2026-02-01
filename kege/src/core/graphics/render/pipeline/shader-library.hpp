//
//  shader-library.hpp
//  editor
//
//  Created by Kenneth Esdaile on 1/29/26.
//

#ifndef shader_library_hpp
#define shader_library_hpp

#include "json-parser.hpp"
#include "asset-manager.hpp"
#include "shader-data.hpp"
#include "file-io.hpp"

namespace kege{

    struct LibraryContext
    {
        struct Field
        {
            std::string name;
            std::string type;
            int count;
        };

        struct Attribute
        {
            std::string name;
            int location;
            int binding;
            ShaderVarType format;
            VertexInputRate input_rate;
        };
        struct AttributeBinding
        {
            int stride;
            int binding;
        };

        struct VertexAttribute
        {
            std::vector< Attribute > attributes;
            std::vector< AttributeBinding > bindings;
        };

        struct Struct
        {
            std::string name;
            std::vector< Field > fields;
        };

        struct Uniform
        {
            std::string name;
            std::vector< Field > fields;
            int shader_struct_block_index = -1;
        };

        struct Storage
        {
            std::string name;
            std::vector< Field > fields;
        };

        struct PushConst
        {
            int size;
            int count;
            int offset;
            std::string name;
            ShaderStageFlag stages;
            std::vector< Field > fields;

            int shader_struct_block_index = -1;
        };

        struct Shader
        {
            std::string uri;
            std::string name;
            std::string entry_point;
            ShaderStageFlag type;
        };

        struct BindConstant
        {
            std::string name;
            int push_constant;
            int count;
        };

        struct Block
        {
            std::string target;
            kege::BindingUsage usage;
            int index;
        };
        struct BindLayout
        {
            std::string name;
            Block block;
            int binding;
            int count;
            ShaderStageFlag stages;
        };

        struct BindSet
        {
            int set;
            std::vector< BindLayout > bindings;
        };

        struct Output
        {
            std::string name;
            std::string semantic;
            int location;
            Format format;
        };

        struct PipelineLayout
        {
            std::vector< BindConstant > push_constants;
            std::vector< BindSet > uniform_layouts;
        };

        struct Pipeline
        {
            PipelineType type;
            std::string name;
            PrimitiveTopology topology;
            int rasterizer;
            int depth_stencil;
            int color_blend;
            int pipeline_layout;
            int output;
            int attributes;
            std::vector< int > shaders;
            std::vector< int > outputs;
        };

        std::vector< kege::ref::ShaderStructBlock > _shader_struct_blocks;
        std::vector< RasterizationStateDesc > rasterizer_states;
        std::vector< DepthStencilStateDesc > depth_stencil_states;
        std::vector< ColorBlendStateDesc > color_blend_states;
        std::vector< Shader > shaders;
        std::vector< VertexAttribute > attributes;
        std::vector< Struct > structs;
        std::vector< Uniform > uniforms;
        std::vector< Storage > storages;
        std::vector< PushConst > push_constants;
        std::vector< PipelineLayout > pipeline_layouts;
        std::vector< Output > outputs;
        std::vector< Pipeline > pipelines;
        std::string path;
    };

    bool loadLibraryContext(kege::LibraryContext& context, const std::string& filename);

    ref::ShaderPipeline createShaderPipeline
    (
        kege::Graphics* graphics,
        std::vector< kege::ref::ShaderStructBlock >& shader_struct_blocks,
        kege::LibraryContext& context,
        uint64_t pipeline_index
    );

}


namespace kege{

    class ShaderLibrary : public AssetLibrary<ref::ShaderPipeline>
    {
    public:

        bool load( const std::string& filename );
        ref::ShaderPipeline* find( uint32_t guid );
        ShaderLibrary(kege::Graphics* graphics, uint64_t error_shader_handle);
        ~ShaderLibrary();
        
    private:

        struct Record
        {
            const std::string filename;
            uint64_t physical_handle;
            uint64_t pipeline_index;
        };

    private:

        std::vector<kege::ref::ShaderStructBlock> _shader_struct_blocks;
        LibraryContext _context;
        std::vector<Record> _records;
        kege::Graphics* _graphics;
        uint64_t _error_shader_handle;
    };

}
#endif /* shader_library_hpp */

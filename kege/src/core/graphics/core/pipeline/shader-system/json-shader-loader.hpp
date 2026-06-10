//
//  json-to-shader.hpp
//  editor
//
//  Created by Kenneth Esdaile on 2/16/26.
//

#ifndef json_shader_loader_hpp
#define json_shader_loader_hpp

#include <iostream>
#include <fstream>
#include "../../../../resource/asset-manager.hpp"
#include "../../../../io/file-io.hpp"
#include "../../../../utils/json-parser.hpp"
#include "../../device/core/device.hpp"

namespace kege{

    struct JsonShaderContent
    {
        struct Field
        {
            std::string name;
            std::string type;
            int count;
        };

        /**
         * -------- STRUCT --------
         */

        struct Struct
        {
            std::string name;
            std::vector< Field > fields;
        };

        /**
         * -------- PUSH BLOCKS --------
         */

        struct PushBlockDesc
        {
            int size;
            int count;
            int offset;
            std::string id;
            std::string name;
            ShaderStageFlag stages;
            std::vector< Field > fields;
        };

        /**
         * -------- SETS --------
         */

        struct Set
        {
            struct Binding
            {
                std::vector< Field > fields;
                ShaderStageFlag stages;
                BindingUsage usage;
                std::string id;
                std::string name;
                int index;
                int count;
                kege::ArrayType array_type;
            };

            int index;
            std::vector< Set::Binding > bindings;
        };

        /**
         * -------- VERTEX ATTRIBUTE --------
         */

        struct Attribute
        {
            std::string name;
            int location;
            int binding;
            ShaderVar format;
            VertexBit semantic;
        };

        struct AttributeBinding
        {
            int stride;
            VertexInputRate input_rate;
        };

        struct VertexAttribute
        {
            std::vector< Attribute > attributes;
            std::vector< AttributeBinding > bindings;
        };

        /**
         * -------- SHADER --------
         */

        struct Shader
        {
            std::string uri;
            std::string name;
            std::string entry_point;
            ShaderStageFlag type;
        };

        struct Output
        {
            std::string name;
            std::string semantic;
            int location;
            Format format;
        };

        struct GlobalSet
        {
            int set;
            int binding;
            kege::GlobalSemantic semantic;
        };

        struct PipelineLayout
        {
            std::vector< GlobalSet > global_bindings;
            std::vector< int > blocks;
            std::vector< int > sets;
        };

        struct Pipeline
        {
            PipelineType type;
            std::string name;
            PrimitiveTopology topology;
            int pipeline_output;
            int pipeline_layout;
            int rasterizer_state;
            int depth_state;
            int blend_state;
            int attributes;
            std::vector< int > shaders;
            std::vector< int > outputs;
        };

        std::vector< kege::PipelineRendering > pipeline_outputs;
        std::vector< PipelineLayout > pipeline_layouts;
        std::vector< RasterizerState > rasterizer_states;
        std::vector< DepthStencil > depth_states;
        std::vector< ColorBlendState > blend_states;
        std::vector< VertexAttribute > attributes;
        std::vector< PushBlockDesc > push_blocks;
        std::vector< Set > sets;
        std::vector< Shader > shaders;
        std::vector< Struct > structs;
        std::vector< Pipeline > pipelines;
        std::string error;


        static kege::JsonShaderContent load(const std::string& filename);
        static kege::JsonShaderContent parse(const Json& json);
    };

    kege::ShaderStageFlag parseShaderStages( kege::Json json );

    kege::FaceOperations parseStencilOpState( const kege::Json& json );

    kege::DepthStencil parseDepthStencilStates( const kege::Json& json );

    kege::BlendState getColorBlendAttachmentState( const kege::Json& json );

    kege::ColorBlendState parseColorBlendStates( const kege::Json& json );

    kege::RasterizerState parseRasterizationState( const kege::Json& json );

}

namespace kege{

    class JsonShaderLoader : public kege::AssetLoaderT< ref::ShaderPipeline >
    {
    public:

        ref::ShaderPipeline load( const std::string parent_path, const JsonShaderContent content, int index );
        ref::ShaderPipeline load( const std::string& filename );
        void operator()( kege::GraphicsDevice* response );

        JsonShaderLoader( AssetManager* am );

        kege::GraphicsDevice* _graphics;
    };
}
#endif /* json_shader_loader_hpp */

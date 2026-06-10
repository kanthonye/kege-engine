//
//  meta-shader-parser.hpp
//  editor
//
//  Created by Kenneth Esdaile on 2/2/26.
//

#ifndef meta_shader_parser_hpp
#define meta_shader_parser_hpp

#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <variant>
#include <memory>
#include <optional>
#include "meta-shader-lexer.hpp"
#include "../../device/core/device.hpp"

namespace kege::meta::shader{

    // ========== AST Nodes ==========
//    struct AttributeDecl {
//        std::string type;
//        int offset;
//        int location;
//        int binding;
//    };
//
//    struct VBODecl
//    {
//        int index;
//        int stride;
//        std::string rate;
//    };
//
    struct FieldDecl
    {
        std::string name;
        std::string type;
        int count = 1;
    };

    enum Stage
    {
        VERT = 1,
        FRAG = 2,
        GEOM = 4,
        TESE = 8,
        TESC = 16,
        COMP = 32,
    };

//    struct BindingDecl
//    {
//        std::string type;
//        std::string block_name;
//        std::string name;
//        int index;
//        int count;
//        std::string usage;
//        int stages;
//        std::vector<FieldDecl> members; // Simplified
//    };
//
//    struct Set
//    {
//        int index;
//        std::string name;
//        std::vector<BindingDecl> bindings; // Simplified
//    };
//
//    struct PushRange
//    {
//        int stages;
//        int count;
//        int offset;
//        int size;
//        std::string name;
//        std::string block_name;
//        std::vector<FieldDecl> members; // Simplified
//    };
//
//    struct VertexLayout {
//        std::vector<AttributeDecl> attributes;
//        std::vector<VBODecl> vbos;
//    };
//
//    struct ShaderStage {
//        std::string type;
//        std::string code;
//        std::string entry;
//    };
//
    using Properties = std::unordered_map<std::string, std::vector<std::string>>;
    using Block = std::map<std::string, std::string>;

    struct Struct
    {
        ShaderStageFlag stages;
        std::vector<FieldDecl> mambers;
    };
    using Structs = std::map<std::string, Struct>;

//    struct BlendState
//    {
//        int index;
//        Block params;
//    };

//    struct ColorBlendState
//    {
//        struct Attachment
//        {
//            int index;
//            Block params;
//        };
//
//        Block params;
//        std::vector< Attachment > attachments;
//    };



    struct Const
    {
        int index;
        std::string name;
        std::string value;
        std::string type;
    };
}


namespace kege::meta::shader{

    // ========== Main AST ==========
    struct AST
    {
        ShaderPipelineDesc* pipeline_description;
        ShaderSources* sources;

        std::map<std::string,std::vector<Const>> specialization_constants;

        Structs structs;
    };
}

namespace kege::meta::shader{

    // ========== Parser ==========
    class Parser
    {
    public:

        struct Set
        {
            ShaderSetDesc desc;
            std::string name;
            int index;
        };

        Parser(const std::string& source)
        : _lexer(source), _current_token(_lexer.nextToken()) {}

        ShaderPipelineDesc* parse(ShaderPipelineDesc& pipeline_description, ShaderSources& sources);

    private:

        Token expect(TokenType expected, const std::string& error_msg, const char* caller);
        void consume(TokenType expected, const char* caller);
        bool match(TokenType type);
        bool check(TokenType type) const;
        Token advance();

        // Parsing methods
        void parse_meta(AST& ast);
        void parse_structs(AST& ast);
        void parseVertexLayout(AST& ast);
        void parseSets(AST& ast);
        void parseSpecializationConstants(AST& ast);
        void parseShaderStage(AST& ast);
        //void parse_pipeline_states(AST& ast);
        void parsePushBlocks(AST& ast);
        void parseDepthStencil(AST& ast);
        void parseStencil(Stencil&);
        void parseDepth(Depth&);
        void parseFaceOperations(FaceOperations&);
        void parseColorBlend(AST& ast);
        void parseInputAssembly(AST& ast);
        void parseRasterization(AST& ast);
        void parseMultiSample(AST& ast);
        void parseRenderPass(AST& ast);
        void parseDynamicState(AST& ast);
        void parseDynamicRendering(AST& ast);

        Properties parseEnclosedBracesParameters();
        std::vector<std::string> parseArray();
        Const parseConst();

        void parseSet(ShaderSetDesc& desc, std::string& name, int& set_index);
        ShaderSetBindingDesc parseBinding();
        PushBlockDesc parsePushRange();
        std::string parseAny();
        int parseInt();
        //BufferDecl parse_buffer();
        std::vector<FieldDecl> parse_members();

        ShaderStageFlag toShaderStages(const std::vector<std::string>& s);
        int parse_quantity();

        void parse_block(Block& members);

        Properties parse_properties();

    private:

        Lexer _lexer;
        Token _current_token;
    };

}


//namespace kege::meta::shader{
//
//    // ========== Code Generator ==========
//    class CodeGenerator
//    {
//    public:
//
//        struct GeneratedCode
//        {
//            PipelineStates pipeline_states;
//            VertexLayout vertex_attributes;
//            Structs structs;
//
//            std::string vertex_shader;
//            std::string fragment_shader;
//            std::string tessellation_evaluation_shader;
//            std::string tessellation_control_shader;
//            std::string geometry_shader;
//            std::string compute_shader;
//            std::string pipeline_json;
//            std::string reflection_data;
//        };
//
//        GeneratedCode generate(const AST& ast);
//
//    private:
//
//        void generate_structs(std::stringstream& ss, const Structs& structs, int stage);
//        void generate_fields(std::stringstream& ss, const std::vector<FieldDecl>& members);
//        void generate_shader_resources(std::stringstream& ss, const AST& ast, int stage);
//        std::string generate_vertex_shader(const AST& ast);
//        std::string generate_fragment_shader(const AST& ast);
//        std::string generate_tessellation_evaluation_shader(const AST& ast);
//        std::string generate_tessellation_control_shader(const AST& ast);
//        std::string generate_geometry_shader(const AST& ast);
//        std::string generate_compute_shader(const AST& ast);
//        std::string generate_pipeline_json(const AST& ast);
//        std::string generate_reflection(const AST& ast);
//
//        std::string generate_attribute_decls(const VertexLayout& vf);
////        std::string generate_resource_decls(const Resources& res);
//    };
//
//} // namespace MetaShader


#endif /* meta_shader_parser_hpp */

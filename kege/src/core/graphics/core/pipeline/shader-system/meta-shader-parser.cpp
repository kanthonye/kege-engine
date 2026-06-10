//
//  meta-shader-parser.cpp
//  editor
//
//  Created by Kenneth Esdaile on 2/2/26.
//

#include "meta-shader-parser.hpp"
#include <sstream>
#include <iostream>
#include <cctype>

namespace kege::meta::shader{

#define EXPECTED(tok, msg) this->expect(tok, msg, __FUNCTION__)
#define CONSUME(tok) this->consume(tok, __FUNCTION__)

    // ========== Parser Implementation ==========
    ShaderPipelineDesc* Parser::parse(ShaderPipelineDesc& pipeline_description, ShaderSources& sources)
    {
        AST ast = {.pipeline_description = &pipeline_description, .sources = &sources};
        while (_current_token.type != TokenType::END_OF_FILE)
        {
            if (match(TokenType::AT))
            {
                switch (_current_token.type)
                {
                    case TokenType::KW_METADATA:
                        parse_meta(ast);
                        break;

                    case TokenType::KW_FRAG:
                    case TokenType::KW_VERT:
                    case TokenType::KW_GEOM:
                    case TokenType::KW_TESC:
                    case TokenType::KW_TESE:
                        parseShaderStage(ast);
                        break;

                    case TokenType::KW_VERTEX_LAYOUT:
                        parseVertexLayout(ast);
                        break;

                    case TokenType::KW_SETS:
                        parseSets(ast);
                        break;

                    case TokenType::KW_PUSH_BLOCKS:
                        parsePushBlocks(ast);
                        break;

                    case TokenType::KW_SPECIALIZATION_CONSTANTS:
                        parseSpecializationConstants(ast);
                        break;

                    case TokenType::KW_INPUT_ASSEMBLY:
                        parseInputAssembly(ast);
                        break;

                    case TokenType::KW_RASTERIZATION:
                        parseRasterization(ast);
                        break;

                    case TokenType::KW_MULTISAMPLE:
                        parseMultiSample(ast);
                        break;

                    case TokenType::KW_DEPTH_STENCIL:
                        parseDepthStencil(ast);
                        break;

                    case TokenType::KW_COLOR_BLEND:
                        parseColorBlend(ast);
                        break;

                    case TokenType::KW_DYNAMIC_STATES:
                        parseDynamicState(ast);
                        break;

                    case TokenType::KW_DYNAMIC_RENDERING:
                        parseDynamicRendering(ast);
                        break;

                    case TokenType::KW_STRUCTS:
                        parse_structs(ast);
                        break;

                    default:
                        throw std::runtime_error("kege::meta::shader::Parser::parse(): " + std::string("Expected keyword after @") );
                        break;
                }
            }
            else
            {
                advance();
            }
        }

        return &pipeline_description;
    }

    void Parser::parse_meta(AST& ast)
    {
        CONSUME(TokenType::KW_METADATA);
        Properties params = parseEnclosedBracesParameters();
        auto it = params.find("pipeline");
        if (it != params.end())
        {
            ast.pipeline_description->pipeline_type = stringToPipelineType( it->second[0] );
        }
        it = params.find("name");
        if (it != params.end())
        {
            ast.pipeline_description->name = it->second[0];
        }
    }

    void Parser::parse_structs(AST& ast)
    {
        CONSUME(TokenType::KW_STRUCTS);
        CONSUME(TokenType::LBRACE);

        Structs structs;

        while (_current_token.type != TokenType::RBRACE && _current_token.type != TokenType::END_OF_FILE)
        {
            if (match(TokenType::AT))
            {
                if (_current_token.type == TokenType::KW_STRUCT)
                {
                    advance();
                    Properties params = parse_properties();

                    Struct& struc = structs[ _current_token.lexeme ];
                    CONSUME(TokenType::IDENTIFIER);
                    
                    struc.stages = toShaderStages(params[ "stages" ]);

                    struc.mambers = parse_members();
                    expect(TokenType::SEMICOLON, "Expected ';' at end of struct", __FUNCTION__);
                }
                else expect(TokenType::KW_ATTRIBUTE, "Expected directive in vertex_attributes", __FUNCTION__);
            }
            else
            {
                advance(); // Skip
            }
        }

        EXPECTED(TokenType::RBRACE, "Expected '}' after vertex_attributes");
        ast.structs = structs;
    }

    void Parser::parseVertexLayout(AST& ast)
    {
        CONSUME(TokenType::KW_VERTEX_LAYOUT);
        CONSUME(TokenType::LBRACE);

        while (_current_token.type != TokenType::RBRACE && _current_token.type != TokenType::END_OF_FILE)
        {
            switch (_current_token.type)
            {
                case TokenType::AT:
                {
                    CONSUME(TokenType::AT);
                    if (_current_token.type == TokenType::KW_ATTRIBUTE)
                    {
                        VertexAttribute attr;
                        CONSUME(TokenType::KW_ATTRIBUTE);
                        CONSUME(TokenType::LPAREN);
                        attr.location = parseInt();
                        CONSUME(TokenType::RPAREN);

                        Properties params = parseEnclosedBracesParameters();

                        auto it = params.find("type");
                        if (it != params.end()) {
                            attr.type = stringToShaderVarType(it->second[0]);
                        }

                        //it = params.find("offset");
                        //if (it != params.end()) {
                        //    attr.offset = std::stoi(it->second[0]);
                        //}

                        it = params.find("binding");
                        if (it != params.end()) {
                            attr.binding = std::stoi(it->second[0]);
                        }
                        ast.pipeline_description->vertex_layout.attributes.push_back(attr);
                    }
                    else if (_current_token.type == TokenType::KW_BINDING)
                    {
                        VertexInput input;
                        CONSUME(TokenType::KW_BINDING);
                        CONSUME(TokenType::LPAREN);
                        input.index = parseInt();
                        CONSUME(TokenType::RPAREN);

                        Properties params = parseEnclosedBracesParameters();

                        auto it = params.find("stride");
                        if (it != params.end())
                        {
                            input.stride = std::stoi(it->second[0]);
                        }

                        it = params.find("rate");
                        if (it != params.end())
                        {
                            input.input_rate = stringToVertexInputRate(it->second[0]);
                        }
                        ast.pipeline_description->vertex_layout.input_rates.push_back(input);
                    }
                    else EXPECTED(TokenType::KW_ATTRIBUTE, "Expected directive in vertex_attributes");
                    break;
                }

                default: advance(); break;
            }
        }
        EXPECTED(TokenType::RBRACE, "Expected '}' after vertex_attributes");
    }

    void Parser::parseSets(AST& ast)
    {
        CONSUME(TokenType::KW_SETS);
        CONSUME(TokenType::LBRACE);
        while (_current_token.type != TokenType::RBRACE && _current_token.type != TokenType::END_OF_FILE)
        {
            if (match(TokenType::AT))
            {
                if (_current_token.type == TokenType::KW_SET)
                {
                    ShaderSetDesc desc;
                    std::string name;
                    int set_index;
                    parseSet( desc, name, set_index );
                    ast.pipeline_description->pipeline_layout.resource_layout_desc.push_back( desc );
                }
                else
                {
                    EXPECTED(TokenType::KW_SET, "Expected directive in resources");
                }
            }
            else
            {
                advance(); // Skip
            }
        }
        EXPECTED(TokenType::RBRACE, "Expected '}' after vertex_attributes");
    }

    int Parser::parseInt()
    {
        int num = std::stoi(_current_token.lexeme);
        CONSUME(TokenType::NUMBER);
        return num;
    }

    std::string Parser::parseAny()
    {
        std::string id = _current_token.lexeme;
        advance();
        return id;
    }
    void Parser::parseSet(ShaderSetDesc& desc, std::string& name, int& set_index)
    {
        ShaderSetDesc set;
        CONSUME(TokenType::KW_SET);
        CONSUME(TokenType::LPAREN);
        set_index = parseInt();
        CONSUME(TokenType::RPAREN);
        if (_current_token.type != TokenType::LBRACE)
        {
            name = parseAny();
        }
        CONSUME(TokenType::LBRACE);

        while (_current_token.type != TokenType::RBRACE && _current_token.type != TokenType::END_OF_FILE)
        {
            if (match(TokenType::AT))
            {
                if ( _current_token.type == TokenType::KW_BINDING )
                {
                    desc.push_back( parseBinding() );
                }
                else EXPECTED(TokenType::KW_BINDING, "Expected 'binding' after @");
            }
            else
            {
                advance(); // Skip
            }
        }

        EXPECTED(TokenType::RBRACE, "Expected '}' after resources");
    }

    ShaderSetBindingDesc Parser::parseBinding()
    {
        ShaderSetBindingDesc binding;

        CONSUME(TokenType::KW_BINDING);
        CONSUME(TokenType::LPAREN);
        binding.binding_index = parseInt();
        CONSUME(TokenType::RPAREN);
        Properties params = parseEnclosedBracesParameters();

        auto it = params.find("usage");
        if (it != params.end()) {
            binding.usage = stringToBindingUsage( it->second[0] );
        }

        it = params.find("stages");
        if (it != params.end())
        {
            binding.stages = toShaderStages(it->second);
        }

        it = params.find("count");
        if (it != params.end()) {
            binding.binding_count = std::stoi(it->second[0]);
        }

        it = params.find("id");
        if (it != params.end()) {
            binding.name = it->second[0];
        }

        return binding;
    }

    void Parser::parsePushBlocks(AST& ast)
    {
        CONSUME(TokenType::KW_PUSH_BLOCKS);
        CONSUME(TokenType::LBRACE);
        while (_current_token.type != TokenType::RBRACE && _current_token.type != TokenType::END_OF_FILE)
        {
            if (match(TokenType::AT))
            {
                if ( _current_token.type == TokenType::KW_RANGE )
                {
                    ast.pipeline_description->pipeline_layout.push_block_desc.push_back( parsePushRange() );
                }
                else EXPECTED(TokenType::KW_BINDING, "Expected 'binding' after @");
            }
            else
            {
                advance(); // Skip
            }
        }
        EXPECTED(TokenType::RBRACE, "Expected '}' after resources");
    }

    PushBlockDesc Parser::parsePushRange()
    {
        PushBlockDesc range;
        CONSUME(TokenType::KW_RANGE);
        if (_current_token.type == TokenType::STRING_LITERAL)
        {
            range.name = parseAny();
        }
        Properties params = parseEnclosedBracesParameters();

        // Extract from props
        auto it = params.find("offset");
        if (it != params.end()) range.offset = std::stoi(it->second[0]);

        it = params.find("size");
        if (it != params.end()) range.size = std::stoi(it->second[0]);

        it = params.find("stages");
        if (it != params.end())
        {
            range.stages = toShaderStages(it->second);
        }
        return range;
    }

    void Parser::parseSpecializationConstants(AST& ast)
    {
        CONSUME(TokenType::KW_SPECIALIZATION_CONSTANTS);
        CONSUME(TokenType::LBRACE);
        while (_current_token.type != TokenType::RBRACE && _current_token.type != TokenType::END_OF_FILE)
        {
            if (match(TokenType::AT))
            {
                switch (_current_token.type)
                {
                    case TokenType::KW_FRAG:
                    case TokenType::KW_VERT:
                    {
                        auto& specialization_constants = ast.specialization_constants[ _current_token.lexeme ];
                        advance();
                        CONSUME(TokenType::LBRACE);
                        while (_current_token.type == TokenType::AT && _current_token.type != TokenType::END_OF_FILE)
                        {
                            advance();
                            specialization_constants.push_back( parseConst() );
                        }
                        EXPECTED(TokenType::RBRACE, "Expected '}' after resources");
                        break;
                    }
                    default: EXPECTED(TokenType::KW_VERT, "Expected 'binding' after @"); break;
                }
            }
            else
            {
                advance(); // Skip
            }
        }
        EXPECTED(TokenType::RBRACE, "Expected '}' after resources");
    }

    Const Parser::parseConst()
    {
        Const cnst;
        CONSUME(TokenType::KW_CONST);
        CONSUME(TokenType::LPAREN);
        cnst.index = parseInt();
        CONSUME(TokenType::RPAREN);

        cnst.type = _current_token.lexeme;
        advance();

        cnst.name = _current_token.lexeme;
        advance();

        cnst.value = _current_token.lexeme;
        advance();

        return cnst;
    }
    
    void Parser::parseInputAssembly(AST& ast)
    {
        CONSUME(TokenType::KW_INPUT_ASSEMBLY);
        Properties params = parseEnclosedBracesParameters();
        Properties::iterator m;

        m = params.find("topology");
        if (m != params.end())
        {
            ast.pipeline_description->input_assembly.topology = stringToPrimitiveTopology(m->second[0]);
        }

        m = params.find("primitive_restart");
        if (m != params.end())
        {
            ast.pipeline_description->input_assembly.primitive_restart = m->second[0] == "on";
        }
    }

    void Parser::parseRasterization(AST& ast)
    {
        CONSUME(TokenType::KW_RASTERIZATION);
        CONSUME(TokenType::LBRACE);
        Properties props;
        while (_current_token.type != TokenType::RBRACE && _current_token.type != TokenType::END_OF_FILE)
        {
            if (match(TokenType::AT))
            {
                switch (_current_token.type)
                {
                    case TokenType::KW_DEPTH_BIAS:
                    {
                        CONSUME(TokenType::KW_DEPTH_BIAS);
                        Properties params = parseEnclosedBracesParameters();

                        auto m = params.find( "enable" );
                        ast.pipeline_description->rasterizer.depth_bias.enable = (m != params.end())? (m->second[0] == "on") : false;

                        m = params.find( "constant_factor" );
                        ast.pipeline_description->rasterizer.depth_bias.constant_factor = (m != params.end())? std::stof(m->second[0]) : 0.f;

                        m = params.find( "slope_factor" );
                        ast.pipeline_description->rasterizer.depth_bias.slope_factor = (m != params.end())? std::stof(m->second[0]) : 0.f;

                        m = params.find( "clamp" );
                        ast.pipeline_description->rasterizer.depth_bias.clamp = (m != params.end())? std::stof(m->second[0]) : 0.f;
                        break;
                    }
                    default: EXPECTED(TokenType::KW_DEPTH_BIAS, "Expected 'binding' after @"); break;
                }
            }
            else if (_current_token.type == TokenType::IDENTIFIER)
            {
                auto& m = props[ parseAny() ];
                CONSUME(TokenType::COLON);
                m.push_back(parseAny());
            }
            else
            {
                advance(); // Skip
            }
        }
        auto m = props.find( "enable" );
        ast.pipeline_description->rasterizer.depth_clamp_enable = (m != props.end())? (m->second[0] == "on") : false;

        m = props.find( "discard" );
        ast.pipeline_description->rasterizer.discard = (m != props.end())? (m->second[0] == "on") : false;

        m = props.find( "line_width" );
        ast.pipeline_description->rasterizer.line_width = (m != props.end())? std::stoi(m->second[0]) : 1;

        m = props.find( "front_face" );
        if(m != props.end())
        {
            ast.pipeline_description->rasterizer.front_face = stringToFrontFace( m->second[0] );
        }

        m = props.find( "cull_mode" );
        if(m != props.end())
        {
            ast.pipeline_description->rasterizer.cull_mode = stringToCullMode( m->second[0] );
        }

        m = props.find( "polygon_mode" );
        if(m != props.end())
        {
            ast.pipeline_description->rasterizer.fill_mode = stringToPolygonMode( m->second[0] );
        }

        EXPECTED(TokenType::RBRACE, "Expected '}' after resources");
    }

    void Parser::parseMultiSample(AST& ast)
    {
        CONSUME(TokenType::KW_MULTISAMPLE);
        Properties params = parseEnclosedBracesParameters();
        Properties::iterator m;

        m = params.find("samples");
        if (m != params.end())
        {
            ast.pipeline_description->multisample.sample_count = (SampleCount) std::stoi(m->second[0]);
        }

        m = params.find("sample_shading");
        if (m != params.end())
        {
            ast.pipeline_description->multisample.sample_shading = m->second[0] == "on" ? true : false;
        }

        m = params.find("alpha_to_coverage");
        if (m != params.end())
        {
            ast.pipeline_description->multisample.alpha_to_coverage = m->second[0] == "on" ? true : false;
        }

        m = params.find("alpha_to_one");
        if (m != params.end())
        {
            ast.pipeline_description->multisample.alpha_to_one = m->second[0] == "on" ? true : false;
        }
    }

    void Parser::parseDepthStencil(AST& ast)
    {
        CONSUME(TokenType::KW_DEPTH_STENCIL);
        CONSUME(TokenType::LBRACE);
        while (_current_token.type != TokenType::RBRACE && _current_token.type != TokenType::END_OF_FILE)
        {
            if (match(TokenType::AT))
            {
                switch (_current_token.type)
                {
                    case TokenType::KW_DEPTH:
                    {
                        CONSUME(TokenType::KW_DEPTH);
                        Properties params = parseEnclosedBracesParameters();

                        auto m = params.find( "enable" );
                        ast.pipeline_description->depth_stencil.depth.enable = (m != params.end())? (m->second[0] == "on") : false;

                        m = params.find( "write" );
                        ast.pipeline_description->depth_stencil.depth.write = (m != params.end())? (m->second[0] == "on") : false;

                        m = params.find( "bounds_test" );
                        ast.pipeline_description->depth_stencil.depth.bounds_test = (m != params.end())? (m->second[0] == "on") : false;

                        m = params.find( "bounds" );
                        ast.pipeline_description->depth_stencil.depth.bounds.min = std::stoi(m->second[0]);
                        ast.pipeline_description->depth_stencil.depth.bounds.max = std::stoi(m->second[1]);

                        m = params.find( "compare_op" );
                        ast.pipeline_description->depth_stencil.depth.compare_op = stringToCompareOp(m->second[0]);
                        break;
                    }
                    case TokenType::KW_STENCIL:
                    {
                        parseStencil(ast.pipeline_description->depth_stencil.stencil);
                        break;
                    }
                    default: EXPECTED(TokenType::KW_DEPTH_BIAS, "Expected 'binding' after @"); break;
                }
            }
            else
            {
                advance(); // Skip
            }
        }
        EXPECTED(TokenType::RBRACE, "Expected '}' after resources");
    }

    void Parser::parseStencil(Stencil& stencil)
    {
        Properties props;
        CONSUME(TokenType::KW_STENCIL);
        CONSUME(TokenType::LBRACE);

        while (_current_token.type != TokenType::RBRACE && _current_token.type != TokenType::END_OF_FILE)
        {
            if (match(TokenType::AT))
            {
                switch (_current_token.type)
                {
                    case TokenType::KW_FRONT:
                    {
                        parseFaceOperations(stencil.front);
                        break;
                    }
                    case TokenType::KW_BACK:
                    {
                        parseFaceOperations(stencil.back);
                        break;
                    }
                    default: EXPECTED(TokenType::ERROR, "Expected 'front' or 'back' after @"); break;
                }
            }
            else if(_current_token.type == TokenType::IDENTIFIER)
            {
                auto& m = props[ parseAny() ];
                CONSUME(TokenType::COLON);
                m.push_back(parseAny());
            }
        }
        auto m = props.find( "enable" );
        stencil.enable = (m != props.end())? (m->second[0] == "on") : false;

        m = props.find( "reference" );
        stencil.reference = (m != props.end())? std::stof(m->second[0]) : 0.f;
    }

    void  Parser::parseFaceOperations(FaceOperations& face)
    {
        advance();
        Properties params = parseEnclosedBracesParameters();
        face.fail_op = stringToStencilOp( params["fail_op"][0] );
        face.pass_op = stringToStencilOp( params["pass_op"][0] );
        face.depth_fail_op = stringToStencilOp( params["depth_fail_op"][0] );
        face.compare_op = stringToCompareOp( params["compare_op"][0] );

        auto m = params.find("compare_mask");
        face.compare_mask = (m != params.end())? std::stoi(m->second[0]): 0xFF;

        m = params.find("write_mask");
        face.write_mask = (m != params.end())? std::stoi(m->second[0]): 0xFF;
    }

    void Parser::parseColorBlend(AST& ast)
    {
        CONSUME(TokenType::KW_COLOR_BLEND);
        CONSUME(TokenType::LBRACE);
        Properties props;

        while (_current_token.type != TokenType::RBRACE && _current_token.type != TokenType::END_OF_FILE)
        {
            if (match(TokenType::AT))
            {
                switch (_current_token.type)
                {
                    case TokenType::KW_ATTACHMENT:
                    {
                        CONSUME(TokenType::KW_ATTACHMENT);
                        CONSUME(TokenType::LPAREN);
                        CONSUME(TokenType::NUMBER);
                        CONSUME(TokenType::RPAREN);
                        Properties params = parseEnclosedBracesParameters();

                        BlendState blend;
                        auto m = params.find( "enable" );
                        blend.enable = (m != params.end())? (m->second[0] == "on") : false;

                        //m = params.find( "color_mask" );
                        //blend.color_write_mask = (m != params.end())? (m->second[0] == "on") : false;

                        m = params.find( "src_color" );
                        blend.src_color_blend = (m != params.end())? stringToBlendFactor(m->second[0]) : BlendFactor::SrcAlpha;

                        m = params.find( "dst_color" );
                        blend.dst_color_blend =  (m != params.end())? stringToBlendFactor(m->second[0]) : BlendFactor::OneMinusSrcAlpha;

                        m = params.find( "color_op" );
                        blend.color_blend_op = (m != params.end())? stringToBlendOp(m->second[0]) : BlendOp::Add;

                        m = params.find( "src_alpha" );
                        blend.src_alpha_blend = (m != params.end())? stringToBlendFactor(m->second[0]) : BlendFactor::One;

                        m = params.find( "dst_alpha" );
                        blend.dst_alpha_blend = (m != params.end())? stringToBlendFactor(m->second[0]) : BlendFactor::Zero;

                        m = params.find( "alpha_op" );
                        blend.alpha_blend_op = (m != params.end())? stringToBlendOp(m->second[0]) : BlendOp::Add;

                        ast.pipeline_description->color_blend.attachments.push_back(blend);
                        break;
                    }
                    default: EXPECTED(TokenType::ERROR, "Expected 'attachment' after @"); break;
                }
            }
            else if(_current_token.type == TokenType::IDENTIFIER)
            {
                auto& m = props[ parseAny() ];
                CONSUME(TokenType::COLON);
                if (_current_token.type == TokenType::LBRACKET)
                {
                    m = parseArray();
                }
                else
                {
                    m.push_back(parseAny());
                }
            }
        }

        auto m = props.find( "logic_op" );
        ast.pipeline_description->color_blend.logic_op = (m != props.end())? stringToColorBlendLogicOp(m->second[0]) : ColorBlendLogicOp::Copy;

        m = props.find( "blend_constants" );
        if(m != props.end())
        {
            ast.pipeline_description->color_blend.blend_constants[0] = std::stof(m->second[0]);
            ast.pipeline_description->color_blend.blend_constants[1] = std::stof(m->second[1]);
            ast.pipeline_description->color_blend.blend_constants[2] = std::stof(m->second[2]);
            ast.pipeline_description->color_blend.blend_constants[3] = std::stof(m->second[3]);
        }

        EXPECTED(TokenType::RBRACE, "Expected '}' after resources");
    }
    void Parser::parseRenderPass(AST& ast)
    {
        CONSUME(TokenType::KW_DYNAMIC_STATES);
        CONSUME(TokenType::LBRACE);
        while (_current_token.type != TokenType::RBRACE && _current_token.type != TokenType::END_OF_FILE)
        {
            advance();
        }
        expect(TokenType::RBRACE, "Expected '}'", __FUNCTION__);
    }

    void Parser::parseDynamicState(AST& ast)
    {
        CONSUME(TokenType::KW_DYNAMIC_STATES);
        CONSUME(TokenType::LBRACE);
        while (_current_token.type != TokenType::RBRACE && _current_token.type != TokenType::END_OF_FILE)
        {
            ast.pipeline_description->dynamic_states.push_back( stringToDynamicState(_current_token.lexeme) );
            advance();
        }
        expect(TokenType::RBRACE, "Expected '}'", __FUNCTION__);
    }

    void Parser::parseDynamicRendering(AST& ast)
    {
        CONSUME(TokenType::KW_DYNAMIC_RENDERING);
        Properties props = parseEnclosedBracesParameters();

        auto& attachments = props["color_attachments"];
        for (auto a : attachments) ast.pipeline_description->pipeline_rendering.color_attachment_formats.push_back(stringToFormat(a));
        ast.pipeline_description->pipeline_rendering.stencil_attachment_format = stringToFormat(props["stencil_attachment"][0]);
        ast.pipeline_description->pipeline_rendering.depth_attachment_format = stringToFormat(props["depth_attachment"][0]);
        //props["pass"];
    }

    bool skipHorizontalWhitespace(char c) {
        // Loop as long as the current character is a space or a tab
        return false;//c == ' ' || c == '\t';
    }

    void Parser::parseShaderStage(AST& ast)
    {
        Token token = advance();
        std::string stage = token.lexeme;
        Properties properties = parse_properties();

        std::stringstream code;
        TokenType prev = token.type;
        token = advance();

        const char* white_space = _lexer.getWhiteSpace();
        _lexer.setWhiteSpace("$");
        code << "#version " << properties["version"][0] <<"\n";

        int space_count = 0;
        int nl_count = 0;
        while (token.type != TokenType::KW_END && _current_token.type != TokenType::END_OF_FILE)
        {
            if (token.type == TokenType::SPACE )
            {
                space_count += 1;
                prev = token.type;
                token = _current_token;
                _current_token = _lexer.nextToken();
                continue;
            }

            if (token.type == TokenType::NEWLINE )
            {
                if(space_count != 0)
                {
                    space_count = 0;
                    prev = token.type;
                    token = _current_token;
                    _current_token = _lexer.nextToken();
                    nl_count += 1;
                    if(nl_count == 1) code << "\n";
                    continue;
                }
                nl_count += 1;
                if(nl_count >= 2)
                {
                    space_count = 0;
                    prev = token.type;
                    token = _current_token;
                    _current_token = _lexer.nextToken();
                    continue;
                }
            }
            else
            {
                nl_count = 0;
            }

            code << std::string(space_count, ' ') << token.lexeme;
            space_count = 0;

            prev = token.type;
            token = _current_token;
            _current_token = _lexer.nextToken();
            if (token.type == TokenType::AT)
            {
                break;
            }
        }
        CONSUME(TokenType::KW_END);

        ast.sources->push_back
        ({
            .name   = stage,
            .source = code.str(),
            .entry  = properties["entry"][0],
            .stage  = stringToShaderStageFlag(stage)
        });
        _lexer.setWhiteSpace(white_space);
    }

    std::vector<FieldDecl> Parser::parse_members()
    {
        CONSUME(TokenType::LBRACE);
        std::vector<FieldDecl> fields;
        while (_current_token.type != TokenType::RBRACE && _current_token.type != TokenType::END_OF_FILE)
        {
            FieldDecl field;

            field.count = 1;
            field.type = _current_token.lexeme;
            CONSUME(TokenType::IDENTIFIER);
            field.name = _current_token.lexeme;
            CONSUME(TokenType::IDENTIFIER);
            if (_current_token.type == TokenType::LBRACKET)
            {
                advance();
                if ( _current_token.type == TokenType::NUMBER )
                {
                    field.count = std::stoi(_current_token.lexeme);
                    CONSUME(TokenType::NUMBER);
                }
                CONSUME(TokenType::RBRACKET);
            }
            expect(TokenType::SEMICOLON, "Expected ';' after uniform parameter", __FUNCTION__);
            fields.push_back( field );
        }
        expect(TokenType::RBRACE, "Expected '}' after uniform buffer", __FUNCTION__);
        return fields;
    }

    Properties Parser::parseEnclosedBracesParameters()
    {
        CONSUME(TokenType::LBRACE);

        Properties params;
        while (_current_token.type != TokenType::RBRACE && _current_token.type != TokenType::END_OF_FILE)
        {
            std::vector<std::string>& s = params[ parseAny() ];
            CONSUME(TokenType::COLON);

            if (_current_token.type == TokenType::LBRACKET)
            {
                s = parseArray();
            }
            else
            {
                s.push_back(parseAny());
            }
        }
        EXPECTED(TokenType::RBRACE, "Expected '}' after vertex_attributes");
        return params;
    }

    std::vector<std::string> Parser::parseArray()
    {
        std::vector<std::string> arr;
        CONSUME(TokenType::LBRACKET);
        arr.push_back(_current_token.lexeme);
        advance();
        while(_current_token.type == TokenType::COMMA)
        {
            CONSUME(TokenType::COMMA);
            arr.push_back(_current_token.lexeme);
            advance();
        }
        CONSUME(TokenType::RBRACKET);
        return arr;
    }

    Properties Parser::parse_properties()
    {
        Properties props;
        CONSUME(TokenType::LPAREN);
        while (_current_token.type != TokenType::RPAREN && _current_token.type != TokenType::END_OF_FILE)
        {
            std::vector<std::string>& s = props[ _current_token.lexeme ];
            advance();
            CONSUME(TokenType::COLON);

            if (_current_token.type == TokenType::LBRACKET)
            {
                s = parseArray();
            }
            else
            {
                s.push_back(_current_token.lexeme);
                advance();
            }
            if (_current_token.type == TokenType::COMMA) {
                advance();
            }
        }
        EXPECTED(TokenType::RPAREN, "Expected ')' after uniform parameter");
        return props;
    }

    int Parser::parse_quantity()
    {
        int count = 0;
        if (_current_token.type == TokenType::LBRACKET)
        {
            advance();
            if ( _current_token.type == TokenType::NUMBER )
            {
                count = std::stoi(_current_token.lexeme);
                CONSUME(TokenType::NUMBER);
            }
            CONSUME(TokenType::RBRACKET);
        }
        return count;
    }
    
    void Parser::parse_block(std::map<std::string, std::string>& content)
    {
        CONSUME(TokenType::LBRACE);
        while (_current_token.type != TokenType::RBRACE && _current_token.type != TokenType::END_OF_FILE)
        {
            std::string& value = content[ _current_token.lexeme ];
            CONSUME(TokenType::IDENTIFIER);
            CONSUME(TokenType::EQUALS);
            value = _current_token.lexeme;
            advance();
            expect(TokenType::SEMICOLON, "Expected ';' after uniform parameter", __FUNCTION__);
        }
        expect(TokenType::RBRACE, "Expected '}' after uniform parameter", __FUNCTION__);
    }

    kege::ShaderStageFlag Parser::toShaderStages(const std::vector<std::string>& s)
    {
        kege::ShaderStageFlag stages = kege::ShaderStageFlag::Invalid;
        for (auto& stage: s)
        {
            stages |= stringToShaderStageFlag(stage);
        }
        return stages;
    }

    // Helper methods
    void Parser::consume(TokenType expected, const char* caller)
    {
        if (_current_token.type == expected)
        {
            advance();
        }
        else
        {
            std::stringstream ss;
            ss <<"kege::meta::shader::Parser::"<< caller <<"(): " << "Unexpected token: " << _current_token.lexeme
               << ", expected: " << static_cast<int>(expected);
            throw std::runtime_error(ss.str());
        }
    }

    Token Parser::expect(TokenType expected, const std::string& error_msg, const char* caller)
    {
        if (_current_token.type == expected)
        {
            Token token = _current_token;
            advance();
            return token;
        }
        throw std::runtime_error(caller +std::string(": ")+ error_msg);
    }

    bool Parser::match(TokenType type)
    {
        if (check(type))
        {
            advance();
            return true;
        }
        return false;
    }

    bool Parser::check(TokenType type) const
    {
        return _current_token.type == type;
    }

    Token Parser::advance()
    {
        Token prev = _current_token;
        _current_token = _lexer.nextToken();
        return prev;
    }

}

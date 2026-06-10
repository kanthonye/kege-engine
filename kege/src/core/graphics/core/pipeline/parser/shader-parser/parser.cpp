//
//  parser.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/21/25.
//

#include "parser.hpp"

namespace kege::glsl{

//    const ShaderReflect* Parser::getReflection()const
//    {
//        return &reflection;
//    }

    void Parser::parse()
    {
        while ( peek().type != TokenType::EndOfFile )
        {
            switch ( peek().type )
            {
                case TokenType::VertexLayout:
                {
                    parseVertexLayout();
                    break;
                }

                case TokenType::IndexedSetLayouts:
                {
                    parseSetLayout();
                    break;
                }

                case TokenType::PushBlockDescs:
                {
                    parsePushConstantBlock();
                    break;
                }

                case TokenType::OutLayout:
                {
                    parseOutLayout();
                    break;
                }

                case TokenType::InLayout:
                {
                    parseInLayout();
                    break;
                }

                default:
                {
                    emitToken( advance() );
                    break;
                }
            }
        }
    }

    void Parser::emitToken(const Token& tok)
    {
        TokenType nex = peek().type;

        // remove tab
        if( tok.type == TokenType::RBrace ) // }
        {
            if( !tab_spacing.empty() )
            {
                tab_spacing.erase( tab_spacing.length() - 1 );
            }
            should_tab = true;
            tab_level_count -= 1;
        }

        // append token
        if (0 < tab_level_count && should_tab)
        {
            generated_glsl += tab_spacing + tok.lexeme;
            should_tab = false;
        }
        else
        {
            generated_glsl += tok.lexeme;
        }

        // inserting tabs
        if( tok.type == TokenType::LBrace ) // {
        {
            tab_spacing += '\t';
            should_tab = true;
            tab_level_count += 1;
        }

        // add space between quantifiers or type. eg. uniform, const, float, vec3 etc
        if ( shouldSpaceAfterToken( tok.type ) )
        {
            //if ( nex != TokenType::LParen && nex != TokenType::Operator && nex != TokenType::Equal && nex != TokenType::Semicolon )
            {
                generated_glsl += " ";
            }
        }

        // add space between idenfitiers and tokens that are not delimiter,
        else if ( tok.type == TokenType::Identifier && !delimiter( nex ) )
        {
            generated_glsl += " ";
        }

        // insert space after equal sign
        else if ( tok.type == TokenType::Equal )
        {
            generated_glsl += " ";
        }

        // insert space after equal sign
        else if ( tok.type == TokenType::Number )
        {
            if
            (
             nex != TokenType::Semicolon &&
             nex != TokenType::Dot &&
             nex != TokenType::RParen &&
             nex != TokenType::RBracket &&
             nex != TokenType::Comma
            )
            {
                generated_glsl += " ";
            }
        }

        // add newline after left/right brace
        else if( tok.type == TokenType::LBrace || tok.type == TokenType::RBrace )
        {
            if( nex != TokenType::Semicolon )
            {
                generated_glsl += "\n";
                should_tab = true;
            }
        }
        else if( nex == TokenType::LBrace || nex == TokenType::RBrace )
        {
            generated_glsl += "\n";
            should_tab = true;
        }
        else if( tok.type == TokenType::Semicolon )
        {
            generated_glsl += "\n";
            should_tab = true;
        }
    }

    void Parser::parseVertexLayout()
    {
//        expect( TokenType::VertexLayout, "vertex_layout" );
//        expect( TokenType::LParen, "(" );
//        VertexLayout attr;
//        while ( !match( TokenType::RParen ) )
//        {
//            switch ( peek().type )
//            {
//                case TokenType::Binding:
//                {
//                    attr.binding = std::stoi( consumeAssignNumber( "binding" ) );
//                    break;
//                }
//                case TokenType::Location:
//                {
//                    attr.location = std::stoi( consumeAssignNumber( "location" ) );
//                    break;
//                }
//                case TokenType::InputRate:
//                {
//                    attr.input_rate = consumeVertexInputRate( "input_rate" );
//                    break;
//                }
//                case TokenType::Comma:
//                {
//                    advance();
//                    break;
//                }
//                default: break;
//            }
//        }
//
//        attr.type = stringToShaderVarType( consumeType("data type") );
//        attr.name = consumeIdentifier("attribute name");
//        expect( TokenType::Semicolon, ";" );
//
//        _attributes.push_back( attr );
//
//        // Emit GLSL
//        generated_glsl += "layout(location = \"" +std::to_string( attr.location )+ + "\") in " + toString( attr.type ) + " " + attr.name + ";\n";
    }

    void Parser::parseSetLayout()
    {
        expect(TokenType::IndexedSetLayouts, "set_layout");
        expect(TokenType::LParen, "(");

        ShaderSetBindingDesc binding;
        int set_index = 0;
        while ( !match( TokenType::RParen ) )
        {
            switch ( tokens[ pos ].type )
            {
                case TokenType::Binding:
                {
                    binding.binding_index = std::stoi( consumeAssignNumber( "binding" ) );
                    break;
                }
                case TokenType::Set:
                {
                    set_index = std::stoi( consumeAssignNumber( "location" ) );
                    break;
                }
                case TokenType::Comma:
                {
                    advance();
                    break;
                }
                default: break;
            }
        }

        std::string descriptor_type;
        switch ( tokens[ pos ].type )
        {
            case TokenType::Sampler:
            {
                binding.usage = BindingUsage::Sampler;
                binding.type = BindType::Image;
                descriptor_type = "sampler";
                break;
            }
            case TokenType::SampledImage:
            {
                binding.usage = BindingUsage::SampledImage;
                binding.type = BindType::Image;
                descriptor_type = "texture2D";
                break;
            }
            case TokenType::StorageImage:
            {
                binding.usage = BindingUsage::StorageImage;
                binding.type = BindType::Image;
                descriptor_type = "uniform image2D";
                break;
            }
            case TokenType::CombinedImageSampler:
            {
                binding.usage = BindingUsage::CombinedImageSampler;
                binding.type = BindType::Image;
                descriptor_type = "uniform sampler2D";
                break;
            }
            case glsl::TokenType::InputAttachment:
            {
                binding.usage = BindingUsage::InputAttachment;
                binding.type = BindType::Image;
                descriptor_type = "subpassInput";
                break;
            }
            case TokenType::UniformBuffer:
            {
                binding.usage = BindingUsage::UniformBuffer;
                binding.type = BindType::Buffer;
                descriptor_type = "uniform";
                break;
            }
            case TokenType::UniformTexelBuffer:
            {
                binding.usage = BindingUsage::UniformTexelBuffer;
                binding.type = BindType::Buffer;
                descriptor_type = "uniform samplerBuffer";
                break;
            }
            case TokenType::UniformBufferDynamic:
            {
                binding.usage = BindingUsage::UniformBufferDynamic;
                binding.type = BindType::Buffer;
                descriptor_type = "uniform";
                break;
            }
            case TokenType::StorageBuffer:
            {
                binding.usage = BindingUsage::StorageBuffer;
                binding.type = BindType::Buffer;
                descriptor_type = "buffer";
                break;
            }
            case TokenType::StorageTexelBuffer:
            {
                binding.usage = BindingUsage::StorageTexelBuffer;
                binding.type = BindType::Buffer;
                descriptor_type = "uniform imageBuffer";
                break;
            }
            case TokenType::StorageBufferDynamic:
            {
                binding.usage = BindingUsage::StorageBufferDynamic;
                binding.type = BindType::Buffer;
                descriptor_type = "buffer";
                break;
            }
            default: break;
        }

        advance();
        binding.name = consumeIdentifier("set layout name");

        if ( binding.type != BindType::Image )
        {
            std::vector< StructBlockMemberDesc > fields = parseShaderBlock();
            std::string instance_name;
            if ( check( TokenType::Identifier ) )
            {
                instance_name = consumeIdentifier( "instance name" );
                binding.binding_count = consumeIfArraySize();
            }
            expect(TokenType::Semicolon, ";");
            binding.struct_desc = new StructBlockDesc;
            binding.struct_desc->name = instance_name;
            binding.struct_desc->members = fields;
        }
        else
        {
            expect(TokenType::Semicolon, ";");
        }

        _set_layouts[ set_index ].push_back( binding );

        // Emit GLSL
        generated_glsl += "layout(set = " + std::to_string( set_index ) +
                         ", binding = " + std::to_string( binding.binding_index ) +
                         ") " + descriptor_type + " " + binding.name;
        if ( binding.struct_desc )
        {
            emitGLSLBlock( binding.struct_desc );
        }
        else
        {
            generated_glsl += ";\n";
        }
    }

    void Parser::parsePushConstantBlock()
    {
        expect(TokenType::PushBlockDescs, "push_blocks");

        std::string instance_name;

        PushBlockDesc block;
        block.name = consumeIdentifier( "identifier" );
        std::vector< StructBlockMemberDesc > fields = parseShaderBlock();
        if ( check( TokenType::Identifier ) )
        {
            instance_name = consumeIdentifier( "member name" );
            block.count = consumeIfArraySize();
        }
        expect(TokenType::Semicolon, ";");

        block.struct_desc = new StructBlockDesc;
        block.struct_desc->name = instance_name;
        block.struct_desc->members = fields;
        _push_blocks.push_back( block );

        // Emit GLSL
        generated_glsl += "layout( push_constant ) uniform " + block.name;
        emitGLSLBlock( block.struct_desc );

        if ( !block.name.empty() )
        {
            if ( block.count > 1 )
            {
                generated_glsl += block.name + "[" + std::to_string( block.count ) +"];\n";
            }
            else
            {
                generated_glsl += block.name + ";\n";
            }
        }
        else
        {
            generated_glsl += ";\n";
        }
    }

    void Parser::emitGLSLBlock( ref::StructBlockDesc& block )
    {
        // Emit GLSL
        generated_glsl += "{\n";
        for ( auto& m : block->members )
        {
            generated_glsl += "    " + m.type + " " + m.name + ";\n";
        }
        generated_glsl += "}\n";
    }

    void Parser::parseOutLayout()
    {
        expect(TokenType::OutLayout, "out_layout");
        expect(TokenType::LParen, "(");

        int location = std::stoi( consumeAssignNumber( "location" ) );
        expect(TokenType::RParen, ")");

        ShaderVar type = stringToShaderVarType( consumeType("data type") );
        std::string name = consumeIdentifier("attribute name");
        expect( TokenType::Semicolon, ";" );

        generated_glsl += "layout(location = \"" +std::to_string( location )+ + "\") out " + shaderVarToString( type ) + " " + name + ";\n";
    }

    void Parser::parseInLayout()
    {
        expect(TokenType::OutLayout, "in_layout");
        expect(TokenType::LParen, "(");

        int location = std::stoi( consumeAssignNumber( "location" ) );
        expect(TokenType::RParen, ")");

        ShaderVar type = stringToShaderVarType( consumeType("data type") );
        std::string name = consumeIdentifier("attribute name");
        expect( TokenType::Semicolon, ";" );

        generated_glsl += "layout(location = \"" +std::to_string( location )+ + "\") in " + shaderVarToString( type ) + " " + name + ";\n";
    }

    std::vector< StructBlockMemberDesc > Parser::parseShaderBlock()
    {
        expect(TokenType::LBrace, "{");

        int offset = 0;
        std::vector< StructBlockMemberDesc > fields;
        while ( !check(TokenType::RBrace) && !isAtEnd() )
        {
            StructBlockMemberDesc field = {};
            field.offset = offset;
            field.type   = consumeType("member type");
            field.name   = consumeIdentifier( "member name" );
            field.count  = consumeIfArraySize();
            field.size   = sizeOf( stringToShaderVarType( field.type ) );
            expect( TokenType::Semicolon, ";" );
            fields.push_back(field);

            offset += field.size;
        }

        expect(TokenType::RBrace, "}");
        return fields;
    }

    // Utility functions
    bool Parser::isAtEnd() const
    {
        return pos >= tokens.size() || tokens[pos].type == TokenType::EndOfFile;
    }

    const Token& Parser::prev() const
    {
        size_t p = pos - 1;
        return ( p < 0 ) ? tokens[ pos ] : tokens[ p ];
    }

    const Token& Parser::next() const
    {
        size_t n = pos + 1;
        return ( n >= tokens.size() ) ? tokens[ pos ] : tokens[ n ];
    }

    const Token& Parser::peek() const
    {
        return tokens[pos];
    }

    const Token& Parser::advance()
    {
        if ( !isAtEnd() )
        {
            return tokens[ pos++ ];
        }
        return peek();
    }

    bool Parser::check(TokenType t) const
    {
        return !isAtEnd() && peek().type == t;
    }

    bool Parser::match(TokenType t)
    {
        if ( check(t) )
        {
            advance();
            return true;
        }
        return false;
    }

    void Parser::expect(TokenType t, const std::string& msg)
    {
        if ( !match(t) )
            throw std::runtime_error("Parser::Parser error: expected " + msg + " near token '" + peek().lexeme + "'");
    }

    int Parser::consumeIfArraySize()
    {
        int size = 0;
        if ( check( TokenType::LBrace ) )
        {
            advance();
            size = std::stoi( tokens[ pos ].lexeme );
            advance();
        }
        return size;
    }

    std::string Parser::consumeAssignNumber( const char* what )
    {
        advance();
        expect( TokenType::Equal, "=" );
        if ( !check( TokenType::Number ) )
            throw std::runtime_error("Expected " + std::string(what) + " at line " + std::to_string(peek().line));
        return advance().lexeme;
    }

    VertexInputRate Parser::consumeVertexInputRate( const char* what )
    {
        expect( TokenType::InputRate, "input_rate" );
        expect( TokenType::Equal, "=" );
        VertexInputRate input = VertexInputRate::Vertex;
        if ( advance().lexeme == "instance" )
        {
            return VertexInputRate::Instance;
        }
        return input;
    }

    std::string Parser::consumeIdentifier(const std::string& what)
    {
        if ( !check(TokenType::Identifier) )
            throw std::runtime_error("Expected " + what + " at line " + std::to_string(peek().line));
        return advance().lexeme;
    }

    std::string Parser::consumeType(const std::string& what)
    {
        if ( !check( TokenType::Type ) )
            throw std::runtime_error("Expected " + what + " at line " + std::to_string(peek().line));

//        // Keyword mapping
//        static const std::unordered_map< std::string, kege::ShaderVar > shader_types =
//        {
//            { "float",  kege::ShaderVar::Float  },
//            { "int",    kege::ShaderVar::Sint   },
//            { "uint",   kege::ShaderVar::Uint   },
//            { "vec2",   kege::ShaderVar::Vec2   },
//            { "vec3",   kege::ShaderVar::Vec3   },
//            { "vec4",   kege::ShaderVar::Vec4   },
//            { "mat2",   kege::ShaderVar::Mat2   },
//            { "mat3",   kege::ShaderVar::Mat3   },
//            { "mat4",   kege::ShaderVar::Mat4   },
//            { "bool",   kege::ShaderVar::Bool   },
//            { "double", kege::ShaderVar::Double },
//            { "dvec2",  kege::ShaderVar::Vec2D  },
//            { "dvec3",  kege::ShaderVar::Vec3D  },
//            { "dvec4",  kege::ShaderVar::Vec4D  },
//        };
//
//        auto itr = shader_types.find( advance().lexeme );
//        if ( itr != shader_types.end() )
//        {
//            return itr->second;
//        }

        return advance().lexeme;
    }

    const std::string& Parser::getGLSLOutput()
    {
        return generated_glsl;
    }

    bool Parser::delimiter(const TokenType& tok)
    {
        return
        (tok == TokenType::LBrace || tok == TokenType::LParen || tok == TokenType::LBracket ||
         tok == TokenType::RBrace || tok == TokenType::RParen || tok == TokenType::RBracket ||
         tok == TokenType::Comma || tok == TokenType::Dot || tok == TokenType::Semicolon);
    }

    bool Parser::shouldSpaceAfterToken(const TokenType& tok)
    {
        return
        (tok == TokenType::Qualifier || tok == TokenType::Type || tok == TokenType::Operator ||
         tok == TokenType::Set || tok == TokenType::Binding || tok == TokenType::Location );
    }

    Parser::Parser(const std::vector<Token>& tks)
    :   tokens(tks)
    ,   pos(0)
    {}

}

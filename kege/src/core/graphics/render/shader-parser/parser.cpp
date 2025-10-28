//
//  parser.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/21/25.
//

#include "parser.hpp"

namespace kege::glsl{

    const ShaderReflect* Parser::getReflection()const
    {
        return &reflection;
    }

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

                case TokenType::SetLayout:
                {
                    parseSetLayout();
                    break;
                }

                case TokenType::PushBlockLayout:
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
            if ( nex != TokenType::Semicolon && nex != TokenType::Dot && nex != TokenType::RParen && nex != TokenType::RBracket && nex != TokenType::Comma )
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
        expect( TokenType::VertexLayout, "vertex_layout" );
        expect( TokenType::LParen, "(" );
        VertexAttributeDesc attr;
        while ( !match( TokenType::RParen ) )
        {
            switch ( peek().type )
            {
                case TokenType::Binding:
                {
                    attr.binding = std::stoi( consumeAssignNumber( "binding" ) );
                    break;
                }
                case TokenType::Location:
                {
                    attr.location = std::stoi( consumeAssignNumber( "location" ) );
                    break;
                }
                case TokenType::InputRate:
                {
                    attr.input_rate = consumeVertexInputRate( "input_rate" );
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

        attr.type = consumeType("data type");
        attr.name = consumeIdentifier("attribute name");
        expect( TokenType::Semicolon, ";" );

        reflection.attributes.push_back( attr );

        // Emit GLSL
        generated_glsl += "layout(location = \"" +std::to_string( attr.location )+ + "\") in " + toString( attr.type ) + " " + attr.name + ";\n";
    }

    void Parser::parseSetLayout()
    {
        expect(TokenType::SetLayout, "set_layout");
        expect(TokenType::LParen, "(");

        Ref< ShaderSetBindingPoint > set = new ShaderSetBindingPoint;

        while ( !match( TokenType::RParen ) )
        {
            switch ( tokens[ pos ].type )
            {
                case TokenType::Binding:
                {
                    set->binding = std::stoi( consumeAssignNumber( "binding" ) );
                    break;
                }
                case TokenType::Set:
                {
                    set->set = std::stoi( consumeAssignNumber( "location" ) );
                    reflection.shader_set_binding_points[ set->set ].push_back( set );
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
                set->descriptor = DescriptorType::Sampler;
                set->binding_type = BindingType::TEXTURE;
                descriptor_type = "sampler";
                break;
            }
            case TokenType::SampledImage:
            {
                set->descriptor = DescriptorType::SampledImage;
                set->binding_type = BindingType::TEXTURE;
                descriptor_type = "texture2D";
                break;
            }
            case TokenType::StorageImage:
            {
                set->descriptor = DescriptorType::StorageImage;
                set->binding_type = BindingType::TEXTURE;
                descriptor_type = "uniform image2D";
                break;
            }
            case TokenType::CombinedImageSampler:
            {
                set->descriptor = DescriptorType::CombinedImageSampler;
                set->binding_type = BindingType::TEXTURE;
                descriptor_type = "uniform sampler2D";
                break;
            }
            case TokenType::UniformBuffer:
            {
                set->descriptor = DescriptorType::UniformBuffer;
                set->binding_type = BindingType::BUFFER;
                descriptor_type = "uniform";
                break;
            }
            case TokenType::UniformTexelBuffer:
            {
                set->descriptor = DescriptorType::UniformTexelBuffer;
                set->binding_type = BindingType::BUFFER;
                descriptor_type = "uniform samplerBuffer";
                break;
            }
            case TokenType::UniformBufferDynamic:
            {
                set->descriptor = DescriptorType::UniformBufferDynamic;
                set->binding_type = BindingType::BUFFER;
                descriptor_type = "uniform";
                break;
            }
            case TokenType::StorageBuffer:
            {
                set->descriptor = DescriptorType::StorageBuffer;
                set->binding_type = BindingType::BUFFER;
                descriptor_type = "buffer";
                break;
            }
            case TokenType::StorageTexelBuffer:
            {
                set->descriptor = DescriptorType::StorageTexelBuffer;
                set->binding_type = BindingType::BUFFER;
                descriptor_type = "uniform imageBuffer";
                break;
            }
            case TokenType::StorageBufferDynamic:
            {
                set->descriptor = DescriptorType::StorageBufferDynamic;
                set->binding_type = BindingType::BUFFER;
                descriptor_type = "buffer";
                break;
            }
            case glsl::TokenType::InputAttachment:
            {
                set->descriptor = DescriptorType::InputAttachment;
                set->binding_type = BindingType::TEXTURE;
                descriptor_type = "subpassInput";
                break;
            }
            default: break;
        }

        advance();
        set->name = consumeIdentifier("set layout name");

        if ( set->binding_type != BindingType::TEXTURE )
        {
            set->struct_block = parseShaderBlock();
        }
        else
        {
            expect(TokenType::Semicolon, ";");
        }

        // Emit GLSL
        generated_glsl += "layout(set = " + std::to_string(set->set) +
                         ", binding = " + std::to_string(set->binding) +
                         ") " + descriptor_type + " " + set->name;
        if ( set->struct_block )
        {
            emitGLSLBlock( set->struct_block );
        }
        else
        {
            generated_glsl += ";\n";
        }
    }

    void Parser::parsePushConstantBlock()
    {
        expect(TokenType::PushBlockLayout, "push_block_layout");

        Ref< ShaderConstantBindingPoint > constant = new ShaderConstantBindingPoint;
        constant->name = consumeIdentifier( "identifier" );
        constant->struct_block = parseShaderBlock();
        reflection.push_constant_blocks.push_back( constant );

        // Emit GLSL
        generated_glsl += "layout( push_constant ) uniform " + constant->name;
        emitGLSLBlock( constant->struct_block );
    }

    void Parser::emitGLSLBlock( Ref< ShaderStructBlock >& struct_block )
    {
        // Emit GLSL
        generated_glsl += "{\n";
        for ( auto& m : struct_block->fields )
        {
            generated_glsl += "    " + toString( m.type ) + " " + m.name + ";\n";
        }
        generated_glsl += "}\n";
        if ( !struct_block->instance_name.empty() )
        {
            if ( struct_block->count > 1 )
            {
                generated_glsl += struct_block->instance_name + "[" + std::to_string( struct_block->count ) +"];\n";
            }
            else
            {
                generated_glsl += struct_block->instance_name + ";\n";
            }
        }
        else
        {
            generated_glsl += ";\n";
        }
    }

    void Parser::parseOutLayout()
    {
        expect(TokenType::OutLayout, "out_layout");
        expect(TokenType::LParen, "(");

        int location = std::stoi( consumeAssignNumber( "location" ) );
        expect(TokenType::RParen, ")");

        ShaderDataType type = consumeType("data type");
        std::string name = consumeIdentifier("attribute name");
        expect( TokenType::Semicolon, ";" );

        generated_glsl += "layout(location = \"" +std::to_string( location )+ + "\") out " + toString( type ) + " " + name + ";\n";
    }

    void Parser::parseInLayout()
    {
        expect(TokenType::OutLayout, "in_layout");
        expect(TokenType::LParen, "(");

        int location = std::stoi( consumeAssignNumber( "location" ) );
        expect(TokenType::RParen, ")");

        ShaderDataType type = consumeType("data type");
        std::string name = consumeIdentifier("attribute name");
        expect( TokenType::Semicolon, ";" );

        generated_glsl += "layout(location = \"" +std::to_string( location )+ + "\") in " + toString( type ) + " " + name + ";\n";
    }

    Ref< ShaderStructBlock > Parser::parseShaderBlock()
    {
        expect(TokenType::LBrace, "{");

        int offset = 0;
        std::vector< ShaderStructField > fields;
        while ( !check(TokenType::RBrace) && !isAtEnd() )
        {
            ShaderStructField field = {};
            field.offset = offset;
            field.type   = consumeType("member type");
            field.name   = consumeIdentifier( "member name" );
            field.count  = consumeIfArraySize();
            field.size   = sizeOf( field.type );
            expect( TokenType::Semicolon, ";" );
            fields.push_back(field);

            offset += field.size;
        }

        expect(TokenType::RBrace, "}");
        Ref< ShaderStructBlock > block = new ShaderStructBlock( fields );
        if ( check( TokenType::Identifier ) )
        {
            block->instance_name = consumeIdentifier( "member name" );
            block->count = consumeIfArraySize();
        }
        expect(TokenType::Semicolon, ";");
        return block;
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

    kege::ShaderDataType Parser::consumeType(const std::string& what)
    {
        if ( !check( TokenType::Type ) )
            throw std::runtime_error("Expected " + what + " at line " + std::to_string(peek().line));

        // Keyword mapping
        static const std::unordered_map< std::string, kege::ShaderDataType > shader_types =
        {
            { "float",  kege::ShaderDataType::Float  },
            { "int",    kege::ShaderDataType::Sint   },
            { "uint",   kege::ShaderDataType::Uint   },
            { "vec2",   kege::ShaderDataType::Vec2   },
            { "vec3",   kege::ShaderDataType::Vec3   },
            { "vec4",   kege::ShaderDataType::Vec4   },
            { "mat2",   kege::ShaderDataType::Mat2   },
            { "mat3",   kege::ShaderDataType::Mat3   },
            { "mat4",   kege::ShaderDataType::Mat4   },
            { "bool",   kege::ShaderDataType::Bool   },
            { "double", kege::ShaderDataType::Double },
            { "dvec2",  kege::ShaderDataType::DVec2  },
            { "dvec3",  kege::ShaderDataType::DVec3  },
            { "dvec4",  kege::ShaderDataType::DVec4  },
        };

        auto itr = shader_types.find( advance().lexeme );
        if ( itr != shader_types.end() )
        {
            return itr->second;
        }

        return kege::ShaderDataType::Undefined;
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

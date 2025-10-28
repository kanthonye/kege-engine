//
//  glsl-parser.cpp
//  glsl-preprocessor
//
//  Created by Kenneth Esdaile on 1/31/25.
//

#include "glsl-parser.hpp"
namespace kege{namespace glsl{

    void checkThenGetToken( Token& token, Lexer* lexer, Token expected_token )
    {
        if( token == expected_token )
        {
            token = lexer->getNextToken();
            return;// expecte token open-parenthesis;
        }
        throw std::string( "token mismatch " ) + lexer->getLexeme();
    }

    void getToken( Token& token, Lexer* lexer, Token expected_token )
    {
        if(( token = lexer->getNextToken() ) != expected_token )
        {
            throw std::string("expected token -> ") + lexer->getLexeme();
        }
    }

    void parserQualifier( Token& token, Lexer* lexer, Qualifier* qualifier )
    {
        qualifier->value = -1;
        qualifier->name = lexer->getLexeme();
        if(( token = lexer->getNextToken() ) == Token::EQUAL )
        {
            getToken( token, lexer, Token::INT );
            qualifier->value = atoi( lexer->getLexeme() );
            token = lexer->getNextToken();
        }
    }

    void parserQualifier( Token& token, Lexer* lexer, Layout* layout )
    {
        getToken( token, lexer, Token::OPEN_PARENTHESIS );
        token = lexer->getNextToken();

        Qualifier qualifier;
        parserQualifier( token, lexer, &qualifier );
        layout->qualifiers.push_back( qualifier );

        while ( token == Token::COMMA )
        {
            token = lexer->getNextToken();
            parserQualifier( token, lexer, &qualifier );
            layout->qualifiers.push_back( qualifier );
        }

        checkThenGetToken( token, lexer, Token::CLOSE_PARENTHESIS );
    }

    void parserStorageQualifier( Token& token, Lexer* lexer, Layout* layout )
    {
        switch( token )
        {
            // core glsl storage quantifier
            case Token::IN:
            case Token::OUT:
            case Token::UNIFORM:
            case Token::BUFFER:
            // custom storage quantifier to explicitly specify the descriptor type
            case Token::STORAGE_IMAGE:
            case Token::SAMPLED_IMAGE:
            case Token::UNIFORM_BUFFER:
            case Token::STORAGE_BUFFER:
            case Token::COMBINED_IMAGE_SAMPLER:
            case Token::UNIFORM_TEXEL_BUFFER:
            case Token::STORAGE_TEXEL_BUFFER:
            case Token::UNIFORM_BUFFER_DYNAMIC:
            case Token::STORAGE_BUFFER_DYNAMIC:
            case Token::INPUT_ATTACHMENT:
            {
                layout->storage_qualifier = lexer->getLexeme();
                break;
            }

            default:
            {
                throw std::string("expected layout storage quantifier -> ") + lexer->getLexeme();
                break;
            }
        }
        token = lexer->getNextToken();
    }

    void parserVariable( Token& token, Lexer* lexer, Variable* variable )
    {
        variable->type = lexer->getLexeme();
        getToken( token, lexer, Token::IDENTIFIER );
        variable->name = lexer->getLexeme();
        variable->count = 1;

        switch (( token = lexer->getNextToken() ))
        {
            case Token::OPEN_SQUARE_BRACKET:
            {
                token = lexer->getNextToken();
                if( token != Token::CLOSE_SQUARE_BRACKET )
                {
                    variable->count = atoi( lexer->getLexeme() );
                    getToken( token, lexer, Token::CLOSE_SQUARE_BRACKET );
                }
                getToken( token, lexer, Token::SEMICOLON );
                token = lexer->getNextToken();
                break;
            }

            case Token::SEMICOLON:
            {
                token = lexer->getNextToken();
                break;
            }

            default:
            {
                throw std::string("unexpected token  -> ") + toString( token );
                break;
            };
        }
    }

    void parserBlock( Token& token, Lexer* lexer, std::vector< Variable >& members )
    {
        token = lexer->getNextToken();
        while ( token != Token::CLOSE_CURLY_BRACKET )
        {
            Variable variable;
            parserVariable( token, lexer, &variable );
            members.push_back( variable );
        }
    }

    void parserBlockPostInstance( Token& token, Lexer* lexer, Instanced& instanced )
    {
        instanced.count = 0;
        instanced.name = lexer->getLexeme();
        token = lexer->getNextToken();
        if ( token == Token::OPEN_SQUARE_BRACKET )
        {
            token = lexer->getNextToken();
            instanced.count = atoi( lexer->getLexeme() );
            getToken( token, lexer, Token::CLOSE_SQUARE_BRACKET );
        }
        checkThenGetToken( token, lexer, Token::SEMICOLON );
    }

    void parserTokenLayout( Token& token, Lexer* lexer, Layout* layout )
    {
        layout->count = 1;
        parserQualifier( token, lexer, layout );
        parserStorageQualifier( token, lexer, layout );

        switch ( token )
        {
            case Token::FLOAT:
            case Token::DOUBLE:
            case Token::INT:
            case Token::UINT:
            case Token::VEC2:
            case Token::VEC3:
            case Token::VEC4:
            case Token::MAT2:
            case Token::MAT3:
            case Token::MAT4:
            case Token::MAT2X2:
            case Token::MAT2X3:
            case Token::MAT2X4:
            case Token::MAT3X2:
            case Token::MAT3X3:
            case Token::MAT3X4:
            case Token::MAT4X2:
            case Token::MAT4X3:
            case Token::MAT4X4:
            case Token::SAMPLER:
            case Token::SAMPLER1D:
            case Token::SAMPLER2D:
            case Token::SAMPLER3D:
            case Token::SAMPLER1D_ARRAY:
            case Token::SAMPLER2D_ARRAY:
            case Token::SAMPLER1D_SHADOW:
            case Token::SAMPLER2D_SHADOW:
            case Token::SAMPLER1D_ARRAY_SHADOW:
            case Token::SAMPLER2D_ARRAY_SHADOW:
            case Token::IMAGE1D:
            case Token::IMAGE2D:
            case Token::IMAGE3D:
            case Token::IMAGE1D_ARRAY:
            case Token::IMAGE2D_ARRAY:
            case Token::IMAGE_CUBE:
            case Token::IMAGE_CUBE_ARRAY:
            {
                layout->type = lexer->getLexeme();
                getToken( token, lexer, Token::IDENTIFIER );
                layout->name = lexer->getLexeme();
                token = lexer->getNextToken();

                if( token == Token::OPEN_SQUARE_BRACKET )
                {
                    getToken( token, lexer, Token::INT );
                    layout->count = atoi( lexer->getLexeme() );
                    getToken( token, lexer, Token::CLOSE_SQUARE_BRACKET );
                    token = lexer->getNextToken();
                }
                break;
            }

            default:
            {
                layout->name = lexer->getLexeme();
                token = lexer->getNextToken();
                break;
            };
        }

        switch ( token )
        {
            case Token::OPEN_CURLY_BRACKET: // parse block member variables
            {
                parserBlock( token, lexer, layout->members );
                switch (( token = lexer->getNextToken() ))
                {
                    case Token::IDENTIFIER: // parse block instance
                    {
                        parserBlockPostInstance( token, lexer, layout->instanced );
                        break;
                    }

                    case Token::SEMICOLON: // no block instance exist
                    {
                        token = lexer->getNextToken();
                        break;
                    }

                    default:
                    {
                        throw std::string("unexpected token  -> ") + toString( token );
                        break;
                    }
                }
                break;
            }

            case Token::SEMICOLON:
            {
                token = lexer->getNextToken();
                break;
            }

            default:
            {
                throw std::string("unexpected token  -> ") + toString( token );
                break;
            }
        }
    }

    bool contained( const std::vector< Qualifier >& qualifiers, const std::string& name )
    {
        for (int i=0; i<qualifiers.size(); ++i)
        {
            if (qualifiers[i].name == name )
            {
                return true;
            }
        }
        return false;
    }

    std::string parser( int stage, const std::vector< char >* buffer, Meta* meta )
    {
        Lexer lexer( buffer );
        
        try
        {
            Token token = lexer.getNextToken();
            while ( token != Token::END_STREAM )
            {
                switch ( token )
                {
                    case Token::LAYOUT:
                    {
                        Layout layout;
                        layout.stages.push_back( stage );

                        parserTokenLayout( token, &lexer, &layout );
                        if ( layout.storage_qualifier == "in" )
                        {
                            int i = static_cast<int>(meta->layouts.size());
                            meta->inputs[stage].push_back( i );
                            meta->layouts.push_back( layout );
                        }
                        else if ( layout.storage_qualifier == "out" )
                        {
                            int i = static_cast<int>(meta->layouts.size());
                            meta->outputs[stage].push_back( i );
                            meta->layouts.push_back( layout );
                        }
                        else if ( contained( layout.qualifiers, "push_constant" ) )
                        {
                            //meta->push_constants.insert( layout.name, layout );
                            int* index = meta->push_constants.find( layout.name );
                            if ( index == nullptr )
                            {
                                int i = static_cast<int>(meta->layouts.size());
                                meta->push_constants.insert( layout.name, i );
                                meta->layouts.push_back( layout );
                            }
                            else
                            {
                                meta->layouts[ *index ].stages.push_back( stage );
                            }
                        }
                        else
                        {
                            int* index = meta->uniforms.find( layout.name );
                            if ( index == nullptr )
                            {
                                int i = static_cast<int>(meta->layouts.size());
                                meta->uniforms.insert( layout.name, i );
                                meta->layouts.push_back( layout );
                            }
                            else
                            {
                                meta->layouts[ *index ].stages.push_back( stage );
                            }
                        }
                        break;
                    }

                    case Token::STRUCT:
                    {
                        getToken( token, &lexer, Token::IDENTIFIER );

                        Struct struct_block;
                        struct_block.stage = stage;
                        struct_block.name = lexer.getLexeme();

                        getToken( token, &lexer, Token::OPEN_CURLY_BRACKET );
                        parserBlock( token, &lexer, struct_block.members );
                        meta->struct_blocks[stage].insert(struct_block.name, struct_block);
                        break;
                    }

                    case Token::VOID:
                    {
                        getToken( token, &lexer, Token::IDENTIFIER );
                        std::string main = lexer.getLexeme();
                        if ( main == "main" )
                        {
                            return "success";
                        }
                        break;
                    }

                    default:
                    {
                        token = lexer.getNextToken();
                        break;
                    }
                }
            }
        }
        catch( const std::string& err )
        {
            return err;
        }
        return "success";
    }

    std::ostream& operator<<(std::ostream& s, const std::vector< kege::glsl::Variable >& members )
    {
        if ( !members.empty() )
        {
            s <<"{\n";
            for (int i=0; i<members.size(); ++i)
            {
                s <<"    " << members[i].type <<" ";
                s << members[i].name;
                if ( members[i].count > 1 )
                {
                    s <<"["<< members[i].count <<"]";
                }
                s <<"\n";
            }
            s <<"}\n";
        }
        return s;
    }

    std::ostream& operator<<(std::ostream& s, const kege::glsl::Layout& layout )
    {
        s <<"[ stages: ";
        for (int i=0; i<layout.stages.size(); ++i)
            s << layout.stages[i] << (( i + 1 < layout.stages.size() ) ?",": "");
        s <<" ] ";

        s <<"( ";
        for (int i=0; i<layout.qualifiers.size(); ++i)
        {
            s << layout.qualifiers[i].name;
            if ( layout.qualifiers[i].value >= 0)
            {
                s <<" = " << layout.qualifiers[i].value;
            }
            if ( i + 1 < layout.qualifiers.size() )
            {
                s <<", ";
            }
        }
        s <<" ) ";
        s << layout.storage_qualifier <<" ";
        s << layout.type <<" ";
        s << layout.name;
        if ( layout.count > 0 )
        {
            s <<"["<< layout.count <<"]\n";
        }
        else
        {
            s <<"\n";
        }
        s << layout.members;
        return s;
    }

    std::ostream& operator<<(std::ostream& s, const kege::glsl::Struct& struct_block )
    {
        s <<"[ stage: " << struct_block.stage <<" ] struct " << struct_block.name <<" ]\n";
        s << struct_block.members;
        return s;
    }

    std::ostream& operator<<(std::ostream& s, const kege::glsl::Meta& meta )
    {
//        std::for_each(meta.inputs.begin(), meta.inputs.end(), [&s](const auto& m) {
//            std::for_each(m.second.begin(), m.second.end(), [&s](const auto& e) {
//                s << e;
//            });
//        });
//        std::for_each(meta.outputs.begin(), meta.outputs.end(), [&s](const auto& m) {
//            std::for_each(m.second.begin(), m.second.end(), [&s](const auto& e) {
//                s << e;
//            });
//        });
        std::for_each(meta.struct_blocks.begin(), meta.struct_blocks.end(), [&s](const auto& m) {
            m.second.foreach([&s](const Struct& sb) {
                s << sb;
            });
        });
        for (int i=0; i<meta.layouts.size(); ++i) s << meta.layouts[i];
        return s;
    }

}}

//
//  json-glsl-lexer.cpp
//  kege
//
//  Created by Kenneth Esdaile on 6/15/24.
//

#include "json-lexer.hpp"

namespace kege{

    const char* JsonLexer::toString( int token )
    {
        switch ( token )
        {
            case ERROR: return "error";
            case END_STREAM: return "end_stream";
            case INTEGER: return "integer";
            case FLOAT: return "float";
            case STRING: return "string";
            case COLON: return "colon";
            case COMMA: return "comma";
            case OPEN_CURLY_BRACKET: return "open_curly_bracket";
            case CLOSE_CURLY_BRACKET: return "close_curly_bracket";
            case OPEN_SQUARE_BRACKET: return "open_square_bracket";
            case CLOSE_SQUARE_BRACKET: return "close_square_bracket";
            default: return "unknown";
        }
    }

    bool charIsNumeric( char c )
    {
        return ( c >= '0' && '9' >= c );
    }

    bool charIsWhiteSpace( char c )
    {
        return c == ' ' || c == '\t';
    }

    bool JsonLexer::load( std::vector< char >& buffer, const char* filename )
    {
        FILE* file = fopen( filename, "rb" );
        if ( !file )
        {
            return false;
        }
        fseek( file, 0, SEEK_END );
        uint64_t size = ftell( file );
        rewind( file );

        buffer.resize( size + 1 );
        fread( buffer.data(), sizeof(char), size, file );
        buffer[ size ] = 0;
        fclose( file );
        return true;
    }

    JsonLexer::Token JsonLexer::getNextToken()
    {
        Token token       = END_STREAM;
        Token state       = START;
        int   index       = 0;
        char  curr_char   = 0;
        char  prev_char   = 0;

        while ( *_curr_char != 0 && state != END && state != ERROR )
        {
            prev_char = curr_char;
            curr_char = *_curr_char;

            switch ( state )
            {
                default:
                    state = ERROR;
                break;

                case START:
                {
                    switch ( curr_char )
                    {
                        default:
                        case '#':
                        case '~':
                        case '|':
                        case '&':
                        case '>':
                        case '<':
                        case '!':
                        case '=':
                        case ';':
                        case '+':
                        case '*':
                        case '/':
                        case ')':
                        case '(':
                            state = ERROR;
                        break;

                        case '-':
                        case '0': case '1': case '2': case '3': case '4':
                        case '5': case '6': case '7': case '8': case '9':
                            state = INTEGER;
                            token = INTEGER;
                        break;

                        case '.':
                            state = FLOAT;
                            token = FLOAT;
                        break;

                        case '"':
                            state = STRING;
                            _curr_char++;
                            continue;
                        break;

                        case ',':
                            state = END;
                            token = COMMA;
                        break;

                        case ':':
                            state = END;
                            token = COLON;
                        break;

                        case '{':
                            state = END;
                            token = OPEN_CURLY_BRACKET;
                        break;

                        case '}':
                            state = END;
                            token = CLOSE_CURLY_BRACKET;
                        break;

                        case '[':
                            state = END;
                            token = OPEN_SQUARE_BRACKET;
                        break;

                        case ']':
                            state = END;
                            token = CLOSE_SQUARE_BRACKET;
                        break;

                        case ' ': case '\t':
                            state = START;
                            _curr_char++;
                            continue;
                        break;

                        case '\n': case '\r':
                            state = START;
                            _curr_char++;
                            _line_count++;
                            continue;
                            break;
                    }
                    break;
                }

                case INTEGER:
                {
                    if ( charIsNumeric( curr_char ) )
                    {
                        state = INTEGER;
                    }
                    else if ( curr_char == '.' )
                    {
                        token = FLOAT;
                        state = FLOAT;
                    }
                    else if ( curr_char == 'e' || curr_char == 'E' )
                    {
                        token = FLOAT;
                        state = FLOAT_E;
                    }
                    else if ( curr_char == 'x' )
                    {
                        if ( index == 1 )
                        {
                            token = HEXADECIMAL;
                            state = HEXADECIMAL;
                        }
                        else
                        {
                            state = ERROR;
                            continue;
                        }
                    }
                    else
                    {
                        state = END;
                        continue;
                    }
                    break;
                }
                case HEXADECIMAL:
                {
                    if
                    (
                        (curr_char >= 'a' && 'f' >= curr_char) ||
                        (curr_char >= 'A' && 'F' >= curr_char) ||
                        (curr_char >= '0' && '9' >= curr_char)
                    )
                    {
                        token = HEXADECIMAL;
                        state = HEXADECIMAL;
                    }
                    else
                    {
                        state = END;
                        continue;
                    }
                    break;
                }

                case FLOAT:
                {
                    if ( charIsNumeric( curr_char ) )
                    {
                        state = FLOAT;
                    }
                    else if ( curr_char == 'e' || curr_char == 'E' )
                    {
                        state = FLOAT_E;
                    }
                    else if ( curr_char == 'f' )
                    {
                        token = FLOAT;
                        state = FLOAT;
                    }
                    else
                    {
                        state = END;
                        continue;
                    }
                    break;
                }

                case FLOAT_E:
                {
                    if ( charIsNumeric( curr_char ) )
                    {
                        state = FLOAT_E;
                    }
                    else if ( curr_char == '+' || curr_char == '-' )
                    {
                        state = FLOAT_EX;
                    }
                    else
                    {
                        state = END;
                        continue;
                    }
                    break;
                }

                case FLOAT_EX:
                {
                    if ( charIsNumeric( curr_char ) )
                    {
                        state = FLOAT_EX;
                    }
                    else if ( curr_char == 'f' )
                    {
                        state = FLOAT;
                    }
                    else
                    {
                        state = END;
                        continue;
                    }
                    break;
                }

                case STRING:
                {
                    if( curr_char == '\"')
                    {
                        state = END;
                        token = STRING;
                        _curr_char++;
                        continue;
                    }
                }
                break;
            }

            lexeme[ index++ ] = curr_char;
            _curr_char++;
        }

        lexeme[ index ] = 0;

        if ( state == ERROR )
        {
            std::string errmsg = std::string( "[ ERROR ]: " ) + " : [ issue ]: @line -> " + std::to_string( _line_count );
            _error = errmsg;
            return ERROR;
        }

        return token;
    }

    const char* JsonLexer::getLexeme()
    {
        return lexeme.data();
    }

    int JsonLexer::getCurrLine()
    {
        return _line_count;
    }

    void JsonLexer::clear()
    {
        lexeme.clear();
    }

    JsonLexer::JsonLexer(const char* buffer)
    :   _curr_char( buffer )
    ,   _line_count( 0 )
    {
        lexeme.resize(128);
    }

    JsonLexer:: ~JsonLexer()
    {
        clear();
    }

    JsonLexer::JsonLexer()
    :   _curr_char( nullptr )
    ,   _line_count( 0 )
    {
        lexeme.resize(128);
    }

}



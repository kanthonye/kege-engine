#include "glsl-lexer.hpp"

namespace kege{namespace glsl{

    bool charIsNumeric( char c )
    { 
        return ( c >= '0' && '9' >= c ); 
    }

    bool charIsWhiteSpace( char c )
    { 
        return c == ' ' || c == '\t'; 
    }

    bool charIsId( char c )
    { 
        return 
            ( c >= 'A' && 'Z' >= c ) || 
            ( c >= 'a' && 'z' >= c ) || 
            ( c >= '0' && '9' >= c ) || 
            c == '_'
        ; 
    }

    bool charIsDelimiter( char c )
    { 
        switch (c)
        {
            case '.':
            case ',':
            case '[':
            case ']':
            case '{':
            case '}':
            case '(':
            case ')':
            case ';':
            case ':':
            case ' ':
            case '\t':
            case '\n':
            case '\r':
            return true;
            break;
        
            default:
            return false;
            break;
        } 
    }

    bool charIsOperator( char c )
    { 
        switch (c)
        {
            case '=':
            case '+':
            case '-':
            case '/':
            case '*':
            case '^':
            case '%':
            case '<':
            case '>':
            case '!':
            case '~':
            case '&':
            case '|':
            return true;
            break;
        
            default:
            return false;
            break;
        } 
    }

    Token getOpFinalState( char c )
    { 
        if ( charIsId( c ) || charIsDelimiter( c ) )
        {
            return Token::END_TOKEN;
        }
        return Token::ERROR;
    }

    void Lexer::print()
    {
        Token tok = START_TOKEN;
        //const char* lex;
        while ( tok != END_STREAM )
        {
            tok = getNextToken();
            printf( "%*s | %s \n", 28, toString( tok ), lexeme );
        }
    }

    Token Lexer::getNextToken()
    {
        Token token       = END_STREAM;
        Token state       = START_TOKEN;
        int   index       = 0;
        char  curr_char   = 0;
        char  prev_char   = 0;

        prev_lexeme = lexeme;
        prev_position = curr_position;

        while ( *curr_position != 0 && state != END_TOKEN && state != ERROR )
        {
            prev_char = curr_char;
            curr_char = *curr_position;

            switch ( state )
            {
                case Token::START_TOKEN:
                {
                    switch ( curr_char )
                    {
                        case '0': case '1': case '2': case '3': case '4':
                        case '5': case '6': case '7': case '8': case '9':
                        {
                            state = Token::INT;
                            token = Token::INT;
                            break;
                        }

                        case '~':
                        {
                            state = Token::INVERT;
                            break;
                        }

                        case '|':
                        {
                            state = Token::BAR;
                            break;
                        }

                        case '&':
                        {
                            state = Token::AMPERSAND;
                            break;
                        }

                        case '>': 
                        {
                            state = Token::GREATER;
                            break;
                        }

                        case '<': 
                        {
                            state = Token::LESS;
                            break;
                        }

                        case '!':
                        {
                            state = Token::NOT;
                            break;
                        }

                        case '=': 
                        {
                            state = Token::EQUAL;
                            break;
                        }

                        case ',': 
                        {
                            state = Token::COMMA;
                            token = Token::COMMA;
                            break;
                        }

                        case '#':
                        {
                            token = Token::HASTAG;
                            state = Token::END_TOKEN;
                            break;
                        }

                        case '.':
                        {
                            token = Token::PERIOD;
                            state = Token::END_TOKEN;
                            break;
                        }

                        case ':': 
                        {
                            token = Token::COLON;
                            state = Token::END_TOKEN;
                            break;
                        }

                        case ';': 
                        {
                            token = Token::SEMICOLON;
                            state = Token::END_TOKEN;
                            break;
                        }

                        case '{':
                        {
                            token = Token::OPEN_CURLY_BRACKET;
                            state = Token::END_TOKEN;
                            break;
                        }

                        case '}': 
                        {
                            token = Token::CLOSE_CURLY_BRACKET;
                            state = Token::END_TOKEN;
                            break;
                        }

                        case '[':
                        {
                            token = Token::OPEN_SQUARE_BRACKET;
                            state = Token::END_TOKEN;
                            break;
                        }

                        case ']':
                        {
                            token = Token::CLOSE_SQUARE_BRACKET;
                            state = Token::END_TOKEN;
                            break;
                        }

                        case '(':
                        {
                            token = Token::OPEN_PARENTHESIS;
                            state = Token::END_TOKEN;
                            break;
                        }

                        case ')': 
                        {
                            token = Token::CLOSE_PARENTHESIS;
                            state = Token::END_TOKEN;
                            break;
                        }

                        case '-':
                        {
                            state = Token::SUB;
                            break;
                        }

                        case '+':
                        {
                            state = Token::ADD;
                            break;
                        }

                        case '*':
                        {
                            state = Token::MUL;
                            break;
                        }

                        case '/':
                        {
                            state = Token::DIV;
                            break;
                        }

                        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': 
                        case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
                        case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
                        case 'v': case 'w': case 'x': case 'y': case 'z':
                        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': 
                        case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
                        case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
                        case 'V': case 'W': case 'X': case 'Y': case 'Z':
                        case '_':
                        {
                            state = Token::IDENTIFIER;
                            token = Token::IDENTIFIER;
                            break;
                        }

                        case ' ': case '\t':
                        {
                            state = Token::START_TOKEN;
                            curr_position++;
                            continue;
                        }

                        case '\n': case '\r':
                        {
                            state = Token::START_TOKEN;
                            curr_position++;
                            line_count++;
                            continue;
                        }

                        default:
                        {
                            state = Token::ERROR;
                            continue;
                        }
                    }
                    break;
                }

                case PERIOD:
                {
                    if ( charIsNumeric( curr_char ) )
                    {
                        state = Token::DOUBLE;
                        token = Token::DOUBLE;
                    }
                    else
                    {
                        state = Token::END_TOKEN;
                        continue;
                    }
                    break;
                }

                case LESS:
                {
                    if ( prev_char == '<' && curr_char == '=' )
                    {
                        state = Token::LESS_EQUAL;
                        token = Token::LESS_EQUAL;
                    }
                    else 
                    {
                        state = Token::END_TOKEN;
                        token = Token::LESS;
                        continue;
                    }
                    break;
                }

                case GREATER:
                {
                    if ( prev_char == '>' && curr_char == '=' )
                    {
                        state = Token::GREATER_EQUAL;
                        token = Token::GREATER_EQUAL;
                    }
                    else
                    {
                        state = Token::END_TOKEN;
                        token = Token::GREATER;
                        continue;
                    }
                    break;
                }

                case EQUAL:
                {
                    if ( prev_char == '=' && curr_char == '=' )
                    {
                        state = Token::EQUAL_EQUAL;
                        token = Token::EQUAL_EQUAL;
                    }
                    else
                    {
                        state = Token::END_TOKEN;
                        token = Token::EQUAL;
                        continue;
                    }
                    break;
                }

                case NOT:
                {
                    if ( prev_char == '!' && curr_char == '=' )
                    {
                        state = Token::NOT_EQUAL;
                        token = Token::NOT_EQUAL;
                    }
                    else
                    {
                        state = Token::END_TOKEN;
                        token = Token::NOT;
                        continue;
                    }
                    break;
                }

                case AMPERSAND:
                {
                    if ( prev_char == '&' && curr_char == '=' )
                    {
                        state = Token::AND_EQUAL;
                        token = Token::AND_EQUAL;
                    }
                    else if ( prev_char == '&' && curr_char == '&' )
                    {
                        state = Token::AND;
                        token = Token::AND;
                    }
                    else
                    {
                        state = Token::END_TOKEN;
                        token = Token::AMPERSAND;
                        continue;
                    }
                    break;
                }

                case BAR:
                {
                    if ( prev_char == '|' && curr_char == '=' )
                    {
                        state = Token::OR_EQUAL;
                        token = Token::OR_EQUAL;
                    }
                    if ( prev_char == '|' && curr_char == '|' )
                    {
                        state = Token::OR;
                        token = Token::OR;
                    }
                    else
                    {
                        state = Token::END_TOKEN;
                        token = Token::BAR;
                        continue;
                    }
                    break;
                }

                case MOD:
                {
                    if ( prev_char == '%' && curr_char == '=' )
                    {
                        state = Token::MOD_EQUAL;
                        token = Token::MOD_EQUAL;
                    }
                    else
                    {
                        state = Token::END_TOKEN;
                        token = Token::MOD;
                        continue;
                    }
                    break;
                }

                case INVERT:
                {
                    if ( prev_char == '~' && curr_char == '=' )
                    {
                        state = Token::INVERT_EQUAL;
                        token = Token::INVERT_EQUAL;
                    }
                    else
                    {
                        state = Token::END_TOKEN;
                        token = Token::INVERT;
                        continue;
                    }
                    break;
                }

                case DIV:
                {
                    if ( prev_char == '/' && curr_char == '=' )
                    {
                        token = Token::DIV_EQUAL;
                        state = Token::DIV_EQUAL;
                    }
                    else if ( curr_char == '/' && prev_char == '/' )
                    {
                        state = Token::INLINE_COMMENT;
                        curr_char = '\"';
                        index = 0;
                        continue;
                    }
                    else if ( prev_char == '/' && curr_char == '*' )
                    {
                        state = Token::BLOCK_COMMENT;
                        curr_char = '\"';
                        index = 0;
                        continue;
                    }
                    else
                    {
                        token = Token::DIV;
                        if (( state = getOpFinalState( curr_char ) ) == END_TOKEN )
                        {
                            continue;
                        }
                    }
                    break;
                }

                case MUL:
                {
                    if ( prev_char == '*' && curr_char == '=' )
                    {
                        token = Token::MUL_EQUAL;
                        state = Token::MUL_EQUAL;
                    }
                    if ( charIsOperator( curr_char ) )
                    {
                        token = Token::MUL_EQUAL;
                        state = Token::ERROR;
                        continue;
                    }
                    else
                    {
                        token = Token::MUL;
                        if (( state = getOpFinalState( curr_char ) ) == END_TOKEN )
                        {
                            continue;
                        }
                    }
                    break;
                }

                case ADD:
                {
                    if ( curr_char == '.' )
                    {
                        state = Token::DOUBLE;
                    }
                    else if ( curr_char == '+' )
                    {
                        token = Token::INCREMENT;
                        state = Token::INCREMENT;
                    }
                    else if ( curr_char == '=' )
                    {
                        token = Token::ADD_EQUAL;
                        state = Token::ADD_EQUAL;
                    }
                    else if ( charIsNumeric( curr_char ) )
                    {
                        state = Token::INT;
                        token = Token::INT;
                    }
                    else
                    {
                        token = Token::ADD;
                        if (( state = getOpFinalState( curr_char ) ) == END_TOKEN )
                        {
                            continue;
                        }
                    }
                    break;
                }

                case SUB:
                {
                    if ( curr_char == '.' )
                    {
                        state = Token::DOUBLE;
                    }
                    else if ( curr_char == '-' )
                    {
                        token = Token::DECREMENT;
                        state = Token::DECREMENT;
                    }
                    else if ( curr_char == '=' )
                    {
                        token = Token::SUB_EQUAL;
                        state = Token::SUB_EQUAL;
                    }
                    else if ( charIsNumeric( curr_char ) )
                    {
                        state = Token::INT;
                        token = Token::INT;
                    }
                    else
                    {
                        token = Token::SUB;
                        if (( state = getOpFinalState( curr_char ) ) == END_TOKEN )
                        {
                            continue;
                        }
                    }
                    break;
                }

                case INT:
                {
                    if ( charIsNumeric( curr_char ) )
                    {
                        state = Token::INT;
                    }
                    else if ( curr_char == '.' )
                    {
                        token = Token::DOUBLE;
                        state = Token::DOUBLE;
                    }
                    else
                    {
                        state = Token::END_TOKEN;
                        continue;
                    }
                    break;
                }

                case DOUBLE:
                {
                    if ( charIsNumeric( curr_char ) )
                    {
                        state = Token::DOUBLE;
                    }
                    else if ( curr_char == 'e' || curr_char == 'E' )
                    {
                        state = Token::DOUBLE_E;
                    }
                    else if ( curr_char == 'f' )
                    {
                        token = Token::FLOAT;
                        state = Token::FLOAT;
                    }
                    else
                    {
                        state = Token::END_TOKEN;
                        continue;
                    }
                    break;
                }

                case DOUBLE_E:
                {
                    token = Token::FLOAT;
                    if ( charIsNumeric( curr_char ) )
                    {
                        state = Token::DOUBLE_E;
                    }
                    else if ( curr_char == '+' || curr_char == '-' )
                    {
                        state = Token::DOUBLE_EX;
                    }
                    else
                    {
                        state = Token::END_TOKEN;
                        continue;
                    }
                    break;
                }

                case DOUBLE_EX:
                {
                    token = Token::FLOAT;
                    if ( charIsNumeric( curr_char ) )
                    {
                        state = Token::DOUBLE_EX;
                    }
                    else if ( curr_char == 'f' )
                    {
                        state = Token::FLOAT;
                    }
                    else
                    {
                        state = Token::END_TOKEN;
                        continue;
                    }
                    break;
                }

                case FLOAT:
                {
                    if ( charIsDelimiter( curr_char ) )
                    {
                        state = Token::END_TOKEN;
                        token = Token::FLOAT;
                        continue;
                    }
                    else 
                    {
                        state = Token::ERROR;
                    }
                    break;
                }

                case BLOCK_COMMENT:
                {
                    if ( prev_char == '*' || curr_char == '/' )
                    {
                        state = Token::START_TOKEN;
                    }
                    else 
                    {
                        state = Token::BLOCK_COMMENT;
                        if ( curr_char != '\n' )
                        {
                            curr_char = ' ';
                        }else
                        {
                            line_count++;
                        }
                    }
                    curr_position++;
                    continue;
                }
                break;

                case INLINE_COMMENT:
                {
                    state = Token::INLINE_COMMENT;
                    if ( curr_char == '\n' )
                    {
                        state = Token::START_TOKEN;
                    }
                    else 
                    { 
                        curr_position++;
                    }
                    continue;
                }

                case IDENTIFIER:
                {
                    if ( charIsId( curr_char ) )
                    {
                        state = Token::IDENTIFIER;
                    }
                    else if ( charIsDelimiter( curr_char ) )
                    {
                        state = Token::END_TOKEN;
                        continue;
                    }
                    else if ( charIsOperator( curr_char ) )
                    {
                        state = Token::END_TOKEN;
                        continue;
                    }
                    else 
                    {
                        state = Token::ERROR;
                        continue;
                    }
                    break;
                }

                default: 
                {
                    state = Token::ERROR;
                    continue;
                }
            }

            lexeme[ index++ ] = curr_char;
            curr_position++;
        }

        lexeme[ index ] = 0;

        if ( state == Token::ERROR )
        {
            _error = std::string( "[ ERROR ]: " ) + " : [ issue ]: @line -> [" +  std::to_string( line_count ) + "] ";
        }
        
        if( token == Token::IDENTIFIER )
        {
            uint32_t* t = token_table->find( lexeme );
            if( t != nullptr )
            {
                token = (Token)*t;
            }
        }
        return token;
    }

    void Lexer::revert()
    {
        if( prev_position ) curr_position = prev_position;
    }

    int Lexer::getCurrLine()
    {
        return line_count;
    }

    const char* Lexer::getPrevLexeme()const
    {
        return prev_lexeme;
    }

    const char* Lexer::getLexeme()
    {
        return lexeme;
    }

    void Lexer::begin( const std::vector< char >* buffer )
    {
        curr_position = buffer->data();
        prev_position = nullptr;
    }

    void Lexer::clear()
    {
        writebufr.clear();
        curr_position = nullptr;
        prev_position = nullptr;
        line_count = 0;
    }

    Lexer:: ~Lexer()
    {
        clear();
    }


    Lexer::Lexer( const std::vector< char >* buffer )
    :   curr_position( buffer->data() )
    ,   prev_position( buffer->data() )
    ,   line_count( 0 )
    {
        token_table = getGLSLTokenTable();
        writebufr.resize( 128 );
        lexeme = writebufr.data();
        lexeme[0] = 0;
    }

    Lexer::Lexer()
    :   curr_position( nullptr )
    ,   prev_position( nullptr )
    ,   line_count( 0 )
    {
        line_count = 0;
        writebufr.resize( 128 );
        lexeme = writebufr.data();
        lexeme[0] = 0;

        token_table = getGLSLTokenTable();
    }

}}

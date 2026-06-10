#include "glsl-preprocessor.hpp"

namespace kege{namespace glsl{







    bool strBeginWith( const char* s1, const char* s2 )
    {
        while (*s1 != 0 && *s2 != 0)
        {
            if ( *s1 == *s2 )
            {
                s1++;
                s2++;
                continue;
            }
            break;
        }
        return *s1 == 0 || *s2 == 0;
    }

    bool Preprocessor::preprocess( char* line_buffer, LineNode*& head, LineNode*& tail, const char* filename )
    {
        // ---- Open the input file
        FILE * file;
        if ( ! ( file = fopen ( filename, "r" ) ) )
        {
            if ( _err.empty() )
            {
                _err = std::string( "failed to open file -> " ) + filename;
            }
            return false;
        }

        int line_count = 0;
        int file_index = (int)_filenames.size();
        _filenames.push_back( filename );

        while ( ! feof ( file ) )
        {
            // Read the line from the file
            fgets ( line_buffer, MAX_LINE_LENGTH, file );
            line_buffer[ MAX_LINE_LENGTH ] = 0;
            line_count++;

            if ( line_buffer[ 0 ] == '#' )
            {   
                char* s = &line_buffer[ 1 ];
                if ( strBeginWith( s, "include" ) )
                {
                    s += 8;
                    while ( *s != '"' ) s++;
                    if ( *s == '"' ) s++;

                    int i = int( strlen( s ) - 1 );
                    while (s[ i ] != '"' && i > 0 ) 
                    {
                        s[ i-- ] = 0;
                    }

                    LineNode* h = nullptr;
                    LineNode* t = nullptr;

                    if ( preprocess( line_buffer, h, t, s ) )
                    {
                        if ( head == nullptr )
                        {
                            head = h;
                            tail = t;
                        }
                        else
                        {
                            tail->next = h;
                            h->prev = tail;
                            tail = t;
                        }
                    }
                    else
                    {
                        _err = std::string( "failed to open included file -> " ) + filename;
                        return false;
                    }
                }
            }
            else 
            {
                if( head == nullptr )
                {
                    tail = head = new LineNode;
                    tail->file_index = file_index;
                    tail->data = line_buffer;
                    tail->next = nullptr;
                    tail->prev = nullptr;
                    tail->line = line_count;
                    line_buffer[0]=0;
                }
                else
                {
                    tail->next = new LineNode;
                    tail->next->file_index = file_index;
                    tail->next->data = line_buffer;
                    tail->next->next = nullptr;
                    tail->next->prev = tail;
                    tail->next->line = line_count;
                    tail = tail->next;
                    line_buffer[0]=0;
                }
            }
        }
        fclose ( file );
        return true;
    }

    bool Preprocessor::load( const char* filename )
    {
        char line_buffer[ MAX_LINE_LENGTH + 1 ];
        return preprocess( line_buffer, _head, _tail, filename );
    }

    const std::string& Preprocessor::error()const
    {
        return _err;
    }

    int Preprocessor::getCurrentLineNumber()
    {
        return ( _node ) ? _node->line : -1;
    }

    void Preprocessor::startFromBeginnig()
    {
        _node = _head;
        _curr_line = _node->data.c_str();
    }

    void Preprocessor::startFromEnd()
    {
        _node = _tail;
        _curr_line = &_node->data[ _node->data.length() - 1 ];
    }

    const char* Preprocessor::nextLine()
    {
        if ( _node )
        {
            _node = _node->next;
            if ( _node ) 
            {
                return _node->data.c_str();
            }
        }
        return nullptr;
    }

    const char* Preprocessor::prevLine()
    {
        if ( _node )
        {
            _node = _node->prev;
            if ( _node ) 
            {
                return &_node->data[ _node->data.length() - 1 ];
            }
        }
        return nullptr;
    }

    void Preprocessor::setCheckPoint()
    {
        _check_point = _curr_line;
    }

    void Preprocessor::gotoCheckPoint()
    {
        if ( _check_point )
        {
            _curr_line = _check_point;
        }
    }

    char Preprocessor::operator ++( int )
    {
        if ( _curr_line )
        {
            char c = *_curr_line++;
            if ( *_curr_line == 0 )
            {
                _curr_line = nextLine();
            }
            return c;
        }
        return 0;
    }

    char Preprocessor::operator --( int )
    {
        if ( _curr_line )
        {
            char c = *_curr_line++;
            if ( *_curr_line == 0 )
            {
                _curr_line = prevLine();
            }
            return c;
        }
        return 0;
    }

    char Preprocessor::operator  *()
    {   
        return ( _curr_line ) ? *_curr_line : 0;
    }

    void destroy( Preprocessor::LineNode*& node )
    {
        Preprocessor::LineNode* n;
        while( node )
        {
            n = node;
            node = node->next;
            delete n;
        }
    }

    void Preprocessor::clear()
    {
        destroy( _head );
        _tail = _head = nullptr;
    }

    Preprocessor::~Preprocessor()
    {
        clear();
    }

    Preprocessor::Preprocessor()
    :   _head( nullptr )
    ,   _tail( nullptr )
    {}

}}

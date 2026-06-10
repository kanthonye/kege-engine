//
//  load-code.cpp
//  physics
//
//  Created by Kenneth Esdaile on 10/19/25.
//

#include "load-code.hpp"
#include <unordered_set>

namespace kege{

    struct CharLink
    {
        std::string line;
        CharLink* next;
    };

    struct CharLinkList
    {
        CharLink* head = nullptr;
        CharLink* tail = nullptr;
        size_t buffer_byte_size = 0;

        CharLinkList()
        {
            while ( head != nullptr )
            {
                CharLink* h = head;
                head = head->next;
                delete h;
            }
            head = tail = nullptr;
            buffer_byte_size = 0;
        }
    };

    bool recursiveLoadText( const int MAX_LINE_LENGTH, char* buffer, CharLinkList& list, std::unordered_set<std::string>& included_files, const char* filename )
    {
        // Check for circular dependency
        if ( included_files.count( filename ) )
        {
            std::cout << "[ LOAD_FAILURE ]: Cycle dependency detected -> " << filename <<"\n";
            return false;
        }

        // ---- Open the input file
        FILE * file;
        if ( ! ( file = fopen ( filename, "r" ) ) )
        {
            std::cout << filename<<"\n";
            return false;
        }
        included_files.insert( filename );

        const char* p = strchr( filename, '.' );
        size_t size = p - filename;

        while ( filename[size] != '/' && size > 0 )
            size -= 1;

        std::string fpath;
        {
            size += 1;
            char path[ size + 1 ];
            strncpy( path, filename, size );
            path[ size ] = 0;
            fpath = path;
        }

        while ( ! feof ( file ) )
        {
            char* line = fgets( buffer, MAX_LINE_LENGTH, file );
            buffer[ MAX_LINE_LENGTH - 1 ] = 0;

            if ( line == nullptr )
            {
                continue;
            }

            if ( line[ 0 ] == '/' && line[ 1 ] == '*' )
            {
                const char* directive = strstr( line, "include" );
                if ( directive != nullptr )
                {
                    directive = directive + 7;
                    directive = strstr( line, "\"" );

                    directive = directive + 1;
                    const char* str = directive;
                    directive = strpbrk( directive, "\"");

                    const size_t size = directive - str;
                    char fname[ size + 1 ];
                    strncpy( fname, str, size );
                    fname[ size ] = 0;

                    std::string new_filename = fpath + fname;
                    if ( !recursiveLoadText( MAX_LINE_LENGTH, buffer, list, included_files, new_filename.data() ) )
                    {
                        std::cout << "[ LOAD_FAILURE ]: include file -> " << new_filename << " in " << filename <<"\n";
                        return false;
                    }
                }
                else
                {
                    while( strstr( buffer, "*/") == nullptr && buffer[0] == 0 )
                    {
                        fgets ( buffer, MAX_LINE_LENGTH, file );
                    }
                }
            }
            else
            {
                if ( list.head == nullptr )
                {
                    list.tail = list.head = new CharLink
                    {
                        .line = line,
                        .next = nullptr
                    };
                    list.buffer_byte_size = strlen( line );
                }
                else
                {
                    list.tail->next = new CharLink
                    {
                        .line = line,
                        .next = nullptr
                    };
                    list.tail = list.tail->next;
                    list.buffer_byte_size += strlen( line );
                }
            }
        }
        fclose ( file );
        included_files.erase( filename );
        return true;
    }

    std::vector< char > loadCode( const char* filename )
    {
        CharLinkList list;
        const int MAX_LINE_LENGTH = 256;
        char buffer[ MAX_LINE_LENGTH + 1 ];
        std::unordered_set<std::string> included_files;
        if ( !recursiveLoadText( MAX_LINE_LENGTH, buffer, list, included_files, filename ) )
        {
            return {};
        };

        size_t offset = 0;
        std::vector< char > data( list.buffer_byte_size + 1 );
        for ( CharLink* link = list.head; link != nullptr; link = link->next )
        {
            memcpy( data.data() + offset, link->line.data(), link->line.length() );
            offset += link->line.length();
        }
        data[ list.buffer_byte_size ] = 0;
        return data;
    }

}

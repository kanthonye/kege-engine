//
//  glsl-preprocessor.hpp
//  kege-game-script-engine
//
//  Created by Kenneth A Esdaile on 4/21/24.
//

#ifndef kege_glsl_preprocessor_hpp
#define kege_glsl_preprocessor_hpp

#include <string>
#include <vector>

namespace kege{namespace glsl{

    class Preprocessor
    {
    public:

        struct LineNode
        {
            std::string data;
            LineNode* prev;
            LineNode* next;
            int file_index;
            int line;
        };

    public:

        /**
         * @brief Load and process the given file.
         * @param line_buffer A tempory buffer, which a line of characters are recorded to before storing it into a node.
         * @param head The current linked-list head.
         * @param tail The current linked-list tail.
         * @param filename The given filename.
         * @note Each node contains one string line of data.
         * @return Return true if load successful, false otherwise.
         **/
        bool preprocess( char* line_buffer, LineNode*& head, LineNode*& tail, const char* filename );

        /**
         * @brief Load the given file.
         * @param filename The given filename.
         * @return Return true if load successful, false otherwise.
         **/
        virtual bool load( const char* filename );

        /**
         * @brief Get the recorded error message if a load failed.
         * @return Return error message.
         **/
        const std::string& error()const;

        int getCurrentLineNumber();

        /**
         * @brief Set the node iterator to the first head of the linked-list
         **/
        void startFromBeginnig();

        /**
         * @brief Set the node iterator to the first tail of the linked-list
         **/
        void startFromEnd();

        /**
         * @brief Get the next line of string starting at the first character
         **/
        const char* nextLine();

        /**
         * @brief Get the previous line of string starting at the last character
         **/
        const char* prevLine();

        void setCheckPoint();
        void gotoCheckPoint();

        /**
         * @brief Get the current character and move to the next charactor
         * @return Return current character if exist, otherwise return null character
         **/
        char operator ++( int );

        /**
         * @brief Get the current character and move to the previous charactor
         * @return Return current character if exist, otherwise return null character
         **/
        char operator --( int );

        /**
         * @brief Get the current character
         * @return Return current character if exist, otherwise return null character
         **/
        char operator  *();

        /**
         * @brief Release occupied memory
         **/
        void clear();

        virtual ~Preprocessor();
        Preprocessor();

    protected:

        std::vector< const char* > _filenames;
        std::string _err;

        /**
         * linked-list head node
         **/
        LineNode* _head;

        /**
         * linked-list tail node
         **/
        LineNode* _tail;

        /**
         * linked-list iterator node
         **/
        LineNode* _node;

        enum{ MAX_LINE_LENGTH = 512 };

        /**
         * the current line of data to read characters from
         **/
        const char* _curr_line;
        const char* _check_point;
    };
    
}}
#endif

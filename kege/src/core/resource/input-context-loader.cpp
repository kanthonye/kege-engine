//
//  input-context-loader.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/19/25.
//

#include "input-context-loader.hpp"
namespace kege{

    kege::Ref< kege::InputContext > InputContextLoader::load( const std::string& filename )
    {
        kege::Json json = JsonParser::load( filename.c_str() );
        if ( !json )
        {
            return nullptr;
        }

        Json keybinds = json[ "keybinds" ];
        if ( !keybinds ) return nullptr;

        kege::InputContext* context = new kege::InputContext;
        for (int i=0; i < keybinds.count(); ++i)
        {
            Json keybind = keybinds[i];

            kege::InputCommand info;
            uint32_t key = stringToKey( keybind[ "key" ].value() );
            info.type    = stringToActionType( keybind[ "type" ].value() );
            info.command = stringToAction( keybind[ "action" ].value() );
            info.amount  = atof( keybind[ "sensitivity" ].value() );

            context->map( key, info );
        }
        return context;
    }

    InputContextLoader::InputContextLoader( AssetManager* am )
    :   kege::AssetLoaderT< ref::InputContext >( am )
    {}
}

namespace kege{
    
    uint32_t stringToKey( const std::string& name )
    {
        static std::unordered_map< std::string, uint32_t > keys;
        if ( keys.empty() )
        {
            keys[ "BUTTON_LEFT" ] = kege::BUTTON_LEFT;
            keys[ "BUTTON_RIGHT" ] = kege::BUTTON_RIGHT;
            keys[ "BUTTON_LEFT" ] = kege::BUTTON_LEFT;
            keys[ "BUTTON_MIDDLE" ] = kege::BUTTON_MIDDLE;

            keys[ "BUTTON_0" ] = kege::BUTTON_0;
            keys[ "BUTTON_1" ] = kege::BUTTON_1;
            keys[ "BUTTON_2" ] = kege::BUTTON_2;
            keys[ "BUTTON_3" ] = kege::BUTTON_3;
            keys[ "BUTTON_4" ] = kege::BUTTON_4;
            keys[ "BUTTON_5" ] = kege::BUTTON_5;
            keys[ "BUTTON_6" ] = kege::BUTTON_6;
            keys[ "BUTTON_7" ] = kege::BUTTON_7;
            keys[ "BUTTON_8" ] = kege::BUTTON_8;
            keys[ "BUTTON_9" ] = kege::BUTTON_9;
            keys[ "BUTTON_10" ] = kege::BUTTON_10;
            keys[ "BUTTON_11" ] = kege::BUTTON_11;
            keys[ "BUTTON_12" ] = kege::BUTTON_12;
            keys[ "BUTTON_13" ] = kege::BUTTON_13;
            keys[ "BUTTON_14" ] = kege::BUTTON_14;
            keys[ "BUTTON_15" ] = kege::BUTTON_15;

            keys[ "POINTER_X_NEGATIVE" ] = kege::POINTER_X_NEGATIVE;
            keys[ "POINTER_X_POSITIVE" ] = kege::POINTER_X_POSITIVE;
            keys[ "POINTER_Y_NEGATIVE" ] = kege::POINTER_Y_NEGATIVE;
            keys[ "POINTER_Y_POSITIVE" ] = kege::POINTER_Y_POSITIVE;

            keys[ "SCROLL_X_NEGATIVE" ] = kege::SCROLL_X_NEGATIVE;
            keys[ "SCROLL_X_POSITIVE" ] = kege::SCROLL_X_POSITIVE;
            keys[ "SCROLL_Y_NEGATIVE" ] = kege::SCROLL_Y_NEGATIVE;
            keys[ "SCROLL_Y_POSITIVE" ] = kege::SCROLL_Y_POSITIVE;

            keys[ "KEY_SPACE" ] = kege::KEY_SPACE;
            keys[ "KEY_APOSTROPHE" ] = kege::KEY_APOSTROPHE;
            keys[ "KEY_MINUS" ] = kege::KEY_MINUS;
            keys[ "KEY_PERIOD" ] = kege::KEY_PERIOD;
            keys[ "KEY_SLASH" ] = kege::KEY_SLASH;
            keys[ "KEY_1" ] = kege::KEY_1;
            keys[ "KEY_2" ] = kege::KEY_2;
            keys[ "KEY_3" ] = kege::KEY_3;
            keys[ "KEY_4" ] = kege::KEY_4;
            keys[ "KEY_5" ] = kege::KEY_5;
            keys[ "KEY_6" ] = kege::KEY_6;
            keys[ "KEY_7" ] = kege::KEY_7;
            keys[ "KEY_8" ] = kege::KEY_8;
            keys[ "KEY_9" ] = kege::KEY_9;
            keys[ "KEY_SEMICOLON" ] = kege::KEY_SEMICOLON;
            keys[ "KEY_EQUAL" ] = kege::KEY_EQUAL;
            keys[ "KEY_A" ] = kege::KEY_A;
            keys[ "KEY_B" ] = kege::KEY_B;
            keys[ "KEY_C" ] = kege::KEY_C;
            keys[ "KEY_D" ] = kege::KEY_D;
            keys[ "KEY_E" ] = kege::KEY_E;
            keys[ "KEY_F" ] = kege::KEY_F;
            keys[ "KEY_G" ] = kege::KEY_G;
            keys[ "KEY_H" ] = kege::KEY_H;
            keys[ "KEY_I" ] = kege::KEY_I;
            keys[ "KEY_J" ] = kege::KEY_J;
            keys[ "KEY_K" ] = kege::KEY_K;
            keys[ "KEY_L" ] = kege::KEY_L;
            keys[ "KEY_M" ] = kege::KEY_M;
            keys[ "KEY_N" ] = kege::KEY_N;
            keys[ "KEY_O" ] = kege::KEY_O;
            keys[ "KEY_P" ] = kege::KEY_P;
            keys[ "KEY_Q" ] = kege::KEY_Q;
            keys[ "KEY_R" ] = kege::KEY_R;
            keys[ "KEY_S" ] = kege::KEY_S;
            keys[ "KEY_T" ] = kege::KEY_T;
            keys[ "KEY_U" ] = kege::KEY_U;
            keys[ "KEY_V" ] = kege::KEY_V;
            keys[ "KEY_W" ] = kege::KEY_W;
            keys[ "KEY_X" ] = kege::KEY_X;
            keys[ "KEY_Y" ] = kege::KEY_Y;
            keys[ "KEY_Z" ] = kege::KEY_Z;
            keys[ "KEY_LEFT_BRACKET" ] = kege::KEY_LEFT_BRACKET;
            keys[ "KEY_RIGHT_BRACKET" ] = kege::KEY_RIGHT_BRACKET;
            keys[ "KEY_GRAVE_ACCENT" ] = kege::KEY_GRAVE_ACCENT;

            keys[ "KEY_ENTER" ] = kege::KEY_ENTER;
            keys[ "KEY_ESCAPE" ] = kege::KEY_ESCAPE;
            keys[ "KEY_TAB" ] = kege::KEY_TAB;
            keys[ "KEY_BACKSPACE" ] = kege::KEY_BACKSPACE;
            keys[ "KEY_INSERT" ] = kege::KEY_INSERT;
            keys[ "KEY_DELETE" ] = kege::KEY_DELETE;
            keys[ "KEY_RIGHT" ] = kege::KEY_RIGHT;
            keys[ "KEY_LEFT" ] = kege::KEY_LEFT;
            keys[ "KEY_DOWN" ] = kege::KEY_DOWN;
            keys[ "KEY_UP" ] = kege::KEY_UP;
            keys[ "KEY_PAGE_UP" ] = kege::KEY_PAGE_UP;

            keys[ "KEY_PAGE_DOWN" ] = kege::KEY_PAGE_DOWN;
            keys[ "KEY_HOME" ] = kege::KEY_HOME;
            keys[ "KEY_END" ] = kege::KEY_END;
            keys[ "KEY_CAPS_LOCK" ] = kege::KEY_CAPS_LOCK;
            keys[ "KEY_SCROLL_LOCK" ] = kege::KEY_SCROLL_LOCK;
            keys[ "KEY_NUM_LOCK" ] = kege::KEY_NUM_LOCK;
            keys[ "KEY_PRINT_SCREEN" ] = kege::KEY_PRINT_SCREEN;
            keys[ "KEY_PAUSE" ] = kege::KEY_PAUSE;

            keys[ "KEY_F1" ] = kege::KEY_F1;
            keys[ "KEY_F2" ] = kege::KEY_F2;
            keys[ "KEY_F3" ] = kege::KEY_F3;
            keys[ "KEY_F4" ] = kege::KEY_F4;
            keys[ "KEY_F5" ] = kege::KEY_F5;
            keys[ "KEY_F6" ] = kege::KEY_F6;
            keys[ "KEY_F7" ] = kege::KEY_F7;
            keys[ "KEY_F8" ] = kege::KEY_F8;
            keys[ "KEY_F9" ] = kege::KEY_F9;
            keys[ "KEY_F10" ] = kege::KEY_F10;
            keys[ "KEY_F11" ] = kege::KEY_F11;
            keys[ "KEY_F12" ] = kege::KEY_F12;
            keys[ "KEY_F13" ] = kege::KEY_F13;
            keys[ "KEY_F14" ] = kege::KEY_F14;
            keys[ "KEY_F15" ] = kege::KEY_F15;
            keys[ "KEY_F16" ] = kege::KEY_F16;
            keys[ "KEY_F17" ] = kege::KEY_F17;
            keys[ "KEY_F18" ] = kege::KEY_F18;
            keys[ "KEY_F19" ] = kege::KEY_F19;
            keys[ "KEY_F20" ] = kege::KEY_F20;
            keys[ "KEY_F21" ] = kege::KEY_F21;
            keys[ "KEY_F22" ] = kege::KEY_F22;
            keys[ "KEY_F23" ] = kege::KEY_F23;
            keys[ "KEY_F24" ] = kege::KEY_F24;
            keys[ "KEY_F25" ] = kege::KEY_F25;

            keys[ "KEY_KP_1" ] = kege::KEY_KP_1;
            keys[ "KEY_KP_2" ] = kege::KEY_KP_2;
            keys[ "KEY_KP_3" ] = kege::KEY_KP_3;
            keys[ "KEY_KP_4" ] = kege::KEY_KP_4;
            keys[ "KEY_KP_5" ] = kege::KEY_KP_5;
            keys[ "KEY_KP_6" ] = kege::KEY_KP_6;
            keys[ "KEY_KP_7" ] = kege::KEY_KP_7;
            keys[ "KEY_KP_8" ] = kege::KEY_KP_8;
            keys[ "KEY_KP_9" ] = kege::KEY_KP_9;

            keys[ "KEY_KP_DECIMAL" ] = kege::KEY_KP_DECIMAL;
            keys[ "KEY_KP_DIVIDE" ] = kege::KEY_KP_DIVIDE;
            keys[ "KEY_KP_MULTIPLY" ] = kege::KEY_KP_MULTIPLY;
            keys[ "KEY_KP_SUBTRACT" ] = kege::KEY_KP_SUBTRACT;
            keys[ "KEY_LEFT_SHIFT" ] = kege::KEY_LEFT_SHIFT;
            keys[ "KEY_KP_ADD" ] = kege::KEY_KP_ADD;
            keys[ "KEY_KP_ENTER" ] = kege::KEY_KP_ENTER;
            keys[ "KEY_LEFT_CONTROL" ] = kege::KEY_LEFT_CONTROL;
            keys[ "KEY_KP_EQUAL" ] = kege::KEY_KP_EQUAL;
            keys[ "KEY_LEFT_ALT" ] = kege::KEY_LEFT_ALT;
            keys[ "KEY_LEFT_SUPER" ] = kege::KEY_LEFT_SUPER;
            keys[ "KEY_RIGHT_SHIFT" ] = kege::KEY_RIGHT_SHIFT;
            keys[ "KEY_RIGHT_CONTROL" ] = kege::KEY_RIGHT_CONTROL;
            keys[ "KEY_RIGHT_ALT" ] = kege::KEY_RIGHT_ALT;
            keys[ "KEY_RIGHT_SUPER" ] = kege::KEY_RIGHT_SUPER;
            keys[ "KEY_MENU" ] = kege::KEY_MENU;
        }

        auto i = keys.find( name );
        if ( i == keys.end() ) return 0;
        return i->second;
    }

    uint32_t stringToAction( const std::string& name )
    {
        static std::map< std::string, uint32_t > types;

        if ( types.empty() )
        {
            types[ "STATE" ] =  kege::InputCommand::STATE;
            types[ "ACTION" ] = kege::InputCommand::ACTION;
            types[ "RANGE" ] =  kege::InputCommand::RANGE;
        }

        auto i = types.find( name );
        if ( i == types.end() ) return 0;
        return i->second;
    }

    uint32_t stringToActionType( const std::string& name )
    {
        static std::unordered_map< std::string, uint32_t > types;

        if ( types.empty() )
        {
            types[ "STATE" ] =  kege::InputCommand::STATE;
            types[ "ACTION" ] = kege::InputCommand::ACTION;
            types[ "RANGE" ] =  kege::InputCommand::RANGE;
        }

        if ( types.empty() )
        {
            for (int i=0; i<kege::TOTAL_ENUM_ACTIONS; i++)
            {
                types[ enumActionToString( i ) ] = i;
            }
        }

        auto i = types.find( name );
        if ( i == types.end() ) return 0;
        return i->second;
    }
}

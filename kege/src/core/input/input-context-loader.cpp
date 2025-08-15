//
//  input-context-loader.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/19/25.
//

#include "input-context-loader.hpp"
namespace kege{

    kege::Hashmap< uint32_t > InputContextLoader::keys;
    kege::Hashmap< uint32_t > InputContextLoader::types;
    kege::Hashmap< uint32_t > InputContextLoader::actions;

    kege::Ref< kege::InputContext > InputContextLoader::load( const kege::string& filename )
    {
        kege::Json json = JsonParser::load( filename.c_str() );
        if ( !json )
        {
            return nullptr;
        }
        populate();
        
        Json keybinds = json[ "keybinds" ];
        if ( !keybinds ) return nullptr;

        kege::InputContext* context = new kege::InputContext;
        for (int i=0; i < keybinds.count(); ++i)
        {
            Json keybind = keybinds[i];

            kege::InputCommand info;
            uint32_t key = *keys.at( keybind[ "key" ].value() );
            info.type    = *types.at( keybind[ "type" ].value() );
            info.command = *actions.at( keybind[ "action" ].value() );
            info.amount  = atof( keybind[ "sensitivity" ].value() );

            context->map( key, info );
        }
        return context;
    }
    
    void InputContextLoader::populate()
    {
        if ( keys.empty() )
        {
            keys.insert( "BUTTON_LEFT", kege::BUTTON_LEFT );
            keys.insert( "BUTTON_RIGHT", kege::BUTTON_RIGHT );
            keys.insert( "BUTTON_LEFT", kege::BUTTON_LEFT );
            keys.insert( "BUTTON_MIDDLE", kege::BUTTON_MIDDLE );

            keys.insert( "BUTTON_0", kege::BUTTON_0 );
            keys.insert( "BUTTON_1", kege::BUTTON_1 );
            keys.insert( "BUTTON_2", kege::BUTTON_2 );
            keys.insert( "BUTTON_3", kege::BUTTON_3 );
            keys.insert( "BUTTON_4", kege::BUTTON_4 );
            keys.insert( "BUTTON_5", kege::BUTTON_5 );
            keys.insert( "BUTTON_6", kege::BUTTON_6 );
            keys.insert( "BUTTON_7", kege::BUTTON_7 );
            keys.insert( "BUTTON_8", kege::BUTTON_8 );
            keys.insert( "BUTTON_9", kege::BUTTON_9 );
            keys.insert( "BUTTON_10", kege::BUTTON_10 );
            keys.insert( "BUTTON_11", kege::BUTTON_11 );
            keys.insert( "BUTTON_12", kege::BUTTON_12 );
            keys.insert( "BUTTON_13", kege::BUTTON_13 );
            keys.insert( "BUTTON_14", kege::BUTTON_14 );
            keys.insert( "BUTTON_15", kege::BUTTON_15 );

            keys.insert( "POINTER_X_NEGATIVE", kege::POINTER_X_NEGATIVE );
            keys.insert( "POINTER_X_POSITIVE", kege::POINTER_X_POSITIVE );
            keys.insert( "POINTER_Y_NEGATIVE", kege::POINTER_Y_NEGATIVE );
            keys.insert( "POINTER_Y_POSITIVE", kege::POINTER_Y_POSITIVE );

            keys.insert( "SCROLL_X_NEGATIVE", kege::SCROLL_X_NEGATIVE );
            keys.insert( "SCROLL_X_POSITIVE", kege::SCROLL_X_POSITIVE );
            keys.insert( "SCROLL_Y_NEGATIVE", kege::SCROLL_Y_NEGATIVE );
            keys.insert( "SCROLL_Y_POSITIVE", kege::SCROLL_Y_POSITIVE );

            keys.insert( "KEY_SPACE", kege::KEY_SPACE );
            keys.insert( "KEY_APOSTROPHE", kege::KEY_APOSTROPHE );
            keys.insert( "KEY_MINUS", kege::KEY_MINUS );
            keys.insert( "KEY_PERIOD", kege::KEY_PERIOD );
            keys.insert( "KEY_SLASH", kege::KEY_SLASH );
            keys.insert( "KEY_1", kege::KEY_1 );
            keys.insert( "KEY_2", kege::KEY_2 );
            keys.insert( "KEY_3", kege::KEY_3 );
            keys.insert( "KEY_4", kege::KEY_4 );
            keys.insert( "KEY_5", kege::KEY_5 );
            keys.insert( "KEY_6", kege::KEY_6 );
            keys.insert( "KEY_7", kege::KEY_7 );
            keys.insert( "KEY_8", kege::KEY_8 );
            keys.insert( "KEY_9", kege::KEY_9 );
            keys.insert( "KEY_SEMICOLON", kege::KEY_SEMICOLON );
            keys.insert( "KEY_EQUAL", kege::KEY_EQUAL );
            keys.insert( "KEY_A", kege::KEY_A );
            keys.insert( "KEY_B", kege::KEY_B );
            keys.insert( "KEY_C", kege::KEY_C );
            keys.insert( "KEY_D", kege::KEY_D );
            keys.insert( "KEY_E", kege::KEY_E );
            keys.insert( "KEY_F", kege::KEY_F );
            keys.insert( "KEY_G", kege::KEY_G );
            keys.insert( "KEY_H", kege::KEY_H );
            keys.insert( "KEY_I", kege::KEY_I );
            keys.insert( "KEY_J", kege::KEY_J );
            keys.insert( "KEY_K", kege::KEY_K );
            keys.insert( "KEY_L", kege::KEY_L );
            keys.insert( "KEY_M", kege::KEY_M );
            keys.insert( "KEY_N", kege::KEY_N );
            keys.insert( "KEY_O", kege::KEY_O );
            keys.insert( "KEY_P", kege::KEY_P );
            keys.insert( "KEY_Q", kege::KEY_Q );
            keys.insert( "KEY_R", kege::KEY_R );
            keys.insert( "KEY_S", kege::KEY_S );
            keys.insert( "KEY_T", kege::KEY_T );
            keys.insert( "KEY_U", kege::KEY_U );
            keys.insert( "KEY_V", kege::KEY_V );
            keys.insert( "KEY_W", kege::KEY_W );
            keys.insert( "KEY_X", kege::KEY_X );
            keys.insert( "KEY_Y", kege::KEY_Y );
            keys.insert( "KEY_Z", kege::KEY_Z );
            keys.insert( "KEY_LEFT_BRACKET", kege::KEY_LEFT_BRACKET );
            keys.insert( "KEY_RIGHT_BRACKET", kege::KEY_RIGHT_BRACKET );
            keys.insert( "KEY_GRAVE_ACCENT", kege::KEY_GRAVE_ACCENT );

            keys.insert( "KEY_ENTER", kege::KEY_ENTER );
            keys.insert( "KEY_ESCAPE", kege::KEY_ESCAPE );
            keys.insert( "KEY_TAB", kege::KEY_TAB );
            keys.insert( "KEY_BACKSPACE", kege::KEY_BACKSPACE );
            keys.insert( "KEY_INSERT", kege::KEY_INSERT );
            keys.insert( "KEY_DELETE", kege::KEY_DELETE );
            keys.insert( "KEY_RIGHT", kege::KEY_RIGHT );
            keys.insert( "KEY_LEFT", kege::KEY_LEFT );
            keys.insert( "KEY_DOWN", kege::KEY_DOWN );
            keys.insert( "KEY_UP", kege::KEY_UP );
            keys.insert( "KEY_PAGE_UP", kege::KEY_PAGE_UP );

            keys.insert( "KEY_PAGE_DOWN", kege::KEY_PAGE_DOWN );
            keys.insert( "KEY_HOME", kege::KEY_HOME );
            keys.insert( "KEY_END", kege::KEY_END );
            keys.insert( "KEY_CAPS_LOCK", kege::KEY_CAPS_LOCK );
            keys.insert( "KEY_SCROLL_LOCK", kege::KEY_SCROLL_LOCK );
            keys.insert( "KEY_NUM_LOCK", kege::KEY_NUM_LOCK );
            keys.insert( "KEY_PRINT_SCREEN", kege::KEY_PRINT_SCREEN );
            keys.insert( "KEY_PAUSE", kege::KEY_PAUSE );

            keys.insert( "KEY_F1", kege::KEY_F1 );
            keys.insert( "KEY_F2", kege::KEY_F2 );
            keys.insert( "KEY_F3", kege::KEY_F3 );
            keys.insert( "KEY_F4", kege::KEY_F4 );
            keys.insert( "KEY_F5", kege::KEY_F5 );
            keys.insert( "KEY_F6", kege::KEY_F6 );
            keys.insert( "KEY_F7", kege::KEY_F7 );
            keys.insert( "KEY_F8", kege::KEY_F8 );
            keys.insert( "KEY_F9", kege::KEY_F9 );
            keys.insert( "KEY_F10", kege::KEY_F10 );
            keys.insert( "KEY_F11", kege::KEY_F11 );
            keys.insert( "KEY_F12", kege::KEY_F12 );
            keys.insert( "KEY_F13", kege::KEY_F13 );
            keys.insert( "KEY_F14", kege::KEY_F14 );
            keys.insert( "KEY_F15", kege::KEY_F15 );
            keys.insert( "KEY_F16", kege::KEY_F16 );
            keys.insert( "KEY_F17", kege::KEY_F17 );
            keys.insert( "KEY_F18", kege::KEY_F18 );
            keys.insert( "KEY_F19", kege::KEY_F19 );
            keys.insert( "KEY_F20", kege::KEY_F20 );
            keys.insert( "KEY_F21", kege::KEY_F21 );
            keys.insert( "KEY_F22", kege::KEY_F22 );
            keys.insert( "KEY_F23", kege::KEY_F23 );
            keys.insert( "KEY_F24", kege::KEY_F24 );
            keys.insert( "KEY_F25", kege::KEY_F25 );

            keys.insert( "KEY_KP_1", kege::KEY_KP_1 );
            keys.insert( "KEY_KP_2", kege::KEY_KP_2 );
            keys.insert( "KEY_KP_3", kege::KEY_KP_3 );
            keys.insert( "KEY_KP_4", kege::KEY_KP_4 );
            keys.insert( "KEY_KP_5", kege::KEY_KP_5 );
            keys.insert( "KEY_KP_6", kege::KEY_KP_6 );
            keys.insert( "KEY_KP_7", kege::KEY_KP_7 );
            keys.insert( "KEY_KP_8", kege::KEY_KP_8 );
            keys.insert( "KEY_KP_9", kege::KEY_KP_9 );

            keys.insert( "KEY_KP_DECIMAL", kege::KEY_KP_DECIMAL );
            keys.insert( "KEY_KP_DIVIDE", kege::KEY_KP_DIVIDE );
            keys.insert( "KEY_KP_MULTIPLY", kege::KEY_KP_MULTIPLY );
            keys.insert( "KEY_KP_SUBTRACT", kege::KEY_KP_SUBTRACT );
            keys.insert( "KEY_LEFT_SHIFT", kege::KEY_LEFT_SHIFT );
            keys.insert( "KEY_KP_ADD", kege::KEY_KP_ADD );
            keys.insert( "KEY_KP_ENTER", kege::KEY_KP_ENTER );
            keys.insert( "KEY_LEFT_CONTROL", kege::KEY_LEFT_CONTROL );
            keys.insert( "KEY_KP_EQUAL", kege::KEY_KP_EQUAL );
            keys.insert( "KEY_LEFT_ALT", kege::KEY_LEFT_ALT );
            keys.insert( "KEY_LEFT_SUPER", kege::KEY_LEFT_SUPER );
            keys.insert( "KEY_RIGHT_SHIFT", kege::KEY_RIGHT_SHIFT );
            keys.insert( "KEY_RIGHT_CONTROL", kege::KEY_RIGHT_CONTROL );
            keys.insert( "KEY_RIGHT_ALT", kege::KEY_RIGHT_ALT );
            keys.insert( "KEY_RIGHT_SUPER", kege::KEY_RIGHT_SUPER );
            keys.insert( "KEY_MENU", kege::KEY_MENU );
        }

        if ( types.empty() )
        {
            types.insert( "STATE",  kege::InputCommand::STATE );
            types.insert( "ACTION", kege::InputCommand::ACTION );
            types.insert( "RANGE",  kege::InputCommand::RANGE );
        }

        if ( actions.empty() )
        {
            for (int i=0; i<kege::TOTAL_ENUM_ACTIONS; i++)
            {
                actions.insert( enumActionToString( i ), i );
            }
        }

    }
}

//
//  input-device.hpp
//  KEGE
//
//  Created by Kenneth Esdaile on 1/25/25.
//

#ifndef input_handler_hpp
#define input_handler_hpp

#include "../input/inputs.hpp"
#include "../input/thread-safe-vector.hpp"
#include "../graphics/core/graphics-window.hpp"

namespace kege{

    class GraphicsWindow;

    class UserInputReceiver : public WindowListener
    {
    public:

        void onKeyboard( int key, int scancode, int action, int mods );
        void onMouseButton( int button, int action, int mods );
        void onCursorPosition( double xpos, double ypos );
        void onScroll( double xoffset, double yoffset );

        void getInputs( std::vector< Input >& inputs );
        bool initialize( GraphicsWindow* window );
        void shutdown();
        UserInputReceiver();

    private:

        ThreadSafeVector< Input > _inputs;
    };

}

#endif /* input_handler_hpp */

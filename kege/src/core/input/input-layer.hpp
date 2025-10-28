//
//  input-layer.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/16/25.
//

#ifndef kege_input_layer_hpp
#define kege_input_layer_hpp

#include "app-layer.hpp"
#include "../../core/input/input-context.hpp"
#include "../../core/input/user-input-receiver.hpp"
#include "../../core/input/input-context-manager.hpp"

namespace kege{

    class InputLayer : public kege::AppLayer
    {
    public:

        InputLayer( ref::InputContextManager icm );
        bool initialize();
        void shutdown();
        void update();

    private:

        ref::InputContextManager _manager;
    };
}

namespace kege::ref{
    typedef kege::Ref< kege::InputLayer > InputLayer;
}
#endif /* kege_input_layer_hpp */

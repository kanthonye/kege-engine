//
//  app-layer-stack.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/22/25.
//

#ifndef kege_app_layer_stack_hpp
#define kege_app_layer_stack_hpp

#include <vector>
#include "app-layer.hpp"

namespace kege{

    class AppLayerStack : public kege::RefCounter
    {
    public:

        bool push( AppLayer* layer );
        bool initialize();
        void shutdown();
        void update();

        ~AppLayerStack();
        AppLayerStack();

    protected:

        std::vector< AppLayer* > _layers;
    };

}

namespace kege::ref{
    typedef kege::Ref< kege::AppLayerStack > AppLayerStack;
}
#endif // kege_app_layer_stack_hpp

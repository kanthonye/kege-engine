//
//  system.hpp
//  game
//
//  Created by Kenneth Esdaile on 3/14/25.
//

#ifndef kege_system_hpp
#define kege_system_hpp

#include "log.hpp"
#include "communication.hpp"
#include "settings.hpp"
#include "../../../src/core/math/algebra/vectors.hpp"
#include "../../../src/core/math/algebra/matrices.hpp"
#include "../../../src/core/math/algebra/quaternion.hpp"
#include "../../../src/core/math/algebra/transform.hpp"
#include "../../../src/core/graphics/core/graphics.hpp"
#include "../../../src/core/graphics/graph/render-graph.hpp"
#include "../../../src/core/input/input-context-manager.hpp"

namespace kege{

    class Engine;

    class System : public kege::RefCounter
    {
    public:

        enum StateBitFlag
        {
            REQUIRE_UPDATE = 1,
            REQUIRE_RENDER = 2,
            REQUIRE_INPUT  = 4,
        };
        
        bool checkFlag( StateBitFlag flag );

        virtual void input( const std::vector< Input >& inputs );
        virtual void update( double dms );
        virtual void render( double dms );

        virtual void onSceneChange();
        virtual bool initialize();
        virtual void shutdown();

        const std::string& getName() const;
        
        System( kege::Engine* engine, const std::string& name, uint32_t requirements );
        virtual ~System();

    protected:

        void setRenderGraph( kege::Ref< kege::RenderGraph > graphics );
        void setWindow( kege::Ref< kege::GraphicsWindow > window );
        void setGraphics( kege::Ref< kege::Graphics > graphics );

    protected:

        kege::Communication _comm;
        kege::Engine* _engine;
        uint32_t _requirements;
        std::string _name;

        friend kege::Engine;
    };

    struct MsgExecuteRenderProcess{};
}

#endif /* system_hpp */

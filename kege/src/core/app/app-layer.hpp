//
//  app-layer.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 10/22/25.
//

#ifndef app_layer_hpp
#define app_layer_hpp

#include <string>
#include "ref.hpp"
#include "log.hpp"

namespace kege{

    class AppLayer : public kege::RefCounter
    {
    public:

        const std::string& name() const { return _name; }

        virtual bool initialize() = 0;
        virtual void shutdown() = 0;
        virtual void update() = 0;

        AppLayer( const std::string& n ): _name( n ) {}
        virtual ~AppLayer() = default;

    protected:

        std::string _name;
    };

}

namespace kege::ref{
    using AppLayer = kege::Ref< kege::AppLayer >;
}
#endif /* app_layer_hpp */

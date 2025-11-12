//
//  input-context-loader.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/19/25.
//

#ifndef input_context_loader_hpp
#define input_context_loader_hpp

#include "../utils/hashmap.hpp"
#include "../utils/json-parser.hpp"
#include "../input/input-actions.hpp"
#include "../input/input-context.hpp"
#include "asset-manager.hpp"

namespace kege{

    struct InputContextLoader : public kege::AssetLoaderT< ref::InputContext >
    {
    public:

        ref::InputContext load( const std::string& filename );
        InputContextLoader( AssetManager* am = nullptr );
    };

    uint32_t stringToActionType( const std::string& name );
    uint32_t stringToAction( const std::string& name );
    uint32_t stringToKey( const std::string& name );

}
#endif /* input_context_loader_hpp */

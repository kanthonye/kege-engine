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

namespace kege{

    struct InputContextLoader
    {
    public:

        static kege::Ref< kege::InputContext > load( const kege::string& filename );
        static void populate();

    private:

        typedef std::unordered_map< std::string, uint32_t > StringHIntergerMap;

        static kege::Hashmap< uint32_t > keys;
        static kege::Hashmap< uint32_t > types;
        static kege::Hashmap< uint32_t > actions;
    };

}
#endif /* input_context_loader_hpp */

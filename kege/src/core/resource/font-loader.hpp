//
//  ui-font-loader.hpp
//  physics
//
//  Created by Kenneth Esdaile on 7/29/25.
//

#ifndef ui_font_loader_hpp
#define ui_font_loader_hpp

#include "asset-manager.hpp"
#include "../utils/communication.hpp"
#include "../graphics/components/font/font.hpp"

namespace kege{

    class FontLoader : public kege::AssetLoaderT< ref::Font >
    {
    public:

        ref::Font load( const std::string& filename );
        void operator()( kege::GraphicsDevice* response );

        FontLoader( AssetManager* am );

        kege::GraphicsDevice* _graphics;
    };
}

#endif /* ui_font_loader_hpp */

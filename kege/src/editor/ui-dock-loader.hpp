//
//  ui-dock-loader.hpp
//  editor
//
//  Created by Kenneth Esdaile on 1/17/26.
//

#ifndef ui_dock_loader_hpp
#define ui_dock_loader_hpp

#include "ui-dock.hpp"

namespace kege::ui{

    class DockManager : public kege::RefCounter
    {
    public:

        static bool save( const ui::Dock& dock, const std::string& filename );
        static kege::Ref< ui::Dock > load( const std::string& filename );
    };

}
#endif /* ui_dock_loader_hpp */

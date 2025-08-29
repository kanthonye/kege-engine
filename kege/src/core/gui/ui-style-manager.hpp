//
//  ui-style-manager.hpp
//  physics
//
//  Created by Kenneth Esdaile on 8/27/25.
//

#ifndef ui_style_manager_hpp
#define ui_style_manager_hpp

#include "ui-core.hpp"

namespace kege::ui{

    struct AddStyle
    {
        std::string name_id;
        ui::Style style;
    };

    class StyleManager
    {
    public:

        void set( const std::vector< std::pair< std::string, Style > >& styles );

        bool load( const std::string& filename );
        
        /**
         * Retrieves a style by its name identifier.
         *
         * @param name_id The name identifier of the style.
         *
         * @return A pointer to the style if found, nullptr otherwise.
         */
        ui::Style* getStyleByName( const std::string& name_id );

        /**
         * Retrieves a style by its index.
         *
         * @param index The index of the style.
         *
         * @return A pointer to the style if index is valid, nullptr otherwise.
         */
        ui::Style* getStyleByID( int index );

        /**
         * Adds a new style to the layout system.
         *
         * @param style The style to add.
         *
         * @return The index of the newly added style.
         */
        uint32_t addStyle( const AddStyle& style );

        /**
         * Resize total number of layout elements.
         *
         * @param max_quantity The maximum number of UI elements the system can manage.
         */
        void resize( uint32_t max_quantity );

        StyleManager();

    private:

        std::map< std::string, int > _style_index_map;
        std::vector< ui::Style > _styles;
        uint32_t _style_indexer;
    };

}

#endif /* ui_style_manager_hpp */

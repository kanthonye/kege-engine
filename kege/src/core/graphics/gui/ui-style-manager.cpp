//
//  ui-style-manager.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/27/25.
//

#include "ui-style-loader.hpp"
#include "ui-style-manager.hpp"

namespace kege::ui{

    void StyleManager::set( const std::vector< std::pair< std::string, Style > >& styles )
    {
        _styles.clear();
        _styles.resize( styles.size() + 1 );
        for (int i=0; i<styles.size(); ++i)
        {
            addStyle({ styles[i].first, styles[i].second });
        }
    }

    bool StyleManager::load( const std::string& filename )
    {
        std::vector< std::pair< std::string, Style > > styles;
        styles = StyleLoader::load( filename );
        if ( !styles.empty() )
        {
            set( styles );
            return true;
        }
        return false;
    }

    uint32_t StyleManager::addStyle( const AddStyle& as )
    {
        auto itr = _style_index_map.find( as.name_id );
        if ( itr == _style_index_map.end() )
        {
            _style_index_map[ as.name_id ] = _style_indexer;
            _styles[ _style_indexer ] = as.style;
            uint32_t index = _style_indexer++;
            return  index;
        }
        return itr->second;
    }

    ui::Style* StyleManager::getStyleByName( const std::string& name_id )
    {
        auto itr = _style_index_map.find( name_id );
        if ( itr == _style_index_map.end() )
        {
            return &_styles[ 0 ];
        }
        return &_styles[ itr->second ];
    }

    ui::Style* StyleManager::getStyleByID( int index )
    {
        return &_styles[ index ];
    }

    void StyleManager::resize(uint32_t max_quantity)
    {
        _styles.resize( max_quantity );
    }

    StyleManager::StyleManager()
    :   _style_indexer( 1 )
    {}

}

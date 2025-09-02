//
//  hierarchy-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "hierarchy-panel.hpp"
namespace kege{

    void HierarchyPanel::put( ui::Layout& layout )
    {
        layout.push( _main );
        build( _engine->scene()->root(), layout );
        layout.pop();
    }

    void HierarchyPanel::build( const Entity& entity, ui::Layout& layout, int spacer )
    {
        const EntityTag* tag = entity.get< EntityTag >();
        const char* entity_name = ( tag ) ? tag->c_str() : "untitled";
        const bool entity_has_children = entity.isParent();

        ui::HierarchyDroplist& list = _open_states[ entity.getID() ];
        if( list.open( layout, entity_has_children, spacer, entity_name ) )
        {
            list.beginContent( layout );
            for (Entity e = entity.begin(); e != 0 ; e = e.next() )
            {
                build( e, layout, spacer + 15 );
            }
            list.endContent( layout );
        }

        if( layout.click( list.field ) )
        {
            if ( _highlight != list.field )
            {
                if ( _highlight )
                {
                    _highlight->style = layout.getStyleByName( "droplist-field" );
                }
                list.field->style = layout.getStyleByName( "droplist-field-highlight" );
                _highlight = list.field;
            }
            _selected_entity = entity;
            Communication::broadcast< const SetSelectedEntity& >({ _selected_entity });
        }
    }

    HierarchyPanel& HierarchyPanel::init( Engine* engine, ui::Layout& layout )
    {
        _engine = engine;
        _main = layout.make
        ({
            .style = layout.getStyleByName( "hierarchy" )
        });
        return *this;
    }

    Entity HierarchyPanel::getSelectedEntity()
    {
        return _selected_entity;
    }

    HierarchyPanel::HierarchyPanel()
    :   _engine( nullptr )
    {}

}

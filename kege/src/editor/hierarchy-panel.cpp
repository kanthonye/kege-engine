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
        const char* text = ( tag ) ? tag->c_str() : "untitled";
        Droplist& list = _open_states[ entity.getID() ];

        if( list.open( layout, entity.isParent(), spacer, text ) )
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
            if ( int( _highlight ) != 0 && int( _highlight ) != int( list.field ) )
            {
                _highlight->style.background = ui::bgColor(0xFFFFFF0B);
            }
            _highlight = list.field;
            list.field->style.background = ui::bgColor(0xFFFFFF20);
            _selected_entity = entity;
        }
    }

    HierarchyPanel& HierarchyPanel::init( Engine* engine, ui::Layout& layout )
    {
        _engine = engine;
        _main = layout.make
        ({
            .style =
            {
                .background = ui::bgColor(0xFFFFFF13),
                .width = ui::extend(),
                .height = ui::extend(),
                .gap = {1,1},
                .align =
                {
                    .self = ui::AlignSelf::Relative,
                    .origin = ui::ALIGN_TOP_LEFT,
                    .direction = ui::DIRECTION_TOP_TO_BOTTOM,
                    .wrap_around = false
                }
            }
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

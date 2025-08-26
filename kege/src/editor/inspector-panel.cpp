//
//  inspector-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "inspector-panel.hpp"
namespace kege{

    vec3 _1value = {0,0,0};
    vec3 _2value = {0,0,0};
    vec3 _3value = {0,0,0};
    void InspectorPanel::put( ui::Layout& layout )
    {
        layout.push( _main );
        if ( droplist.begin( layout, true, 10, "Transform" ) )
        {
            droplist.beginContent( layout );
            transform[0].update( layout, _1value, "Position:" );
            transform[1].update( layout, _2value, "Rotation:" );
            transform[2].update( layout, _3value, "Scale:" );
            droplist.endContent( layout );
        }
        droplist.end( layout );
        layout.pop();
    }

    InspectorPanel& InspectorPanel::init( Engine* engine, ui::Layout& layout )
    {
        _engine = engine;
        _main = layout.make
        ({
            .style = layout.getStyleByName( "inspector" ),
        });
        return *this;
    }

    InspectorPanel::InspectorPanel()
    :   _engine( nullptr )
    {}

}

//
//  inspector-panel.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/18/25.
//

#include "inspector-panel.hpp"
namespace kege{

//    void transformComponentFunct( Entity& entity )
//    {
//        Transform* transform = entity.get< Transform >();
//
//    }
//
//    void processComponent( ui::Layout& layout, Entity& entity )
//    {
//        std::map< int, void(Entity&) > comps;
//        const EntityComponentMap& components = entity.getEntityComponents();
//        for (EntityComponentMap::const_iterator m = components.begin(); m != components.end(); m++)
//        {
//            int component_type = m->first;
//            int component_index = m->second;
//
//            comps[ component_type ]( entity );
//
//        }
//    }

    vec3 _1value = {0,0,0};
    vec3 _2value = {0,0,0};
    vec3 _3value = {0,0,0};
    void InspectorPanel::put( ui::Layout& layout )
    {
        layout.push( _main );

        transform[0].update( layout, _1value, "Position:");
        transform[1].update( layout, _2value, "Rotation:");
        transform[2].update( layout, _3value, "Scale:");

        layout.pop();
    }

    InspectorPanel& InspectorPanel::init( Engine* engine, ui::Layout& layout )
    {
        _engine = engine;
        _main = layout.make
        ({
            .style =
            {
                .background = ui::bgColor(0xFFFFFF13),
                .width = ui::extend(),
                .height = ui::extend(),
                .padding = {},
                .gap = {3,3},
                .align =
                {
                    .origin = ui::ALIGN_TOP_CENTER,
                    .direction = ui::DIRECTION_TOP_TO_BOTTOM,
                    .wrap_around = false
                },
            },
            //.text = "InspectorPanel",
        });
        return *this;
    }

    InspectorPanel::InspectorPanel()
    :   _engine( nullptr )
    {}

}

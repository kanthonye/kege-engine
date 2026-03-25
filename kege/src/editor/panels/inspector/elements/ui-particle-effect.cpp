//
//  ui-particle-effect.cpp
//  editor
//
//  Created by Kenneth Esdaile on 3/6/26.
//

#include "ui-particle-effect.hpp"

namespace kege::ui{

    bool particleEffect( kege::AssetManager* am, int16_t layer, kege::GUI* gui, kege::ECS* ecs, ecs::Entity& entity )
    {
        uint64_t component_id = ecs->getCompId< kege::ParticleEffect >( entity );

        ui::Text text
        {
            .ptr = "Particle Effect",
            .width = 64,
            .font_size = 20,
            .height = 15,
            .color = 0xFFFFFFFF
        };

        static std::vector<ListElem> list;
        if (list.empty())
        {
            list.resize(12);

            list[0].text.ptr = "Circular Area";
            list[0].text.width = 100;

            list[1].text.ptr = "Circular Line";
            list[1].text.width = 100;

            list[2].text.ptr = "Cone";
            list[2].text.width = 150;

            list[3].text.ptr = "Cube";
            list[3].text.width = 150;

            list[4].text.ptr = "Cylindrical Surface";
            list[4].text.width = 150;

            list[5].text.ptr = "Cylindrical Area";
            list[5].text.width = 150;

            list[6].text.ptr = "Line";
            list[6].text.width = 150;

            list[7].text.ptr = "Plane";
            list[7].text.width = 150;

            list[8].text.ptr = "Pyrmid";
            list[8].text.width = 150;

            list[9].text.ptr = "Spherical Surface";
            list[9].text.width = 150;

            list[10].text.ptr = "Spherical Area";
            list[10].text.width = 150;

            list[11].text.ptr = "Triangle";
            list[11].text.width = 150;
        }

        int selection;
        text.ptr = "Emitters:";
        gui->push({.layer = layer, .style = &gui->_theme.row});
        gui->put({.layer = layer, .text = text, .rect = {.width = 45, .height = 20} });
        gui->options(layer, component_id, list, selection);
        gui->pop();

        //float emissions_per_second, bool burst
        //max particle quantity
        //rate_of_deterioration
        gui->push({.layer = layer, .style = &gui->_theme.padded_list});
        gui->pop();

        return false;
    }
}

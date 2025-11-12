//
//  ui-tab.cpp
//  editor
//
//  Created by Kenneth Esdaile on 11/11/25.
//

#include "ui-tab.hpp"

namespace kege::ui{

    void Tabs::recreate( ui::Layout* layout, const std::vector< std::string >& tabs )
    {
        this->tab_elements.resize( tabs.size() );
        for (int i=0; i < tabs.size(); ++i)
        {
            this->tab_elements[i] = layout->make
            ({
                .mouseover = true,
                .trigger = ClickTrigger::OnClick,
                .style = layout->getStyleByName( "label" ),
                .text = {tabs[i].c_str(), 0, 0, 0, 0},
            });
        }
        if ( !this->container )
        {
            this->container = layout->make
            ({
                .mouseover = false,
                .style = layout->getStyleByName( "h-bar" )
            });
        }
    }

    bool Tabs::update( ui::Layout* layout, const std::vector< std::string >& tabs )
    {
        /**
         if the tabs and tab elements are of different size then recreate the tabs elements
         */
        if ( this->tab_elements.size() != tabs.size() )
        {
            recreate( layout,tabs );
        }

        bool active = false;
        if ( !this->tab_elements.empty() )
        {
            // push the elements to the layout so that the elements can be updated and rendered
            layout->push( this->container );
            for (int i=0; i < this->tab_elements.size(); ++i)
            {
                layout->put( this->tab_elements[i] );
            }
            layout->pop();

            // select the clicked tab element
            for (int i=0; i < tabs.size(); ++i)
            {
                if( layout->click( this->tab_elements[i] ) )
                {
                    this->selection = i;
                    active = true;
                    break;
                }
            }
        }
        return active;
    }
    
    Tabs::Tabs(): selection(0) {}
}

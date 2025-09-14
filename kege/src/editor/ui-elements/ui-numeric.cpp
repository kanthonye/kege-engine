//
//  ui-numeric.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/25/25.
//

#include "ui-numeric.hpp"

namespace kege::ui{

    ui::EID makeRow( ui::Layout& layout )
    {
        return layout.make
        ({
            .mouseover = false,
            .style = layout.getStyleByName( "row" ),
        });
    }

    ui::EID makeCol( ui::Layout& layout )
    {
        return layout.make
        ({
            .mouseover = false,
            .style = layout.getStyleByName( "column" ),
        });
    }

    ui::EID makeLabel( ui::Layout& layout )
    {
        return layout.make
        ({
            .mouseover = false,
            .style = layout.getStyleByName( "label" ),
        });
    }



    void setupDroplist( ui::Layout& layout, TreeNode& tree, const char* text )
    {
        tree.elements.push_back
        ({
            layout.make
            ({
                .mouseover = false,
                .style = layout.getStyleByName( "properties-container" ),
            })
        });
        tree.elements.push_back
        ({
            layout.make
            ({
                .trigger = ui::ClickTrigger::OnRelease,
                .style = layout.getStyleByName( "properties-field" ),
            })
        });
        tree.elements.push_back
        ({
            layout.make
            ({
                .mouseover = false,
                .style = layout.getStyleByName( "properties-label" ),
                .text = text,
            })
        });
        tree.elements.push_back
        ({
            layout.make
            ({
                .mouseover = false,
                .style = layout.getStyleByName( "properties-content" ),
            })
        });
    }

    bool droplistOpen( ui::Layout& layout, TreeNode& tree )
    {
        if ( layout.click( tree.elements[1] ) )
        {
            if ( !tree.state[0] )
            {
                tree.state[0] = true;
                tree.state[1] = !tree.state[1];
            }
        }
        else if( tree.state[0] && layout.input()->buttonDown() )
        {
            tree.state[0] = false;
        }
        return tree.state[1];
    }

    bool numeric( ui::Layout& layout, TreeNode& tree, float& num, const char* text )
    {
        if ( tree.elements.empty() )
        {
            tree.state[0] = false;
            tree.cursor = 0;

            char snum[16];
            snprintf(snum, 16, "%.3f", num );

            tree.elements.reserve(3);
            tree.elements.push_back //  main container
            ({
                layout.make
                ({
                    .trigger = ui::ClickTrigger::OnClick,
                    .style = layout.getStyleByName( "numeric" ),
                })
            });
            tree.elements.push_back // label
            ({
                layout.make
                ({
                    .mouseover = false,
                    .style = layout.getStyleByName( "numeric-label" ),
                    .text = text
                })
            });
            tree.elements.push_back // value displayer
            ({
                layout.make
                ({
                    .mouseover = false,
                    .style = layout.getStyleByName( "numeric-value" ),
                    .text = snum
                })
            });
        }

        layout.push( tree.elements[0] );
        {
            layout.put( tree.elements[1] );
            layout.put( tree.elements[2] );
        }
        layout.pop();

        bool active = false;
        if ( layout.input()->buttonDown() )
        {
            if ( layout.click( tree.elements[0] ) )
            {
                num += (layout.input()->deltaPosition().x - layout.input()->deltaPosition().y) * 0.0125;
                char snum[16];
                snprintf(snum, 16, "%.3f", num );
                tree.elements[2]->text.text = snum;
                active = true;
            }
            else if ( layout.doubleClick( tree.elements[0] ) )
            {
                tree.state[0] = true;
            }
            else if ( tree.state[0] )
            {
                tree.state[0] = false;
                tree.elements[0]->style = layout.getStyleByName( "numeric" );
            }
        }

        if ( tree.state[0] )
        {
            layout.input()->onTextInput
            (
                ui::Input::INPUT_NUMERIC,
                &tree.elements[2]->text.text,
                &tree.cursor,
                &tree.state[0]
            );
            tree.elements[0]->style = layout.getStyleByName( "numeric-focus" );

            if ( !tree.state[0] )
            {
                tree.elements[0]->style = layout.getStyleByName( "numeric" );
                num = atof( tree.elements[2]->text.text.c_str() );
            }
        }

        return tree.state[0] || active;
    }

    bool numeric2( ui::Layout& layout, TreeNode& tree, float& x, float& y, const char* text )
    {
        if ( tree.elements.empty() )
        {
            tree.elements.reserve(5);
            setupDroplist( layout, tree, text );
            tree.elements.push_back({ makeRow( layout ) });
            tree.children.resize(3);
        }

        bool active = false;
        layout.push( tree.elements[0] );
        {
            layout.push( tree.elements[1] );
            {
                layout.put( tree.elements[2] );
            }
            layout.pop();

            if ( droplistOpen( layout, tree ) )
            {
                layout.push( tree.elements[3] );
                {
                    layout.push( tree.elements[4] );
                    {
                        if( numeric( layout, tree.children[0], x, "x:" ) ) active = true;
                        if( numeric( layout, tree.children[1], y, "y:" ) ) active = true;
                    }
                    layout.pop();
                }
                layout.pop();
            }
        }
        layout.pop();
        return active || tree.state[1];
    }

    bool numeric3( ui::Layout& layout, TreeNode& tree, float& x, float& y, float& z, const char* text )
    {
        if ( tree.elements.empty() )
        {
            tree.elements.reserve(5);
            setupDroplist( layout, tree, text );
            tree.elements.push_back({ makeRow( layout ) });
            tree.children.resize(3);
        }

        bool active = false;
        layout.push( tree.elements[0] );
        {
            layout.push( tree.elements[1] );
            {
                layout.put( tree.elements[2] );
            }
            layout.pop();

            if ( droplistOpen( layout, tree ) )
            {
                layout.push( tree.elements[3] );
                {
                    layout.push( tree.elements[4] );
                    {
                        if( numeric( layout, tree.children[0], x, "x:" ) ) active = true;
                        if( numeric( layout, tree.children[1], y, "y:" ) ) active = true;
                        if( numeric( layout, tree.children[2], z, "z:" ) ) active = true;
                    }
                    layout.pop();
                }
                layout.pop();
            }
        }
        layout.pop();
        return active || tree.state[1];
    }

    bool numeric4( ui::Layout& layout, TreeNode& tree, float& x, float& y, float& z, float& w, const char* text )
    {
        if ( tree.elements.empty() )
        {
            tree.elements.reserve(5);
            setupDroplist( layout, tree, text );
            tree.elements.push_back({ makeRow( layout ) });
            tree.children.resize(4);
        }

        bool active = false;
        layout.push( tree.elements[0] );
        {
            layout.push( tree.elements[1] );
            {
                layout.put( tree.elements[2] );
            }
            layout.pop();

            if ( droplistOpen( layout, tree ) )
            {
                layout.push( tree.elements[3] );
                {
                    layout.push( tree.elements[4] );
                    {
                        if( numeric( layout, tree.children[0], x, "x:" ) ) active = true;
                        if( numeric( layout, tree.children[1], y, "y:" ) ) active = true;
                        if( numeric( layout, tree.children[2], z, "z:" ) ) active = true;
                        if( numeric( layout, tree.children[3], w, "w:" ) ) active = true;
                    }
                    layout.pop();
                }
                layout.pop();
            }
        }
        layout.pop();
        return active || tree.state[1];
    }




    bool numericD( ui::Layout& layout, TreeNode& tree, double& num, const char* text )
    {
        if ( tree.elements.empty() )
        {
            tree.state[0] = false;
            tree.cursor = 0;

            char snum[16];
            snprintf(snum, 16, "%.3f", num );

            tree.elements.reserve(3);
            tree.elements.push_back //  main container
            ({
                layout.make
                ({
                    .trigger = ui::ClickTrigger::OnClick,
                    .style = layout.getStyleByName( "numeric" ),
                })
            });
            tree.elements.push_back // label
            ({
                layout.make
                ({
                    .mouseover = false,
                    .style = layout.getStyleByName( "numeric-label" ),
                    .text = text
                })
            });
            tree.elements.push_back // value displayer
            ({
                layout.make
                ({
                    .mouseover = false,
                    .style = layout.getStyleByName( "numeric-value" ),
                    .text = snum
                })
            });
        }

        layout.push( tree.elements[0] );
        {
            layout.put( tree.elements[1] );
            layout.put( tree.elements[2] );
        }
        layout.pop();

        bool active = false;
        if ( layout.input()->buttonDown() )
        {
            if ( layout.click( tree.elements[0] ) )
            {
                num += (layout.input()->deltaPosition().x - layout.input()->deltaPosition().y) * 0.0125;
                char snum[16];
                snprintf(snum, 16, "%.3f", num );
                tree.elements[2]->text.text = snum;
                active = true;
            }
            else if ( layout.doubleClick( tree.elements[0] ) )
            {
                tree.state[0] = true;
            }
            else if ( tree.state[0] )
            {
                tree.state[0] = false;
                tree.elements[0]->style = layout.getStyleByName( "numeric" );
            }
        }

        if ( tree.state[0] )
        {
            layout.input()->onTextInput
            (
                ui::Input::INPUT_NUMERIC,
                &tree.elements[2]->text.text,
                &tree.cursor,
                &tree.state[0]
            );
            tree.elements[0]->style = layout.getStyleByName( "numeric-focus" );

            if ( !tree.state[0] )
            {
                tree.elements[0]->style = layout.getStyleByName( "numeric" );
                num = atof( tree.elements[2]->text.text.c_str() );
            }
        }

        return tree.state[0] || active;
    }

}

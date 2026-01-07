//
//  gui.cpp
//  editor
//
//  Created by Kenneth Esdaile on 12/8/25.
//

#include "gui.hpp"

namespace kege{

    bool GUI::textField( uint16_t layer, ui::UID (&uid)[2], int& mode, char* str, size_t& size )
    {
        _layout->push
        ({
            .layer = layer,
            .uid = &uid[0],
            .style = ( mode == 2 ) ? getStyle( Theme::NumericFocus ) : getStyle( Theme::NumericValue ),
            .single_click = ui::ClickTrigger::OnRelease,
            .double_click = ui::ClickTrigger::Immediate,
        });
        _layout->put
        ({
            .layer = layer,
            .uid = &uid[1],
            .style = getStyle( Theme::NumericLabel ),
            .text.ptr = str,
            .text.length = size,
        });
        _layout->pop( layer );

        bool clicked = false;

        if ( _layout->click( uid[0] ) )
        {
            clicked = true;
            mode += 1;
        }
        else if ( _layout->doubleClick( uid[0] ) )
        {
            mode = 2;
        }
        else if ( !_layout->hasFocus( uid[0] ) && mode > 0 )
        {
            mode = 0;
        }

        if ( mode == 2 )
        {
            if ( !_layout->onTextInput( uid[1], str, size ) )
            {
                mode = 0;
            }
        }

        return clicked;
    }
    
    bool GUI::scrubber( uint16_t layer, ui::UID (&uid)[2], int& mode, float& num, char* str, size_t& size )
    {
        char snum[16];
        snprintf(snum, 16, "%.6g", num );

        _layout->push
        ({
            .layer = layer,
            .uid = &uid[0],
            .style = ( mode == 2 ) ? getStyle( Theme::NumericFocus ) : getStyle( Theme::NumericValue ),
            .single_click = ui::ClickTrigger::Continuous,
            .double_click = ui::ClickTrigger::Immediate,
        });
        _layout->put({ .uid = &uid[1], .style = getStyle( Theme::NumericLabel ), .text = snum });
        _layout->pop( layer );

        bool active = false;
        if ( _layout->buttonDown() )
        {
            if ( _layout->click( uid[0] ) )
            {
                if ( _layout->input()->pointerDragging() )
                {
                    if( 0.0 != _layout->input()->deltaPosition().x )
                        num += _layout->input()->deltaPosition().x;

                    if( 0.0 != _layout->input()->deltaPosition().y )
                        num -= _layout->input()->deltaPosition().y;
                    active = true;
                }
            }
            else if ( _layout->doubleClick( uid[0] ) )
            {
                mode = 2;
            }
            else
            {
                mode = 0;
            }
        }

        if ( mode == 2 )
        {
            if ( !_layout->onTextInput( uid[1], str, size ) )
            {
                active = true;
                mode = 0;
            }
            num = atof( str );
        }

        return active;
    }

    bool GUI::numeric( uint16_t layer, ui::UID (&uid)[3], int& mode, float& num, char* str, size_t& size )
    {
        snprintf(str, size, "%.6g", num );

        _layout->push(kege::ui::WidgetDesc{
            .uid = &uid[0],
            .layer = layer,
            .style = getStyle( Theme::Numeric ),
        });
        {
            _layout->put
            ({
                .layer = layer,
                .uid = &uid[2],
                .style = ( mode == 2 )? getStyle( Theme::NumericFocus ) : getStyle( Theme::NumericValue ),
                .single_click = ui::ClickTrigger::OnRelease,
                .double_click = ui::ClickTrigger::Immediate,
                .text.ptr = str
            });
        }
        _layout->pop( layer );

        bool clicked = false;

        if ( _layout->click( uid[2] ) )
        {
            clicked = true;
            mode += 1;
        }
        else if ( _layout->doubleClick( uid[2] ) )
        {
            mode = 2;
        }
        else if ( !_layout->hasFocus( uid[2] ) && mode > 0 )
        {
            mode = 0;
        }

        if ( mode == 2 )
        {
            if ( !_layout->onTextInput( uid[2], str, size ) )
            {
                mode = 0;
            }
            num = atof( str );
        }

        return clicked;
    }

    bool  GUI::select( uint16_t layer, ui::Style* style, std::vector<std::pair< ui::UID, ui::Text >>& list, int& selection )
    {
        bool has_selection = false;
        _layout->push
        ({
            .layer = layer,
            .style = style
        });
        for (int i=0; i<list.size(); ++i)
        {
            _layout->put
            ({
                .layer = layer,
                .uid = &list[i].first,
                .text = list[i].second,
                .single_click = ui::ClickTrigger::OnRelease,
                .style = (selection == i) ? getStyle( Theme::ListSelection ) : getStyle( Theme::ListElem )
            });
            if( _layout->click( list[i].first ) )
            {
                selection = i;
                has_selection = true;
                //break;
            }
        }
        _layout->pop( layer );
        return has_selection;
    }
    
    bool GUI::select( uint16_t layer, std::vector<std::pair< ui::UID, ui::Text >>& list, int& selection )
    {
        return select( layer, getStyle( Theme::List ), list, selection);
    }

    void GUI::list( uint16_t layer, std::vector<std::pair<ui::UID,std::string>>& list, int selection )
    {
        _layout->push({ .layer = layer, .style = getStyle( Theme::List ) });
        for (int i=0; i<list.size(); ++i)
        {
            _layout->put
            ({
                .layer = layer,
                .uid = &list[i].first,
                .text = list[i].second.c_str(),
                .style = getStyle( Theme::ListElem )
            });
        }
        _layout->pop( layer );
    }

    bool GUI::tab( uint16_t layer, std::vector<std::pair< ui::UID, std::string >>& list, int& selection )
    {
        bool active = false;
        _layout->push({ .layer = layer, .style = getStyle( Theme::Tab ) });
        for (int i=0; i<list.size(); ++i)
        {
            _layout->put({
                .layer = layer,
                .uid = &list[i].first,
                .text = list[i].second.c_str(),
                .single_click = ui::ClickTrigger::OnRelease,
                .style = (selection == i)
                    ? getStyle( Theme::TabSelection )
                    : getStyle( Theme::TabElem )
            });
            if( _layout->click( list[i].first ) )
            {
                selection = i;
                active = true;
            }
        }
        _layout->pop( layer );
        return active;
    }

    void slidebarOp(ui::Layout* layout, const ui::UID& id, void* data)
    {
        //layout->elemParent(id)->uid
        RangeParams* params = reinterpret_cast<RangeParams*>(data);
        float length = layout->elem(id)->rect.width;
        float& w = layout->elem(*params->id)->rect.width;

        // calculate where the slider should be (base on the numeric value) before moving it.
        w = (length * (*params->val - params->min)) / (params->max - params->min);

        if( layout->click( id ) )
        {
            // update slide bar width
            w += layout->input()->deltaPosition().x;
            // clamp slide bar width
            w = (w < 0)? 0.f : (w > length) ? length: w;
            // calculate numeric value base on slide bar width
            *params->val = (params->max - params->min) * (w / length) + params->min;
        }
        else if( layout->doubleClick( id ) )
        {
            // update slide bar width
            w = layout->input()->getClick(0).position.x - layout->elem(id)->rect.x;
            // clamp slide bar width
            w = (w < 0)? 0.f : (w > length) ? length: w;
            // calculate numeric value base on slide bar width
            *params->val = (params->max - params->min) * (w / length) + params->min;
        }
    }

    bool GUI::numSlideBar( uint16_t layer, ui::UID (&id)[2], float* val, float min, float max )
    {
        char snum[16];
        snprintf(snum, 16, "%.6g", *val );

        _layout->push
        ({
            .layer = layer,
            .uid = &id[1],
            .style = getStyle( Theme::SlideBarTrack ),
            .single_click = ui::ClickTrigger::Continuous,
            .double_click = ui::ClickTrigger::Immediate
        });
        _layout->put({ .layer = layer, .uid = &id[0], .style = getStyle( Theme::SlideBar ) });
        _layout->put({ .layer = layer, .style = getStyle( Theme::SlideBarNumber ), .text = snum });
        _layout->pop( layer );

        _layout->pushDeferredOp(id[1], slidebarOp, RangeParams{min,max,val,&id[0]});

        return _layout->click( id[1] );
    }

    bool GUI::slidebar( uint16_t layer, ui::UID (&id)[2], float* val, float min, float max )
    {
        _layout->push({ .layer = layer, .uid = &id[1], .style = getStyle( Theme::SliderTrack ), .single_click = ui::ClickTrigger::Continuous });
        _layout->put({ .layer = layer, .uid = &id[0], .style = getStyle( Theme::SlideBar ) });
        _layout->pop( layer );

        _layout->pushDeferredOp(id[1], slidebarOp, RangeParams{min,max,val,&id[0]});

        return _layout->click( id[1] );
    }

    void sliderOp(ui::Layout* layout, const ui::UID& id, void* data)
    {
        RangeParams* params = reinterpret_cast<RangeParams*>(data);
        float length = layout->elemParent(id)->rect.width - layout->elem(id)->rect.width;
        float& x = layout->elem(id)->rect.x;

        // calculate where the slider should be (base on the numeric value) before moving it.
        x = (length * (*params->val - params->min)) / (params->max - params->min);

        if( layout->click( id ) )
        {
            // update slider position
            x += layout->input()->deltaPosition().x;
            // keep the slider with in bound
            x = (x < 0)? 0.f : (x > length) ? length: x;
            // calculate the new numeric value base on the updated position
            *params->val = (params->max - params->min) * (x / length) + params->min;
        }
        layout->elem(id)->rect.x += x;
    }

    bool GUI::slider( uint16_t layer, ui::UID (&id)[2], float* val, float min, float max )
    {
        _layout->push({ .layer = layer, .uid = &id[1], .style = getStyle( Theme::SliderTrack ) });
        _layout->put({ .layer = layer, .uid = &id[0], .style = getStyle( Theme::Slider ), .single_click = ui::ClickTrigger::Continuous });
        _layout->pop( layer );

        _layout->pushDeferredOp(id[0], sliderOp, RangeParams{min,max,val});

        return _layout->click( id[0] );
    }

    bool GUI::button( uint16_t layer, ui::UID& id, ui::Style* style, const ui::Text& text )
    {
        return button
        ({
            .layer = layer,
            .uid = &id,
            .text = text,
            .style = style,
            .single_click = ui::ClickTrigger::OnRelease,
        });
    }
    bool GUI::button( const kege::ui::WidgetDesc& desc )
    {
        _layout->put( desc );
        return _layout->click( *desc.uid );
    }

    bool GUI::button( uint16_t layer, ui::UID& id, const ui::Text& text )
    {
        return button( layer, id, getStyle( Theme::Button ), text );
    }

    void GUI::label( uint16_t layer, const char* text )
    {
        _layout->put
        ({
            .layer = layer,
            .text = text,
            .style = getStyle( Theme::Label ),
            .enabled = false
        });
    }

    void GUI::pushHPanel( uint16_t layer )
    {
        _layout->push
        ({
            .layer = layer,
            .style = getStyle( Theme::HPanel )
        });
    }

    void GUI::pushVPanel( uint16_t layer )
    {
        _layout->push({ .layer = layer, .style = getStyle( Theme::VPanel ) });
    }

    void GUI::push( const kege::ui::WidgetDesc& desc )
    {
        _layout->push( desc );
    }

    void GUI::put( const kege::ui::WidgetDesc& desc )
    {
        _layout->put( desc );
    }

    void GUI::pop( uint16_t layer )
    {
        _layout->pop( layer );
    }

    const bool GUI::pointerDragging() const
    {
        return _layout->input()->pointerDragging();
    }
    
    bool GUI::click( ui::UID& id )
    {
        return _layout->click( id );
    }
    
    bool GUI::hot( ui::UID& id )
    {
        return _layout->mouseover( id );
    }

    bool GUI::buttonDown()const
    {
        return _layout->buttonDown();
    }

    ui::Widget* GUI::get( ui::UID& id )
    {
        return _layout->elem( id );
    }

    kege::ui::Style* GUI::getStyle( int index )
    {
        return &_theme.styles[index];
    }

    bool GUI::initialize(Ref< ui::Layout > layout)
    {
        _layout = layout;
        return true;
    }

    void GUI::createLayers( uint32_t quantity )
    {
        _layout->createLayers( quantity );
    }

    vec2d GUI::deltaPointer()const
    {
        return _layout->input()->deltaPosition();
    }

    vec2d GUI::pointer()const
    {
        return _layout->input()->currentPosition();
    }

    GUI::~GUI()
    {}

    GUI::GUI()
    {
    }


    Theme::Theme()
    {
        styles[Label] = kege::ui::Style{
            .background = 0xFFFFFF00,
            .align_text = ui::AlignText::Left,
            .padding = {10,0,0,0},
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(18),
            .width = ui::extend(),
            .font_size = 20,
        };

        styles[Button] = kege::ui::Style{
            .height = ui::fixed(20),
            .width = ui::extend(),
            .background = 0xFFFFFF20,
            .align_text =  ui::AlignText::Center,
        };
        styles[CloseButn] = kege::ui::Style{
            .height = ui::fixed(18),
            .width = ui::fixed(18),
            .background = 0xFFFFFF20,
            .align_text =  ui::AlignText::Center,
            .font_size = 20,
            .border.corner_curves = {2,2,2,2},
        };

        styles[Slider] = kege::ui::Style{
            .height = ui::fixed(14),
            .width = ui::fixed(14),
            .background = 0xFF22FFFF,
            .align_text =  ui::AlignText::Center,
        };
        styles[SliderTrack] = kege::ui::Style{
            .height = ui::fixed(14),
            .width = ui::extend(),
            .background = 0xFFFFFF20,
            .align_text = ui::AlignText::Center,
        };


        styles[SlideBar] = kege::ui::Style{
            .background = 0xFF22FFFF,
            .position = ui::Positioning::Absolute,
            .height = ui::fixed(14),
            .width = ui::fixed(14),
        };
        styles[SlideBarTrack] = kege::ui::Style{
            .background = 0xFFFFFF20,
            .height = ui::fixed(14),
            .width = ui::extend(),
            .align =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::CENTER},
                .direction = ui::AlignDir::HORIZONTAL
            }
        };
        styles[SlideBarNumber] = kege::ui::Style{
            .position = ui::Positioning::Absolute,
            .background = 0xFFFFFF00,
            .align_text = ui::AlignText::Center,
            .height = ui::fixed(14),
            .width = ui::percent(100),
            .font_size = 20,
        };


        styles[List] = kege::ui::Style{
            .background = ui::Background(0x232323FF),
            .height = ui::flexible(),
            .width = ui::extend(),
            .gap = {2,2},
            .align =
            {
                .content = {ui::AlignPosX::CENTER, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::VERTICAL
            }
        };
        styles[ListElem] = kege::ui::Style{
            .height = ui::fixed(20),
            .width = ui::extend(),
            .background = 0xFFFFFF10,
            .align_text = ui::AlignText::Center,
            .border.corner_curves = {4,4,4,4},
            .font_size = 20,
        };
        styles[ListSelection] = kege::ui::Style{
            .height = ui::fixed(20),
            .width = ui::extend(),
            .background = 0xFFFFFF30,
            .align_text =  ui::AlignText::Center,
            .font_size = 20,
        };


        styles[Tab] = kege::ui::Style{
            .background = ui::Background(0x10101000),
            .height = ui::fixed(20),
            .width = ui::extend(),
            .align =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::HORIZONTAL
            }
        };
        styles[TabElem] = kege::ui::Style{
            .height = ui::fixed(20),
            .width = ui::fixed(120),
            .background = 0x171420FF,
            .align_text = ui::AlignText::Center,
            .text_color = 0xBBA0FFFF,
            .font_size = 20,
        };
        styles[TabSelection] = kege::ui::Style{
            .height = ui::fixed(20),
            .width = ui::fixed(120),
            .background = ui::Background(0x171420FF),
            .align_text = ui::AlignText::Center,
            .border.corner_curves = {4,4,4,4},
            .font_size = 20,
        };


        styles[FlexH] = kege::ui::Style{
            .height = ui::flexible(),
            .width = ui::extend(),
            .background = ui::Background(0x333333FF),
            .align =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::VERTICAL
            }
        };
        styles[FlexW] = kege::ui::Style{
            .height = ui::extend(),
            .width = ui::flexible(),
            .background = ui::Background(0x333333FF),
            .align =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::HORIZONTAL
            }
        };


        styles[HPanel] = kege::ui::Style{
            .height = ui::extend(),
            .width = ui::extend(),
            .background = ui::Background(0x333333FF),
            .align =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::HORIZONTAL
            }
        };
        styles[VPanel] = kege::ui::Style{
            .height = ui::extend(),
            .width = ui::extend(),
            .background = ui::Background(0x334433FF),
            //.padding = {10,10,10,10},
            .align =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::VERTICAL,
                .wrap_around = true,
            }
        };


        styles[Hierarchy] = kege::ui::Style{
            .background = ui::Background(0x433333FF),
            .height = ui::extend(),
            .width = ui::fixed(400),
            //.padding = {10,10,10,10},
            .align =
            {
                .content = {ui::AlignPosX::CENTER, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::VERTICAL
            }
        };
        styles[Viewport] = kege::ui::Style{
            .height = ui::extend(),
            .width = ui::extend(),
            .background = ui::Background(0x222222FF),
            //.padding = {10,10,10,10},
            .align =
            {
                .content = {ui::AlignPosX::RIGHT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::HORIZONTAL
            }
        };



        styles[Numeric] = kege::ui::Style{
            .background = ui::Background(0xFFFFFF0B),
            .height = ui::fixed(18),
            .width = ui::extend(),
            .padding = {8,2,8,2},
            .gap = {2,2},
            .align =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP},
                .direction = ui::AlignDir::HORIZONTAL
            }
        };
        styles[NumericLabel] = kege::ui::Style{
            .background = ui::Background(0xFFFFFF00),
            .align_text = ui::AlignText::Right,
            .text_color = 0xFFFFFFFF,
            .height = ui::fixed(18),
            .width = ui::fixed(100),
            .font_size = 20,
            .align =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::CENTER},
                .direction = ui::AlignDir::HORIZONTAL
            }
        };
        styles[NumericValue] = kege::ui::Style{
            .background = ui::Background(0xFFFFFF00),
            .align_text = ui::AlignText::Left,
            .text_color = 0xFFFFFFFF,
            .border.corner_curves = {5,5,5,5},
            .height = ui::fixed(18),
            .width = ui::extend(),
            .font_size = 20,
            .align =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::CENTER},
                .direction = ui::AlignDir::HORIZONTAL
            }
        };
        styles[NumericFocus] = kege::ui::Style{
            .background = ui::Background(0xFFFFFF1B),
            .align_text = ui::AlignText::Left,
            .text_color = 0xFFFFFFFF,
            .border.corner_curves = {5,5,5,5},
            .height = ui::fixed(18),
            .width = ui::extend(),
            .font_size = 20,
            .align =
            {
                .content = {ui::AlignPosX::LEFT, ui::AlignPosY::CENTER},
                .direction = ui::AlignDir::HORIZONTAL
            }
        };
    }
}

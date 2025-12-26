//
//  gui.hpp
//  editor
//
//  Created by Kenneth Esdaile on 12/8/25.
//

#ifndef gui_hpp
#define gui_hpp

#include "../core/graphics/gui/ui-input.hpp"
#include "../core/graphics/gui/ui-layout.hpp"
#include "../core/graphics/gui/ui-viewer.hpp"

namespace kege{

    struct RangeParams
    {
        float  min;
        float  max;
        float* val;
        kege::UID* id;
    };

    struct Theme
    {
        enum
        {
            Label,
            Button,
            Slider, SliderTrack,
            SlideBar, SlideBarTrack, SlideBarNumber,

            List, ListElem, ListSelection,
            Tab, TabElem, TabSelection,

            VPanel, HPanel,
            Numeric, NumericLabel, NumericFocus, NumericValue,


            Hierarchy, Viewport,
            DockNorizonal, DockVertical,

            FlexH, FlexW,
            CloseButn,
            COUNT,
        };

        Theme();

        kege::ui::Style styles[Theme::COUNT];
    };

    class GUI
    {
    public:

        bool textField( kege::UID (&uid)[2], int& mode, kege::string& text );

        bool scrubber( kege::UID (&uid)[2], int& mode, float& num );
        bool numeric( kege::UID (&uid)[3], int& mode, float& num );
        bool select( ui::Style* style, std::vector<std::pair< kege::UID, std::string >>& list, int& selection );
        bool select( std::vector<std::pair< kege::UID, std::string >>& list, int& selection );
        void list( std::vector<std::pair< kege::UID, std::string >>& list, int selection = -1 );
        bool tab( std::vector<std::pair< kege::UID, std::string >>& list, int& selection );

        bool numSlideBar( kege::UID (&id)[2], float* val, float min, float max );
        bool slidebar( kege::UID (&id)[2], float* val, float min, float max );
        bool slider( kege::UID (&id)[2], float* val, float min, float max );

        bool button( kege::UID& id, ui::Style* style, const char* text );
        bool button( kege::UID& id, const char* text );
        bool button( const kege::ui::Desc& desc );
        void label( const char* text );

        void pushHPanel();
        void pushVPanel();

        void push( const kege::ui::Desc& desc );
        void put( const kege::ui::Desc& desc );
        void pop();

        bool click( kege::UID& id );
        bool hot( kege::UID& id );
        bool buttonDown()const;

        ui::Widget* get( kege::UID& id );
        kege::ui::Style* getStyle( int index );
        bool initialize(Ref< ui::Layout > layout);

        vec2d deltaPointer()const;
        vec2d pointer()const;

        ~GUI();
        GUI();

        kege::Ref< kege::ui::Layout > _layout;
        kege::Theme _theme;
        //kege::ui::Viewer _viewer;
        //kege::ui::Input _input;
    };

}

#endif /* gui_hpp */

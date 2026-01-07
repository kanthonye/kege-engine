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
        ui::UID* id;
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

        template<typename Params>void pushDeferredOp(const ui::UID& id, ui::DeferredOp::Fn fn, const Params& params)
        {
            _layout->pushDeferredOp< Params >( id, fn, params );
        }

        bool textField( uint16_t layer, ui::UID (&uid)[2], int& mode, char* str, size_t& size );

        bool scrubber( uint16_t layer, ui::UID (&uid)[2], int& mode, float& num, char* str, size_t& size );
        bool numeric( uint16_t layer, ui::UID (&uid)[3], int& mode, float& num, char* str, size_t& size );
        bool select( uint16_t layer, ui::Style* style, std::vector<std::pair< ui::UID, ui::Text >>& list, int& selection );
        bool select( uint16_t layer, std::vector<std::pair< ui::UID, ui::Text >>& list, int& selection );
        void list( uint16_t layer, std::vector<std::pair< ui::UID, std::string >>& list, int selection = -1 );
        bool tab( uint16_t layer, std::vector<std::pair< ui::UID, std::string >>& list, int& selection );

        bool numSlideBar( uint16_t layer, ui::UID (&id)[2], float* val, float min, float max );
        bool slidebar( uint16_t layer, ui::UID (&id)[2], float* val, float min, float max );
        bool slider( uint16_t layer, ui::UID (&id)[2], float* val, float min, float max );

        bool button( uint16_t layer, ui::UID& id, ui::Style* style, const ui::Text& text );
        bool button( uint16_t layer, ui::UID& id, const ui::Text& text );
        bool button( const kege::ui::WidgetDesc& desc );
        void label( uint16_t layer, const char* text );

        ui::Layout* layout()
        {
            return _layout.ref();
        }

        void pushHPanel( uint16_t layer );
        void pushVPanel( uint16_t layer );

        void push( const kege::ui::WidgetDesc& desc );
        void put( const kege::ui::WidgetDesc& desc );
        void pop( uint16_t layer );

        /**
         * Checks if the mouse pointer is being dragged.
         *
         * @return true if the pointer is being dragged, false otherwise.
         */
        const bool pointerDragging() const;
        bool click( ui::UID& id );
        bool hot( ui::UID& id );
        bool buttonDown()const;
        bool dragging()const;


        /**
         * Checks if geven point and rectangular shape intersects.
         *
         * @param p The given point to test the rectangle against.
         * @param rect The rectangle to test the point against.
         *
         * @return true if mouse is over ui element, false otherwise.
         */
        inline bool testPointVsRect( const kege::dvec2& p, const ui::Rect& rect )const
        {
            return _layout->testPointVsRect( p, rect );
        }

        ui::Widget* get( ui::UID& id );
        kege::ui::Style* getStyle( int index );
        bool initialize(Ref< ui::Layout > layout);


        void createLayers( uint32_t quantity );
        
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

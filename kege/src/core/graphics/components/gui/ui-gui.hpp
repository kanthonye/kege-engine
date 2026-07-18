//
//  ui-gui.hpp
//  editor
//
//  Created by Kenneth Esdaile on 5/12/26.
//

#ifndef ui_gui_hpp
#define ui_gui_hpp

#include "ui-layout.hpp"
#include "ui-renderer.hpp"
#include "ui-post-layout-ops.hpp"

namespace kege{

    struct GuiConfig
    {
        std::map< uint32_t, kege::mat44> layout_transforms;
        std::map< uint32_t, kege::ui::Rect> layout_rects;

        // Capacity
        uint32_t max_layers_per_layout = 8;
        uint32_t max_widgets = 10000;
        uint32_t max_layouts = 1;

        // Render resources
        kege::Extent2D target_extent; // Screen size
        uint32_t font_size = 16;

        std::string font_path;
        std::string icon_theme_path;
        std::string pipeline_filename;

        // Performance
        bool enable_batching = true;
        uint32_t vertex_buffer_size_mb = 4;

        // Editor specific (ignored by Runtime)
        bool enable_debug_overlays = false;
        bool enable_imgui_style_panels = false;
    };

    class GUI : public kege::RefCounter
    {
    public:

        template<typename Params> void pushDeferredOp
        (
            kege::ui::ID user_id,
            kege::ui::WidgetId widget_index,
            kege::ui::DeferredOperation fn,
            const Params& params
        )
        {
            _deferred_operations.push< Params >(user_id, widget_index, fn, params);
        }

        template<typename Params> void pushDeferredOpPtr
        (
            kege::ui::ID user_id,
            kege::ui::WidgetId widget_index,
            kege::ui::DeferredOperation fn,
            Params* params
        )
        {
            _deferred_operations.pushPtr< Params >(user_id, widget_index, fn, params);
        }


        uint32_t computeExtent( int font_size, const char* text, float& width, float& height );

        kege::ui::Text text( const char* str, int font_size );

        void onWindowResize(int width, int height);

        /**
         * Creates a parent UI element with the give info.
         *
         * @param desc This refers to the ui element id
         *
         * @return reference to the ui element.
         */
        kege::ui::WidgetId pushRoot( const kege::ui::WidgetDesc& desc );
        kege::ui::WidgetId putRoot( const kege::ui::WidgetDesc& desc );

        /**
         * Pops the current parent UI element from the parent stack.
         */
        void popRoot();

        /**
         * Creates a parent UI element with the give info.
         *
         * @param desc This refers to the ui element id
         *
         * @return reference to the ui element.
         */
        kege::ui::WidgetId push( const kege::ui::WidgetDesc& desc );

        /**
         * Pops the current parent UI element from the parent stack.
         *
         * @return The index of the popped UI element in the elements array.
         */
        uint32_t pop();

        /**
         * Creates a UI element with the give description.
         *
         * @param desc Description of the ui element
         *
         * @return reference to the ui element.
         */
        kege::ui::WidgetId put( const kege::ui::WidgetDesc& desc );
        kege::ui::WidgetId text( const kege::ui::Text& text );

        /**
         * Push an existing layer onto the layer stack, so that, that layer can
         * be modified by added widget to that layer.
         *
         * @param index The index of the layer to push onto the stack
         */
        void pushLayer( uint32_t index );

        /**
         * Pop an existing layer off the layer stack, so that, and restore the previous
         * layer back to the top of the stack.
         */
        bool popLayer();

        kege::dvec2 getClickPosition( kege::MouseButtonCode code ) const;

        /**
         * Retrieves the current position of the mouse pointer.
         *
         * @return The current position as a 2D vector.
         */
        kege::dvec2 getPointerPosition() const;

        /**
         * Retrieves the delta (change) in the mouse pointer's position.
         *
         * @return The delta position as a 2D vector.
         */
        kege::dvec2 getPointerDelta() const;

        /**
         * Retrieves the mouse scroll offset.
         *
         * @return The scroll offset as a 2D vector.
         */
        kege::dvec2 getScrollOffset() const;

        /**
         * Checks if the mouse pointer is being dragged.
         *
         * @return true if the pointer is being dragged, false otherwise.
         */
        bool isPointerDragging() const;

        /**
         * Checks if mouse pointer is over ui element.
         *
         * @param uid The ui element layout.
         *
         * @return true if mouse is over ui element, false otherwise.
         */
        bool mouseover( const kege::ui::ID& uid ) const;

        /**
         * Checks if a ui-element that is associated with geven id was double clicked on.
         *
         * @param uid The given id.
         *
         * @return true if the element was double clicked on, false otherwise.
         */
        bool doubleClick( const kege::ui::ID& uid ) const;

        /**
         * Checks if a ui-element that is associated with geven id was single clicked on.
         *
         * @param uid The given id.
         *
         * @return true if the ui-element was single clicked on, false otherwise.
         */
        bool click( const kege::ui::ID& uid ) const;

        /**
         * Checks if a ui-element that is associated with geven id has focus.
         *
         * @param uid The given id.
         *
         * @return true if the ui-element has focus, false otherwise.
         */
        bool hasFocus( const kege::ui::ID& uid )const;

        /**
         * Retrieves a UI element by its index (const version).
         *
         * @param uid The ui element index.
         *
         * @return The UI element at the specified index.
         */
        const kege::ui::Widget* elem( const kege::ui::WidgetId& uid ) const;

        /**
         * Retrieves a UI element by its index (non-const version).
         *
         * @param uid The ui element index.
         *
         * @return The UI element at the specified index.
         */
        kege::ui::Widget* elem( const kege::ui::WidgetId& uid );
        kege::ui::Widget* elemParent( const kege::ui::WidgetId& uid );

        /**
         * Retrieves a UI element by its index (const version).
         *
         * @param index The ui element index.
         *
         * @return The UI element at the specified index.
         */
        const kege::ui::Widget* at(uint32_t index) const;

        /**
         * Retrieves a UI element by its index (non-const version).
         *
         * @param index The ui element index.
         *
         * @return The UI element at the specified index.
         */
        kege::ui::Widget* at(uint32_t index);

        /**
         * Retrieves the parent index of a UI element.
         */
        uint32_t parent( uint32_t index )const;

        /**
         * Retrieves the head index of a UI element.
         */
        uint32_t head( uint32_t index )const;

        /**
         * Retrieves the tail index of a UI element.
         */
        uint32_t tail( uint32_t index )const;

        /**
         * Retrieves the next sibling index of a UI element.
         */
        uint32_t next( uint32_t index )const;

        /**
         * Retrieves the prev sibling index of a UI element.
         */
        uint32_t prev( uint32_t index )const;

        /**
         * Retrieves the number of children of a UI element.
         */
        uint32_t count( uint32_t index )const;

        /**
         * Sets the current font for rendering text.
         *
         * @param font The font to set.
         */
        void setFont(const kege::ref::Font& font);

        /**
         * Retrieves the current font.
         *
         * @return The current font.
         */
        const kege::ref::Font& getFont() const;

        /**
         * Resize total number of layout elements.
         *
         * @param max_elements The maximum number of UI elements the system can manage.
         */
        //void resize( uint32_t max_elements );


        bool leftClickDown()const;


        /**
         * Retrieves the input handler associated with the layout.
         *
         * @return A pointer to the input handler.
         */
        const kege::InputManager* getInputManager()const;
        kege::GraphicsDevice* getGraphicsDevice();
        kege::AssetManager* getAssetManager();

        const kege::ui::Cursor& getCursor()const;
        kege::ui::Cursor* cursor();
        double getTimeDelta()const;

        const kege::ui::Layout& getLayout( uint32_t i )const;
        kege::ui::Layout& getLayout( uint32_t i );
        uint32_t totalLayouts()const;

        const kege::ui::Rect& getRect()const;

        bool hasHit()const;

        bool initialize( const GuiConfig& config );

        void shutdown();

        /**
         * Begins the UI layout construction. Must be called before creating any UI elements.
         */
        void begin( double dms );

        /**
         * Ends the UI layout construction. Must be called after all UI elements are created.
         */
        void end();

        const kege::ui::DrawRecord* getDrawRecord( uint32_t frame_index );

        const kege::mat44& getProjection()const;

        ~GUI();
        
        GUI
        (
            kege::GraphicsDevice* device,
            kege::AppWindow* window,
            kege::InputManager* input_manager,
            kege::AssetManager* asset_manager
        );

    private:

        kege::ui::Record getHotElem(bool button);
        uint32_t find(uint64_t user_id);
        void handleInputEvents();

        void renderLayout
        (
            kege::ui::DrawBatch* batch,
            const kege::ui::Layout& layout,
            uint32_t first_instance
        );

        void renderLayer
        (
            kege::ui::DrawBatch* batch,
            const kege::ui::Layout& layout,
            const kege::ui::Layer& layer,
            uint32_t first_instance
        );

        void renderWidget
        (
            kege::ui::DrawBatch* batch,
            const kege::ui::Layout& layout,
            const kege::ui::Widget* widget,
            kege::ui::Rect clip_rect
        );

        ui::Extent renderText
        (
            kege::ui::DrawBatch* batch,
            const kege::vec2& start,
            float width,
            float font_size,
            uint32_t color,
            bool wrap_around,
            const char* text,
            const kege::ui::Rect& clip_rect
        );

        bool indexIsValid( uint32_t index ) const;

        kege::ui::Widget* newWidget();

    private:

        kege::ui::PostLayoutOpsExecutor _deferred_operations;

        kege::array< kege::ui::Layout > _layouts;
        kege::array< kege::ui::Widget > _widgets;

        const kege::InputManager* _input_manager;
        kege::AssetManager* _asset_manager;
        kege::GraphicsDevice* _graphic_device;
        const kege::AppWindow* _window;
        const kege::Mouse* _mouse;


        kege::array< kege::ref::Buffer > _mesh_storage_buffers;
        kege::array< kege::ui::DrawRecord > _draw_records;
        kege::ui::DrawInstance* _draw_buffer;

        kege::ui::Cursor _cursor;

        kege::ref::Font _font;

        kege::ui::Rect _rect;

        kege::mat44 _proj_matrix;
        kege::vec2 _window_scale_factor;
        
        uint32_t _widget_count;
        uint32_t _layout_index;
        double _dms;

        mutable kege::ui::HitRecord _curr;
        mutable kege::ui::HitRecord _next;

        bool _click_registered;
        bool _left_click_down;
        bool _left_click_state;
        bool _butn_down;

        friend kege::ui::Layout;
        friend kege::ui::Layer;
    };

}

namespace kege::ref{
    typedef kege::Ref< kege::GUI > GUI;
}
#endif /* ui_gui_hpp */

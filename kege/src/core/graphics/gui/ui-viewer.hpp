//
//  ui-viewer.hpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#ifndef ui_viewer_hpp
#define ui_viewer_hpp

#include "../font/font.hpp"
#include "ui-layout.hpp"
#include "../mesh/mesh.hpp"
#include "../render/graph/render-queue.hpp"
#include "../render/pipeline/shader-pipeline.hpp"
#include "../../io/virtual-directory.hpp"
#include "../../resource/asset-manager.hpp"
#include "../../utils/frames.hpp"

namespace kege::ui{

    /**
     * UIDrawInstance hold the data required to draw a single ui element
     */
    struct UIDrawInstance
    {
        /**
         * rect: hold the position and size of the ui element
         */
        ui::Rect rect;  // 16 bytes

        /**
         * texel: hold the uv coordinates for the texture
         */
        ui::Rect texel;  // 16 bytes

        /**
         * corner_curves: hold the radius for each corner of the rectangle
         */
        ui::Border border;  // 16 bytes

        /**
         * clip_rect: hold the clipping rectangle for this ui element
         */
        ui::Rect clip_rect;  // 16 bytes

        struct // 16 bytes
        {
            /**
             * color is a hex color value holding the rgba value for each color channel.
             note the value is unpacked on the GPU to make a vec4
             */
            uint32_t color;

            /**
             * texr_info hold the texture information for this ui element
             */
            TexrInfo texr_info;

            float pad[2];
        };
    };


    class Viewer : public kege::RefCounter
    {
    public:

        void onWindowFrameBufferResize(int width, int height);
        void onWindowResize(int width, int height);

        UIDrawInstance& nextInstance();

        void push( const ui::UIDrawInstance& instance );

        ui::Extent drawText
        (
            const kege::vec2& start,
            float width,
            float font_size,
            uint32_t color,
            bool wrap_around,
            const char* text,
            const ui::Rect& clip_rect
        );

        void drawRect
        (
            /**
             rect: hold the position and size of the ui element
             */
            const ui::Rect& rect,

            /**
             color is a hex color value holding the rgba value for each color channel.
             note the value is unpacked on the GPU to make a vec4
             */
            const ui::Color& color,

            /**
             corner_curves: hold the radius for each corner of the rectangle
             */
            const ui::Border& border,

            /**
             texr_info hold the texture information for this ui element
             */
            const TexrInfo& texr_info,

            /**
             texel: hold the uv coordinates for the texture
             */
            const ui::Rect& texel,

            /**
             clip_rect: hold the clipping rectangle for this ui element
             */
            const ui::Rect& clip_rect
        );

        void draw( const ui::Widget& content, const ui::Rect& clip_rect );

        /**
         */
        void draw( ui::Layout& layout, int pid, ui::Rect clip_rect );

        //void linearize( ui::Layout& layout, int pid, int zindex, std::vector< std::pair< int, ui::Widget* > >& nodes, int& count );
        //void insertionSort(std::vector< std::pair< int, ui::Widget* > >& arr);

        //void drawsort( ui::Layout& layout, int pid );

        void render( ui::Layout& layout );

        /**
         */
        void begin();
        void end(RenderExecutor* manager);

        /**
         * Sets the current font for rendering text.
         *
         * @param font The font to set.
         */
        void setFont(const ref::Font& font);

        /**
         * Retrieves the current font.
         *
         * @return The current font.
         */
        const ref::Font& getFont() const;

        /**
         * Initializes the Core object.
         *
         * @return true if initialization is successful, false otherwise.
         */
        bool initialize( kege::Graphics* graphics, kege::AssetManager* asset_manager, ref::Font font );

        ref::Image getDefaultTexture();

        void setViewportImage( const kege::ImageBindInfo& info, int frame );
        void setThemeImage( const kege::ImageBindInfo& info, int frame );
        void setFontImage( const kege::ImageBindInfo& info, int frame );

        /**
         * Uninitializes the Core object, releasing any allocated resources.
         */
        void shutdown();

        void flush();
        
        ~Viewer();
        Viewer();

    private:

        kege::BufferBindInfo createBuffer();

        std::vector< ui::UIDrawInstance > _drawbuffer;
        ref::Font _font; // The current font used for rendering text.

        ref::ShaderPipeline _shader_pipeline;

        kege::AssetManager* _asset_manager;
        kege::Graphics* _graphics;

        PushConstantBlock _push_constant;
        ui::Extent _fbo_size;
        ui::Extent _window_size;

        kege::ref::ShaderData _shader_data;
        kege::ref::Mesh _meshs[2];

        kege::BufferBindings _buffer_bindings[2];
        ui::Rect _clip_rect;

        uint64_t _pipeline;

        uint32_t _curr_buffer_index;
        uint32_t _draw_count;
    };

}

#endif /* ui_viewer_hpp */

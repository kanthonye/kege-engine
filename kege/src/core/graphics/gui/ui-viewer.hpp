//
//  ui-viewer.hpp
//  gui
//
//  Created by Kenneth Esdaile on 8/5/25.
//

#ifndef ui_viewer_hpp
#define ui_viewer_hpp

#include "font.hpp"
#include "ui-layout.hpp"
#include "../mesh/mesh.hpp"
#include "../render/graph/render-queue.hpp"
#include "../render/pipeline/shader-pipeline.hpp"
#include "../../io/virtual-directory.hpp"

namespace kege::ui{

    class Viewer : public kege::RefCounter
    {
    public:

        kege::vec2 drawText
        (
            const kege::vec2& start,
            float width,
            float font_size,
            const ui::Color& color,
            bool wrap_around,
            const char* text,
            const ui::Rect& clip_rect
        );

        void draw( const ui::Widget& content, const ui::Rect& clip_rect );

        /**
         */
        void draw( ui::Layout& layout, int pid, const ui::Rect& clip_rect );

        void linearize( ui::Layout& layout, int pid, int zindex, std::vector< std::pair< int, ui::Widget* > >& nodes, int& count );
        void insertionSort(std::vector< std::pair< int, ui::Widget* > >& arr);

        void drawsort( ui::Layout& layout, int pid );

        void collectVisibleWidgets( RenderExecutor* manager, ui::Layout& layout );

        /**
         */
        void begin();
        void end();

        /**
         * Sets the current font for rendering text.
         *
         * @param font The font to set.
         */
        void setFont(const kege::Font& font);

        /**
         * Retrieves the current font.
         *
         * @return The current font.
         */
        const kege::Font& getFont() const;

        /**
         * Initializes the Core object.
         *
         * @return true if initialization is successful, false otherwise.
         */
        bool initialize( Graphics* graphics );

        ref::Image getDefaultTexture();

//        void setViewportImage( const ImageInfo& info );
//        void setThemeImage( const ImageInfo& info );
//        void setFontImage( const ImageInfo& info );

        /**
         * Uninitializes the Core object, releasing any allocated resources.
         */
        void shutdown();

        void flush();
        
        ~Viewer();
        Viewer();

    private:

        //ImageInfo _scene_image_info;

        kege::Ref< MaterialSource > createMaterial();
        kege::Ref< MeshSource > createMesh();

        ref::ShaderData _shader_data;

        std::vector< kege::ui::DrawElem > _drawbuffer;
        kege::Font _font; // The current font used for rendering text.

        const int _max_render_instances;

        //BufferHandle   _indirect_draw_buffer[ kege::MAX_FRAMES_IN_FLIGHT ];
        //ShaderResource _storage_buffer_resource[ kege::MAX_FRAMES_IN_FLIGHT ];
        //ShaderResource _ui_texture_shader_resource;
        //ShaderResource _font_shader_resource;
        ref::Image _default_texture;
        ref::ShaderPipeline _pipeline;

        kege::CommandEncoder* _encoder;
        Graphics* _graphics;

        PushConstantBlock _push_constant;
        Extent2D _fbo_size;

        std::vector< kege::Ref< MeshSource > > _meshes[2];
        Ref< kege::MaterialSource > _material;
        uint32_t _curr_mesh_index;
        uint32_t _draw_count;
    };

}

#endif /* ui_viewer_hpp */

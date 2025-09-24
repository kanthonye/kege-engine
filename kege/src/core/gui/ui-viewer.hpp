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
#include "../graphics/mesh/mesh.hpp"

namespace kege::ui{

    class UIMeshSource : public kege::MeshSource
    {
    public:
        
        UIMeshSource( kege::Graphics* graphics, int _max_render_instances )
        {
            size_t size = _max_render_instances * sizeof( kege::ui::DrawElem );

            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
            // create and setup the ui instance buffer shader resources
            // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

            instance_buffer_list = new kege::InstanceBufferList(nullptr, {});
            kege::InstanceBuffer instance = kege::InstanceBuffer
            {
                .buffer = graphics->createBuffer
                ({
                    .size = size,
                    .usage = kege::BufferUsage::StorageBuffer,
                    .memory_usage = kege::MemoryUsage::CpuToGpu,
                    .data = nullptr
                }),
                .shader_resource = graphics->allocateUniformSet(kege::UniformSetDesc{
                    kege::UniformDesc
                    {
                        .descriptor_type = kege::DescriptorType::StorageBuffer,
                        .stage_flags = kege::ShaderStage::Vertex,
                        .name = "UIViewBuffer",
                        .binding = 0,
                        .count = 1,
                    }
                })
            };
            instance.shader_resource[0][0] = kege::UniformBinding
            {
                .binding = 0,
                .uniform = kege::BufferBindings
                {{
                    .buffer = instance.buffer,
                    .offset = 0,
                    .range = size
                }}
            };
            instance.shader_resource.update({});

            instance_buffer_list = new kege::InstanceBufferList( graphics, { instance });

            first_instance = 0;
            first_index = 0;
            index_count = 4;
            instance_count = 0;
            material_index = 0;
        }
    };


    class Viewer
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

        void collectVisibleWidgets( RenderManager* manager, ui::Layout& layout );

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
        bool initialize( Graphics* graphics, kege::ShaderPipeline pipeline, kege::Font font, ImageInfo& scene_image_info );

        kege::ImageHandle getDefaultTexture();

        /**
         * Uninitializes the Core object, releasing any allocated resources.
         */
        void shutdown();

        void flush();
        
        ~Viewer();
        Viewer();

    private:

        ImageInfo _scene_image_info;

        kege::Ref< MaterialSource > createMaterial();
        kege::Ref< MeshSource > createMesh();

        std::vector< kege::ui::DrawElem > _drawbuffer;
        kege::Font _font; // The current font used for rendering text.

        const int _max_render_instances;

        //BufferHandle   _indirect_draw_buffer[ kege::MAX_FRAMES_IN_FLIGHT ];
        //ShaderResource _storage_buffer_resource[ kege::MAX_FRAMES_IN_FLIGHT ];
        //ShaderResource _ui_texture_shader_resource;
        //ShaderResource _font_shader_resource;
        ImageHandle _default_texture;
        kege::ShaderPipeline _pipeline;

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

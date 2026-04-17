//
//  main.cpp
//  ui
//
//  Created by Kenneth Esdaile on 3/5/25.
//

#include "../core/app/entry-point.hpp"
#include "editor-layer.hpp"

namespace kege{

    class TestEditor : public kege::Application
    {
    public:

        bool initialize()
        {
            if ( !kege::Application::initialize() )
            {
                return false;
            }
            kege::ref::AppLayer ui_app_layer = new EditorLayer
            (
                _asset_manager.ref(),
                _render_graph.ref(),
                _project_manager.ref(),
                _input_manager.ref(),
                _ecs.ref()
            );
            _running = _app_layer_stack->push( ui_app_layer );


            ShaderCompiler compiler(_graphics.ref(), (vfs("snippets") + "/").c_str());
            ref::ShaderPipeline pipeline = compiler.compileVariant
            ({
                .shading_model = kege::ShadingModel::LitPBR_Metallic,
                .render_pass = kege::RenderPass::Forward,
                .renderer_type = kege::MeshType::StaticMesh,
                .topology = kege::PrimitiveTopology::TriangleStrip,
                .features = (
                    kege::FeatureFlag::POINT_LIGHT | kege::FeatureFlag::DIRECTIONAL_LIGHT |
                    kege::FeatureFlag::NORMAL_MAPPING |
                    kege::FeatureFlag::MATERIAL
                ),
                .sample_count = 1,
                .blend_state = kege::BlendStatePreset::alphaBlend(),
                .depth_state = kege::DepthStencilPresets::depthTestWrite(),
                .raster_state = kege::RasterizerStatePresets::cullBack()
            });

            _running = false;
            return _running;
        }
    };
    
    kege::Ref< Application > createApplication()
    {
        return new TestEditor;
    }
}



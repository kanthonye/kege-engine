//
//  ui-create-material.cpp
//  editor
//
//  Created by Kenneth Esdaile on 4/4/26.
//

#include "shader-compiler.hpp"
#include "ui-create-pipeline.hpp"
#include "../ui-asset-manager.hpp"
#include "../../../common/ui-schema.hpp"
#include "../../../dock/ui-dock-manager.hpp"
#include "../../../editor-layer.hpp"

namespace kege::ui{

    CreatePipeline::CreatePipeline(AssetManagerUI* m,kege::UI* g)
    : AssetManagerModule(m,g), _created_counter(0)
    ,  _pipeline_features_ui(g)
    , _shader_model_ui(g)
    , _mesh_type_ui(g)
    , _depth_stencil_ui(g)
    , _render_passes(g)
    , _primitive_topology_ui(g)
    , _rasterizer_state_ui(g)
    , _blend_state_ui(g)
    {}

    void CreatePipeline::operator()(const std::string& type, void* data)
    {
        if (type == "OpenCreateShaderWindow")
        {
            snprintf(_asset_name, 31, "pipeline-%i", _created_counter++);
            _asset_label = _ui->layout()->text(_asset_name, 20);
            _name.ptr = _asset_name;

            _open_window = true;
            _window_rect.width = 600;
            _window_rect.height = 400;
            _window_rect.x = (_ui->layout()->getRect().width - _window_rect.width) * 0.5;
            _window_rect.y = (_ui->layout()->getRect().height - _window_rect.height) * 0.5;
        }
    }

    void CreatePipeline::update()
    {
        if (!_open_window) return;

        ui::ID id[3] = {_uid[0], _uid[1], _uid[2]};
        _ui->beginWindow(id, _window_rect, "Create Pipeline", _open_window);
        {
            _ui->push({.style = &_ui->theme().card2});
            {
                if( _ui->labelInput("Name:", _uid[3], _textmode, _name) )
                {
                    size_t size = kege::min<size_t>(31, _ui->getCharBufrLen());
                    strncpy(_asset_name, _ui->getCharBufr(), size);
                    _asset_name[size] = 0;
                }

                if( _ui->submit(_uid[4], "Submit") )
                {
                    ref::AssetManager asset_manager = _manager->getManager()->getEditor()->getAssetManager();
                    kege::Graphics* graphics = _manager->getManager()->getEditor()->getRenderGraph()->getGraphics();

                    ShaderCompiler compiler(graphics);
                    kege::Ref< kege::ShaderPipeline > pipeline = compiler.compileVariant(_pipeline_key);
                    uint64_t handle = asset_manager->add< kege::Ref< kege::ShaderPipeline > >(_asset_name, pipeline);

                    // Create metadata
                    AssetMetadata metadata;
                    metadata.handle         = handle;
                    metadata.name           = _asset_name;
                    metadata.display_name   = metadata.name;
                    metadata.original_path  = "";
                    metadata.type           = AssetType::SHADER;
                    metadata.category       = "Shader";
                    metadata.loader_id      = ".glsl";
                    metadata.import_time    = std::chrono::system_clock::now();
                    metadata.last_modified  = std::filesystem::file_time_type::min();
                    metadata.file_size      = 0;
                    metadata.missing_source = false;
                    metadata.loaded         = true;

                    _manager->addAsset(metadata);
                    _asset_name[0] = 0;
                    _pipeline_key = {};
                    
                    _open_window = false;
                }
            }
            _ui->pop();

            ui::ID scroller[3] = {_uid[10], _uid[11], _uid[12]};
            _ui->beginScrollContainer(scroller);
            {
                _ui->push({.style = &_ui->theme().row});
                {
                    _ui->push({.style = &_ui->theme().column});
                    {
                        _render_passes.update(_ui, _pipeline_key);
                        _shader_model_ui.update(_ui, _pipeline_key);
                    }
                    _ui->pop();

                    _ui->push({.style = &_ui->theme().column});
                    {
                        _mesh_type_ui.update(_ui, _pipeline_key);
                        _primitive_topology_ui.update(_ui, _pipeline_key);
                    }
                    _ui->pop();
                }
                _ui->pop();

                _rasterizer_state_ui.update(_ui, _pipeline_key);
                _blend_state_ui.update(_ui, _pipeline_key);
                _pipeline_features_ui.update(_ui, _pipeline_key);
                _depth_stencil_ui.update(_ui, _pipeline_key);
            }
            _ui->endScrollContainer();
        }
        _ui->endWindow();
    }
    
    CreatePipeline::~CreatePipeline()
    {
        //_create_mesh_uis.clear();
    }
}


//
//  ui-create-material.hpp
//  editor
//
//  Created by Kenneth Esdaile on 4/4/26.
//

#ifndef ui_create_pipeline_hpp
#define ui_create_pipeline_hpp

#include "asset-manager-module.hpp"
#include "../mesh/create-mesh-ui.hpp"

namespace kege::ui{

    class ShadingModels
    {
    public:

        void operator()(kege::UI* ui, const char* name, kege::ShadingModel model)
        {
            _shader_models.push_back(model);
            _list.push_back(ui->gui()->text(name, 20));
        }

        void update(kege::UI* ui, kege::PipelineKey& key)
        {
            if( ui->labelOptions(_uid, _title, _list, _selection))
            {
                key.shading_model = _shader_models[_selection];
            }
        }

        ShadingModels(kege::UI* ui)
        {
            _selection = 0;
            _title = kege::ui::Text{
                .x = 0, .y = 0,
                .width = 90,
                .font_size = 20,
                .color = 0xFFFFFF50,
                .data = "Shading Model:",
            };

            operator()(ui, "Unlit", kege::ShadingModel::Unlit);
            operator()(ui, "Lit PBR Metallic", kege::ShadingModel::LitPBR_Metallic);
            operator()(ui, "Lit PBR Clearcoat", kege::ShadingModel::LitPBR_Clearcoat);
            operator()(ui, "Lit PBR Subsurface", kege::ShadingModel::LitPBR_Subsurface);
            operator()(ui, "Lit PBR Cloth", kege::ShadingModel::LitPBR_Cloth);
            operator()(ui, "Lit PBR Hair", kege::ShadingModel::LitPBR_Hair);
            operator()(ui, "Lit PBR Skin", kege::ShadingModel::LitPBR_Skin);
            operator()(ui, "Toon Cel", kege::ShadingModel::Toon_Cel);
            operator()(ui, "Toon UnlitOutline", kege::ShadingModel::Toon_UnlitOutline);
            operator()(ui, "Emissive", kege::ShadingModel::Emissive);
            operator()(ui, "Skybox", kege::ShadingModel::Skybox);
            operator()(ui, "Depth Only", kege::ShadingModel::DepthOnly);
        }
        std::vector< kege::ShadingModel > _shader_models;
        kege::ui::Text _title;
        std::vector< ui::Text > _list;
        kege::ui::UID _uid;
        int _selection;
    };

    class RenderPasses
    {
    public:
    
        void operator()(kege::UI* ui, const char* name, kege::RenderPassType model)
        {
            _render_passes.push_back(model);
            _list.push_back( ui->gui()->text(name, 20) );
        }

        void update(kege::UI* ui, kege::PipelineKey& key)
        {
            if( ui->labelOptions(_uid, _title, _list, _selection))
            {
                key.render_pass= _render_passes[_selection];
            }
        }

        RenderPasses(kege::UI* ui)
        {
            _selection = 0;
            _title = kege::ui::Text{
                .x = 0, .y = 0, .width = 90, .font_size = 20,
                .color = 0xFFFFFF50, .data = "Render Passes:", 
            };
            operator()(ui, "DepthPrePass", kege::RenderPassType::DepthPrePass);
            operator()(ui, "Geometry", kege::RenderPassType::Geometry);
            operator()(ui, "Lighting", kege::RenderPassType::Lighting);
            operator()(ui, "Shadow", kege::RenderPassType::Shadow);
            operator()(ui, "Forward", kege::RenderPassType::Forward);
            operator()(ui, "Sky", kege::RenderPassType::Sky);
            operator()(ui, "VolumetricFog", kege::RenderPassType::VolumetricFog);
            operator()(ui, "Particles", kege::RenderPassType::Particles);
            operator()(ui, "PostProcess", kege::RenderPassType::PostProcess);
            operator()(ui, "UI", kege::RenderPassType::UI);
            operator()(ui, "Debug", kege::RenderPassType::Debug);
        }
        std::vector< kege::RenderPassType > _render_passes;
        std::vector< ui::Text > _list;
        kege::ui::Text _title;
        kege::ui::UID _uid;
        int _selection;
    };

    class DepthStencilState
    {
    public:
    
        void operator()(kege::UI* ui, const char* name, kege::ComparisonFunc model)
        {
            _comparison_functs.push_back(model);
            _list.push_back(ui->gui()->text(name, 20));
        }

        void update(kege::UI* ui, kege::PipelineKey& key)
        {
            ui->push({ .wid = ui->newElem({ ui->theme()->card }) });
            if( ui->collapsableHeader(_uid[0][0], _expand, _depth_stencil_label) )
            {
                ui->push({ .wid = ui->newElem(ui->theme()->card) });
                {
                    ui->push({ .wid = ui->newElem(ui->theme()->card) });
                    ui->checkbox( _uid[0][1], _depth_enable_label, key.depth_state.depth.enable);
                    ui->checkbox( _uid[0][2], _depth_write_enable_label, key.depth_state.depth.write);
                    ui->pop();
                    if( ui->labelOptions(_uid[1], _comparison_functs_label, _list, _selection))
                    {
                        key.depth_state.depth.compare_op = _comparison_functs[_selection];
                    }
                }
                ui->pop();
            }
            ui->pop();
        }

        DepthStencilState(kege::UI* ui)
        {
            _expand = false;
            _selection = 0;
            _depth_stencil_label = { .data = "Depth Stencil:", .color = 0xFFFFFF50, .x = 0, .y = 0, .width = 200, .font_size = 20 };
            _depth_enable_label = { .data = "Depth Enable:", .color = 0xFFFFFF50, .x = 0, .y = 0, .width = 150, .font_size = 20 };
            _depth_write_enable_label = { .data = "Depth Write Enable:", .color = 0xFFFFFF50, .x = 0, .y = 0, .width = 150, .font_size = 20 };
            _comparison_functs_label = { .data = "Comparison Fn:", .color = 0xFFFFFF50, .x = 0, .y = 0, .width = 80, .font_size = 20 };
            operator()(ui, "Never", kege::ComparisonFunc::Never);
            operator()(ui, "Less", kege::ComparisonFunc::Less);
            operator()(ui, "Equal", kege::ComparisonFunc::Equal);
            operator()(ui, "LessEqual", kege::ComparisonFunc::LessEqual);
            operator()(ui, "Greater", kege::ComparisonFunc::Greater);
            operator()(ui, "NotEqual", kege::ComparisonFunc::NotEqual);
            operator()(ui, "GreaterEqual", kege::ComparisonFunc::GreaterEqual);
            operator()(ui, "Always", kege::ComparisonFunc::Always);
        }
        
    private:

        std::vector< kege::ComparisonFunc > _comparison_functs;
        std::vector< ui::Text > _list;
        kege::ui::Text _depth_stencil_label;
        kege::ui::Text _depth_enable_label;
        kege::ui::Text _depth_write_enable_label;
        kege::ui::Text _comparison_functs_label;
        kege::ui::UID _uid[2];

        int _selection;
        bool _expand;
    };

    class MeshTypes
    {
    public:
    
        void operator()(kege::UI* ui, const char* name, kege::MeshType model)
        {
            _mesh_types.push_back(model);
            _list.push_back(ui->gui()->text(name, 20));
        }

        void update(kege::UI* ui, kege::PipelineKey& key)
        {
            if( ui->labelOptions(_uid, _title, _list, _selection))
            {
                key.renderer_type= _mesh_types[_selection];
            }
        }

        MeshTypes(kege::UI* ui)
        {
            _selection = 0;
            _list.reserve(11);
            _title = kege::ui::Text{
                .x = 0, .y = 0,
                .width = 70,
                .font_size = 20,
                .color = 0xFFFFFF50,
                .data = "Mesh Type:",
            };
            operator()(ui, "Point", kege::MeshType::Point);
            operator()(ui, "ScreenRect", kege::MeshType::ScreenRect);
            operator()(ui, "StaticMesh", kege::MeshType::StaticMesh);
            operator()(ui, "Particle", kege::MeshType::Particle);
            operator()(ui, "Terrain", kege::MeshType::Terrain);
            operator()(ui, "Atmosphere", kege::MeshType::Atmosphere);
            operator()(ui, "Ocean", kege::MeshType::Ocean);
            operator()(ui, "Foliage", kege::MeshType::Foliage);
            operator()(ui, "UI", kege::MeshType::UI);
            operator()(ui, "FlatTerrain", kege::MeshType::FlatTerrain);
            operator()(ui, "SphericalTerrain", kege::MeshType::SphericalTerrain);
        }
        std::vector< kege::MeshType > _mesh_types;
        std::vector< ui::Text > _list;
        std::vector< std::string > _strlist;
        kege::ui::Text _title;
        kege::ui::UID _uid;
        int _selection;
    };

    class PipelineFeatures
    {
    public:
    
        struct Feature
        {
            std::string name;
            FeatureFlag flag;
            ui::Text label;
            bool state;
        };
        struct Features
        {
            std::string name;
            bool expand;
            ui::Text title;
            std::vector<Feature> list;
        };

        void operator()( std::vector<Feature>& list, kege::FeatureFlag flag )
        {
            Feature feature;
            feature.name = featureFlagToString( flag );
            feature.label = kege::ui::Text
            {
                .x = 0.f,
                .y = 0.f,
                .width = 90.f,
                .font_size = 25,
                .color = 0xFFFFFF50,
                .data = feature.name.c_str(),
            };
            feature.state = false;
            feature.flag = flag;
            list.push_back(feature);
        }

        Features* getFeatures( const std::string& name )
        {
            _features.push_back
            ({
                name,
                false,
                ui::Text
                {
                    .x = 0.f,
                    .y = 0.f,
                    .width = 150.f,
                    .font_size = 25,
                    .color = 0xFFFFFF50,
                }
            });
            return &_features[ _features.size() - 1 ];
        }

        void update(kege::UI* ui, kege::PipelineKey& key)
        {
             ui->push({ .wid = ui->newElem(ui->theme()->card) });
            // ui->label(kege::ui::Text{ .data = "Mesh Type:", .x = 0, .y = 0, .width = 200, .height = 20, .color = 0xFFFFFF50 }, nullptr);
            int id_offset = 0;
            kege::ui::Text text_feature = kege::ui::Text{ 0.f, 0.f, 100.f, 20, 0xFFFFFF50, "Feature:" };


                const char* ptr = nullptr;   // 8 bytes
                float x = 0.f;
                float y = 0.f;
                float width = 0.f;
                float height = 20.f;
                uint16_t font_size = 20;
                uint32_t color = 0xFFFFFFFF;
                AlignText align = AlignText::Left;

            if( ui->collapsableHeader(_uid[ id_offset++ ], _expand_feature, text_feature) )
            {
                for(auto& features : _features)
                {
                    features.title.data = features.name.c_str();
                    ui->push({ .wid = ui->newElem(ui->theme()->card) });
                    if( ui->collapsableHeader(_uid[ id_offset++ ], features.expand, features.title) )
                    {
                        ui->push({ .wid = ui->newElem(ui->theme()->card) });
                        for(Feature& feature : features.list)
                        {
                            feature.label.data = feature.name.c_str();
                            if( ui->checkbox(_uid[ id_offset ], feature.label, feature.state) )
                            {
                            }
                            id_offset += 1;
                        }
                        ui->pop();
                    }
                    ui->pop();
                }
            }
             ui->pop();
        }

        PipelineFeatures(kege::UI* ui)
        {
            _list.reserve(64);
            Features* features = getFeatures( "Vertex Shader Features" );
            operator()(features->list, kege::FeatureFlag::VERTEX_COLOR);
            operator()(features->list, kege::FeatureFlag::INSTANCED_RENDERING);
            operator()(features->list, kege::FeatureFlag::VERTEX_DISPLACEMENT);
            operator()(features->list, kege::FeatureFlag::GPU_SKINNING);
            operator()(features->list, kege::FeatureFlag::MORPH_TARGETS);
            operator()(features->list, kege::FeatureFlag::VERTEX_ANIMATION);
            operator()(features->list, kege::FeatureFlag::WIND_ANIMATION);

            features = getFeatures( "Fragment Shader Features" );
            operator()(features->list, kege::FeatureFlag::NORMAL_MAPPING);
            operator()(features->list, kege::FeatureFlag::TRIPLANAR_MAPPING);
            operator()(features->list, kege::FeatureFlag::PARALLAX_MAPPING);
            operator()(features->list, kege::FeatureFlag::PARALLAX_OCCLUSION);
            operator()(features->list, kege::FeatureFlag::DETAIL_NORMAL);

            features = getFeatures( "Material Features" );
            operator()(features->list, kege::FeatureFlag::MATERIAL);
            operator()(features->list, kege::FeatureFlag::CLEAR_COAT);
            operator()(features->list, kege::FeatureFlag::ANISOTROPIC_REFLECTION);
            operator()(features->list, kege::FeatureFlag::SHEEN_LAYER);
            operator()(features->list, kege::FeatureFlag::THIN_FILM);
            operator()(features->list, kege::FeatureFlag::TRANSMISSION);
            operator()(features->list, kege::FeatureFlag::SUBSURFACE_SCATTERING);

            features = getFeatures( "Lighting Features" );
            operator()(features->list, kege::FeatureFlag::DIRECTIONAL_LIGHT);
            operator()(features->list, kege::FeatureFlag::POINT_LIGHT);
            operator()(features->list, kege::FeatureFlag::SPOT_LIGHT);
            operator()(features->list, kege::FeatureFlag::AREA_LIGHTS);
            operator()(features->list, kege::FeatureFlag::VOLUMETRIC_LIGHTING);
            operator()(features->list, kege::FeatureFlag::MULTIPLE_BOUNCE_GI);
            operator()(features->list, kege::FeatureFlag::IMAGE_BASE_LIGHTING);
            operator()(features->list, kege::FeatureFlag::CONTACT_SHADOWS);

            features = getFeatures( "Tesselation Features" );
            operator()(features->list, kege::FeatureFlag::TESSELLATION);
            operator()(features->list, kege::FeatureFlag::PN_TRIANGLES);
            operator()(features->list, kege::FeatureFlag::TESSELLATION_DISPLACEMENT);

            features = getFeatures( "Shadow Features" );
            operator()(features->list, kege::FeatureFlag::CONTACT_HARDENING);
            operator()(features->list, kege::FeatureFlag::VARIANCE_SHADOW_MAP);
            operator()(features->list, kege::FeatureFlag::CASCADE_BLENDING);

            features = getFeatures( "Transparency Features" );
            operator()(features->list, kege::FeatureFlag::ALPHA_TEST);
            operator()(features->list, kege::FeatureFlag::ALPHA_TO_COVERAGE);
            operator()(features->list, kege::FeatureFlag::DITHERED_OPACITY);
            operator()(features->list, kege::FeatureFlag::PREMULTIPLIED_ALPHA);

            features = getFeatures( "Particle Features" );
            operator()(features->list, kege::FeatureFlag::SOFT_PARTICLES);
            operator()(features->list, kege::FeatureFlag::PARTICLE_ROTATION);

            features = getFeatures( "Environment Features" );
            operator()(features->list, kege::FeatureFlag::SCREEN_SPACE_REFLECTION);
            operator()(features->list, kege::FeatureFlag::REFRACTION);
            operator()(features->list, kege::FeatureFlag::WATER_EFFECTS);
            operator()(features->list, kege::FeatureFlag::DECAL);

            features = getFeatures( "Post Process/Screen Space Features" );
            features = getFeatures( "Debug Development" );
            operator()(features->list, kege::FeatureFlag::GAMMA);
            operator()(features->list, kege::FeatureFlag::TONE_MAPPING);
            operator()(features->list, kege::FeatureFlag::MOTION_VECTORS);
            operator()(features->list, kege::FeatureFlag::VELOCITY_BUFFER);
            operator()(features->list, kege::FeatureFlag::DEPTH_ONLY);
            operator()(features->list, kege::FeatureFlag::CUSTOM_DEPTH);

            features = getFeatures( "Terrain / Vegetation" );
            operator()(features->list, kege::FeatureFlag::TERRAIN_BLENDING);
            operator()(features->list, kege::FeatureFlag::VERTEX_GRASS_WIND);
            operator()(features->list, kege::FeatureFlag::LOD_CROSSFADE);
            operator()(features->list, kege::FeatureFlag::DECAL_LAYER);

            features = getFeatures( "Advance Rendering" );
            operator()(features->list, kege::FeatureFlag::RAY_TRACING_OVERRIDE);
            operator()(features->list, kege::FeatureFlag::VIRTUAL_TEXTURE);
            operator()(features->list, kege::FeatureFlag::FLIPBOOK_ANIMATION);

            features = getFeatures( "UI Rendering" );
            operator()(features->list, kege::FeatureFlag::ROUNDED_UI_CORNERS);
            operator()(features->list, kege::FeatureFlag::SINE_DIST_TEXT);

            features = getFeatures( "Debug Development" );
            operator()(features->list, kege::FeatureFlag::WIREFRAME_OVERLAY);
            operator()(features->list, kege::FeatureFlag::DEBUG_NORMALS);
            operator()(features->list, kege::FeatureFlag::DEBUG_UVS);
            operator()(features->list, kege::FeatureFlag::DEBUG_TANGENTS);
        }

        std::vector< Features > _features;
        bool _expand_feature;

        std::vector< kege::MeshType > _mesh_types;
        std::vector< ui::Text > _list;
        kege::ui::UID _uid;
        int selection;
    };

    class RasterizerState
    {
    public:
    
        void update(kege::UI* ui, kege::PipelineKey& key)
        {
            ui->push({ .wid = ui->newElem(ui->theme()->card) });
            kege::ui::Text text_rasterizer = { .data = "Rasterizer State:", .x = 0, .y = 0, .width = 100, .font_size = 30, .color = 0xFFFFFF50 };
            if( ui->collapsableHeader(_uid[3][11], _expand, text_rasterizer) )
            {
                ui->push({ .wid = ui->newElem(ui->theme()->card) });
                {
                    ui->push({ .wid = ui->newElem(ui->theme()->row) });
                    {
                        kege::ui::Text text_polymode = { .data = "Polygon Mode:", .color = 0xFFFFFF50, .x = 0, .y = 0, .width = 85, .font_size = 20 };
                        kege::ui::Text text_cullmode = { .data = "Cull Mode:", .color = 0xFFFFFF50, .x = 0, .y = 0, .width = 65, .font_size = 20 };
                        kege::ui::Text text_front_face = { .data = "Front Face:", .color = 0xFFFFFF50, .x = 0, .y = 0, .width = 70, .font_size = 20 };
                        if( ui->labelOptions(_uid[0], text_polymode, _fill_list, _fill_selection))
                        {
                            key.raster_state.fill_mode = _fill_modes[_fill_selection];
                        }

                        if( ui->labelOptions(_uid[1], text_cullmode, _cull_list, _cull_selection))
                        {
                            key.raster_state.cull_mode = _cull_modes[_cull_selection];
                        }

                        if( ui->labelOptions(_uid[2], text_front_face, _front_list, _front_selection))
                        {
                            key.raster_state.front_face = _front_faces[_front_selection];
                        }
                    }
                    ui->pop();

                    kege::ui::Text text_lw = { .data = "Line Width:", .x = 0, .y = 0, .width = 75, .font_size = 20, .color = 0xFFFFFF50 };
                    ui->labelScrubber< float >(ScrubberState::Type::F32, _uid[3][6], text_lw, key.raster_state.line_width);

                    ui->push({ .wid = ui->newElem(ui->theme()->row) });
                    {
                        ui->push({ .wid = ui->newElem(ui->theme()->card) });
                        {
                            kege::ui::Text depth_clip_enable_label = { .data = "Depth Clip Enable", .x = 0, .y = 0, .width = 200, .font_size = 20, .color = 0xFFFFFF50 };
                            kege::ui::Text depth_clamp_enable_label = { .data = "Depth Clamp Enable", .x = 0, .y = 0, .width = 150, .font_size = 20, .color = 0xFFFFFF50 };
                            kege::ui::Text scissor_enable_label = { .data = "Scissor Enable", .x = 0, .y = 0, .width = 150, .font_size = 20, .color = 0xFFFFFF50 };
                            ui->checkbox( _uid[3][0], depth_clip_enable_label, key.raster_state.depth_clip_enable);
                            ui->checkbox( _uid[3][1], depth_clamp_enable_label, key.raster_state.depth_clamp_enable);
                            ui->checkbox( _uid[3][2], scissor_enable_label, key.raster_state.scissor_enable);
                        }
                        ui->pop();
                        ui->push({ .wid = ui->newElem(ui->theme()->card) });
                        {
                            kege::ui::Text multisample_enable_label = { .data = "Multisample Enable", .x = 0, .y = 0, .width = 150, .font_size = 20, .color = 0xFFFFFF50 };
                            kege::ui::Text antialiased_line_enable_label = { .data = "Antialiased Line Enable", .x = 0, .y = 0, .width = 150, .font_size = 20, .color = 0xFFFFFF50 };
                            kege::ui::Text discard_label = { .data = "Discard", .x = 0, .y = 0, .width = 150, .font_size = 20, .color = 0xFFFFFF50 };
                            ui->checkbox( _uid[3][3], multisample_enable_label, key.raster_state.multisample_enable);
                            ui->checkbox( _uid[3][4], antialiased_line_enable_label, key.raster_state.antialiased_line_enable);
                            ui->checkbox( _uid[3][5], discard_label, key.raster_state.discard);
                        }
                        ui->pop();
                    }
                    ui->pop();

                    ui->push({ .wid = ui->newElem(ui->theme()->card) });
                    {
                        kege::ui::Text text = { .data = "Depth Bias Enable", .x = 0, .y = 0, .width = 200, .font_size = 20, .color = 0xFFFFFF50 };
                        ui->checkbox( _uid[3][7], text, key.raster_state.depth_bias.enable);
                        if(key.raster_state.depth_bias.enable)
                        {
                            text.data = "Depth Bias Constant Factor:";
                            ui->labelScrubber<float>(ScrubberState::Type::F32, _uid[3][8], text, key.raster_state.depth_bias.constant_factor);
                            text.data = "Depth Bias Slope Factor:";
                            ui->labelScrubber<float>(ScrubberState::Type::F32, _uid[3][9], text, key.raster_state.depth_bias.slope_factor);
                            text.data = "Depth Bias Clamp:";
                            ui->labelScrubber<float>(ScrubberState::Type::F32, _uid[3][10], text, key.raster_state.depth_bias.clamp);
                        }
                    }
                    ui->pop();
                }
            }
            ui->pop();
        }

        RasterizerState(kege::UI* ui)
        {
            _expand = false;
            _fill_selection = 0;
            _cull_selection = 0;
            _front_selection = 0;
            _fill_modes.push_back( kege::FillMode::Fill );
            _fill_modes.push_back( kege::FillMode::Line );
            _fill_modes.push_back( kege::FillMode::Point );
            _fill_list.push_back( ui->gui()->text("Fill", 20) );
            _fill_list.push_back( ui->gui()->text("Line", 20) );
            _fill_list.push_back( ui->gui()->text("Point", 20) );

            _cull_modes.push_back( kege::CullMode::None );
            _cull_modes.push_back( kege::CullMode::Front );
            _cull_modes.push_back( kege::CullMode::Back );
            _cull_list.push_back( ui->gui()->text("None", 20) );
            _cull_list.push_back( ui->gui()->text("Front", 20) );
            _cull_list.push_back( ui->gui()->text("Back", 20) );

            _front_faces.push_back( kege::FrontFace::Clockwise );
            _front_faces.push_back( kege::FrontFace::CounterClockwise );
            _front_list.push_back( ui->gui()->text("CW", 20) );
            _front_list.push_back( ui->gui()->text("CCW", 20) );
        }
        std::vector< kege::FillMode > _fill_modes;
        std::vector< kege::CullMode > _cull_modes;
        std::vector< kege::FrontFace > _front_faces;
        std::vector< ui::Text > _fill_list;
        std::vector< ui::Text > _cull_list;
        std::vector< ui::Text > _front_list;
        kege::ui::UID _uid[6];
        int _fill_selection;
        int _cull_selection;
        int _front_selection;
        bool _expand;
    };

    class PrimitiveTopology
    {
    public:
    
        void operator()(kege::UI* ui, const std::string& name, kege::PrimitiveTopology model)
        {
            _strlist.push_back(name);

            _primitive_topologys.push_back(model);
            _list.push_back(ui->gui()->text(_strlist[ _strlist.size() - 1 ].c_str(), 20));
        }

        void update(kege::UI* ui, kege::PipelineKey& key)
        {
            if(ui->labelOptions(_uid, _title, _list, _selection))
            {
                key.topology = _primitive_topologys[_selection];
            }
        }

        PrimitiveTopology(kege::UI* ui)
        {
            _expand = false;
            _selection = 0;
            _title = { .data = "Primitive Topology:", .x = 0, .y = 0, .width = 122, .font_size = 20, .color = 0xFFFFFF70 };
            if(_list.empty())
            {
                _strlist.reserve(11);
                operator()(ui, "TriangleList", kege::PrimitiveTopology::TriangleList);
                operator()(ui, "TriangleStrip", kege::PrimitiveTopology::TriangleStrip);
                operator()(ui, "TriangleListAdjacency", kege::PrimitiveTopology::TriangleListAdjacency);
                operator()(ui, "TriangleStripAdjacency", kege::PrimitiveTopology::TriangleStripAdjacency);
                operator()(ui, "LineList", kege::PrimitiveTopology::LineList);
                operator()(ui, "LineStrip", kege::PrimitiveTopology::LineStrip);
                operator()(ui, "LineListAdjacency", kege::PrimitiveTopology::LineListAdjacency);
                operator()(ui, "LineStripAdjacency", kege::PrimitiveTopology::LineStripAdjacency);
                operator()(ui, "PointList", kege::PrimitiveTopology::PointList);
                operator()(ui, "PatchList", kege::PrimitiveTopology::PatchList);
            }
        }
        std::vector< kege::PrimitiveTopology > _primitive_topologys;
        std::vector< std::string > _strlist;
        std::vector< ui::Text > _list;
        kege::ui::Text _title;
        kege::ui::UID _uid_butn;
        kege::ui::UID _uid;
        int _selection;
        bool _expand;
    };

    class BlendState
    {
    public:

        void update(kege::UI* ui, kege::PipelineKey& key)
        {
            ui->push({ .wid = ui->newElem(ui->theme()->card) });
            kege::ui::Text text = kege::ui::Text{ .data = "Blend State:", .x = 0, .y = 0, .width = 200, .font_size = 20, .color = 0xFFFFFF50 };
            if( ui->collapsableHeader(_uid[3][11], _expand, text) )
            {
                ui->push({ .wid = ui->newElem(ui->theme()->card) });
                {
                    text.width = 100;
                    text.data = "Enable Blend:";
                    ui->checkbox( _uid[0][0], text, key.blend_state.enable);

                    text.width = 60;
                    ui->push({ .wid = ui->newElem(ui->theme()->card2) });
                    {
                        text.data = "Src Color:";
                        if( ui->labelOptions(_uid[3], text, _blend_factor_list, _selection_src_color_blend))
                        {
                            key.blend_state.src_color_blend = _blend_factors[ _selection_src_color_blend ];
                        }

                        text.data = "Src Alpha:";
                        if( ui->labelOptions(_uid[1], text, _blend_factor_list, _selection_src_alpha_blend))
                        {
                            key.blend_state.src_alpha_blend = _blend_factors[ _selection_src_alpha_blend ];
                        }
                    }
                    ui->pop();

                    ui->push({ .wid = ui->newElem(ui->theme()->card2) });
                    {
                        text.data = "Dst Color:";
                        if( ui->labelOptions(_uid[2], text, _blend_factor_list, _selection_dst_color_blend))
                        {
                            key.blend_state.dst_color_blend = _blend_factors[ _selection_dst_color_blend ];
                        }

                        text.data = "Dst Alpha:";
                        if( ui->labelOptions(_uid[4], text, _blend_factor_list, _selection_dst_alpha_blend))
                        {
                            key.blend_state.dst_alpha_blend = _blend_factors[ _selection_dst_alpha_blend ];
                        }
                    }
                    ui->pop();

                    text.width = 95;
                    ui->push({ .wid = ui->newElem(ui->theme()->card2) });
                    {
                        text.data = "Color Blend Op:";
                        if( ui->labelOptions(_uid[6], text, _blend_op_list, _selection_color_blend_op))
                        {
                            key.blend_state.color_blend_op = _blend_ops[ _selection_color_blend_op ];
                        }

                        text.data = "Alpha Blend Op:";
                        if( ui->labelOptions(_uid[5], text, _blend_op_list, _selection_alpha_blend_op))
                        {
                            key.blend_state.alpha_blend_op = _blend_ops[ _selection_alpha_blend_op ];
                        }
                    }
                    ui->pop();

                    text.width = 90;
                    text.data = "Color Mark:";
                    if( ui->labelOptions(_uid[7], text, _color_write_mask_list, _selection_color_write_mask))
                    {
                        key.blend_state.color_write_mask = _color_write_masks[ _selection_color_write_mask ];
                    }
                }
                ui->pop();
            }
            ui->pop();
        }

        void setBlendFactor(kege::UI* ui, const std::string& name, kege::BlendFactor model)
        {
            _strlist_blend_factor.push_back(name);
            const char* str = _strlist_blend_factor[ _strlist_blend_factor.size() - 1 ].c_str();
            _blend_factors.push_back(model);
            _blend_factor_list.push_back(ui->gui()->text(str, 20));
        }

        void setBlendOp(kege::UI* ui, const std::string& name, kege::BlendOp blend_op)
        {
            _strlist_blend_op.push_back(name);
            const char* str = _strlist_blend_op[ _strlist_blend_op.size() - 1 ].c_str();
            _blend_ops.push_back( blend_op );
            _blend_op_list.push_back( ui->gui()->text(str, 20) );
        }

        void setBlendStatePreset(kege::UI* ui, const std::string& name, const kege::BlendState& blend_state)
        {
            _strlist_blend_presets.push_back(name);
            const char* str = _strlist_blend_presets[ _strlist_blend_presets.size() - 1 ].c_str();
            _blend_state_presets.push_back(blend_state);
            _blend_state_presets_list.push_back( ui->gui()->text(str, 20) );
        }

        void insertColorWriteMask(kege::UI* ui, const std::string& name, const kege::ColorWriteMask& color_write_mask)
        {
            _strlist_color_write_mask.push_back(name);
            const char* str = _strlist_color_write_mask[ _strlist_color_write_mask.size() - 1 ].c_str();
            _color_write_masks.push_back(color_write_mask);
            _color_write_mask_list.push_back( ui->gui()->text(str, 20) );
        }

        BlendState(kege::UI* ui)
        {
            _selection_dst_color_blend = 0;
            _selection_color_blend_op = 0;
            _selection_src_color_blend = 0;
            _selection_src_alpha_blend = 0;
            _selection_dst_alpha_blend = 0;
            _selection_alpha_blend_op = 0;
            _selection_color_write_mask = 0;

            _strlist_color_write_mask.reserve(7);
            _strlist_blend_presets.reserve(8);
            _strlist_blend_factor.reserve(20);
            _strlist_blend_op.reserve(6);

            setBlendStatePreset(ui, "Opaque", kege::BlendStatePreset::opaque() );
            setBlendStatePreset(ui, "Alpha Blend", kege::BlendStatePreset::alphaBlend() );
            setBlendStatePreset(ui, "Additive", kege::BlendStatePreset::additive() );
            setBlendStatePreset(ui, "Multiplicative", kege::BlendStatePreset::multiplicative() );
            setBlendStatePreset(ui, "PreMultiplied Alpha", kege::BlendStatePreset::preMultipliedAlpha() );
            setBlendStatePreset(ui, "UI Alpha Blend", kege::BlendStatePreset::uiAlphaBlend() );
            setBlendStatePreset(ui, "Light Accumulation", kege::BlendStatePreset::lightAccumulation() );

            setBlendFactor(ui, "Zero", kege::BlendFactor::Zero);
            setBlendFactor(ui, "One", kege::BlendFactor::One);
            setBlendFactor(ui, "SrcColor", kege::BlendFactor::SrcColor);
            setBlendFactor(ui, "OneMinusSrcColor", kege::BlendFactor::OneMinusSrcColor);
            setBlendFactor(ui, "SrcAlpha", kege::BlendFactor::SrcAlpha);
            setBlendFactor(ui, "OneMinusSrcAlpha", kege::BlendFactor::OneMinusSrcAlpha);
            setBlendFactor(ui, "SrcAlphaSaturate", kege::BlendFactor::SrcAlphaSaturate);
            setBlendFactor(ui, "Src1Color", kege::BlendFactor::Src1Color);
            setBlendFactor(ui, "OneMinusSrc1Color", kege::BlendFactor::OneMinusSrc1Color);
            setBlendFactor(ui, "Src1Alpha", kege::BlendFactor::Src1Alpha);
            setBlendFactor(ui, "OneMinusSrc1Alpha", kege::BlendFactor::OneMinusSrc1Alpha);
            setBlendFactor(ui, "DstColor", kege::BlendFactor::DstColor);
            setBlendFactor(ui, "OneMinusDstColor", kege::BlendFactor::OneMinusDstColor);
            setBlendFactor(ui, "DstAlpha", kege::BlendFactor::DstAlpha);
            setBlendFactor(ui, "OneMinusDstAlpha", kege::BlendFactor::OneMinusDstAlpha);
            setBlendFactor(ui, "ConstantColor", kege::BlendFactor::ConstantColor);
            setBlendFactor(ui, "OneMinusConstantColor", kege::BlendFactor::OneMinusConstantColor);
            setBlendFactor(ui, "ConstantAlpha", kege::BlendFactor::ConstantAlpha);
            setBlendFactor(ui, "OneMinusConstantAlpha", kege::BlendFactor::OneMinusConstantAlpha);

            setBlendOp(ui, "Add", kege::BlendOp::Add );
            setBlendOp(ui, "Subtract", kege::BlendOp::Subtract );
            setBlendOp(ui, "ReverseSubtract", kege::BlendOp::ReverseSubtract );
            setBlendOp(ui, "Min", kege::BlendOp::Min );
            setBlendOp(ui, "Max", kege::BlendOp::Max );

            insertColorWriteMask(ui, "Red", kege::ColorWriteMask::Red);
            insertColorWriteMask(ui, "Green", kege::ColorWriteMask::Green);
            insertColorWriteMask(ui, "Blue", kege::ColorWriteMask::Blue);
            insertColorWriteMask(ui, "Alpha", kege::ColorWriteMask::Alpha);
            insertColorWriteMask(ui, "RGB", kege::ColorWriteMask::RGB);
            insertColorWriteMask(ui, "RGBA", kege::ColorWriteMask::RGBA);
        }

        std::vector< kege::BlendState > _blend_state_presets;

        std::vector< kege::ColorWriteMask > _color_write_masks;
        std::vector< kege::BlendFactor > _blend_factors;
        std::vector< kege::BlendOp > _blend_ops;

        std::vector< ui::Text > _color_write_mask_list;
        std::vector< ui::Text > _blend_state_presets_list;
        std::vector< ui::Text > _blend_factor_list;
        std::vector< ui::Text > _blend_op_list;

        std::vector< std::string > _strlist_blend_presets;
        std::vector< std::string > _strlist_color_write_mask;
        std::vector< std::string > _strlist_blend_factor;
        std::vector< std::string > _strlist_blend_op;

        kege::ui::UID _uid[8];

        int _selection_dst_color_blend;
        int _selection_color_blend_op;
        int _selection_src_color_blend;
        int _selection_src_alpha_blend;
        int _selection_dst_alpha_blend;
        int _selection_alpha_blend_op;
        int _selection_color_write_mask;
        bool _expand;
    };

    class CreatePipeline : public AssetManagerModule
    {
    public:

        enum{WINDOW, BANNER, CLOSE_BUTN, NAMING, SCROLL_CLIPPER, SCROLL_CONTAINER, OFFSET_COUNT, };
        CreatePipeline(AssetManagerUI* m,kege::UI* g);
        void operator()(const std::string& type, void* data);
        void update();
        
        ~CreatePipeline();

    private:

        kege::PipelineKey _pipeline_key;
        kege::MaterialParams _parameters;

        PrimitiveTopology _primitive_topology_ui;
        RasterizerState _rasterizer_state_ui;
        BlendState _blend_state_ui;
        RenderPasses _render_passes;
        PipelineFeatures _pipeline_features_ui;
        MeshTypes _mesh_type_ui;
        ShadingModels _shader_model_ui;
        DepthStencilState _depth_stencil_ui;

        ui::Text _asset_label;
        // TextFieldMode _text_input_mode;
        uint32_t _created_counter;

        ui::UID _uid;
        // uint32_t _id_offset;

        kege::vec2 position;
        //ui::ID _close_butn;
        bool _open_window;
        ui::Rect _window_rect;
        // int _selection;

        kege::TextFieldMode _textmode;
        ui::Text _name;
        
        // size_t _current_size = 0;
        // size_t _buffer_capacity = 0;
        char _asset_name[32];
    };

}
#endif /* ui_create_pipeline_hpp */

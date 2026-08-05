//
//  ui-create-material.cpp
//  editor
//
//  Created by Kenneth Esdaile on 4/4/26.
//

#include "ui-create-material.hpp"
#include "../ui-asset-manager.hpp"
#include "../../../common/ui-schema.hpp"
#include "../../../dock/ui-dock-manager.hpp"
#include "../../../editor-layer.hpp"

namespace kege::ui{

    CreateMaterial::CreateMaterial(AssetManagerUI* m,kege::UI* g)
    : AssetManagerModule(m,g), _selection(0), _id_offset(0), _count(0)
    {
        Features* features = getFeatures( "Vertex Shader Features" );
        setFeature(features->list, kege::FeatureFlag::VERTEX_COLOR);
        setFeature(features->list, kege::FeatureFlag::INSTANCED_RENDERING);
        setFeature(features->list, kege::FeatureFlag::VERTEX_DISPLACEMENT);
        setFeature(features->list, kege::FeatureFlag::GPU_SKINNING);
        setFeature(features->list, kege::FeatureFlag::MORPH_TARGETS);
        setFeature(features->list, kege::FeatureFlag::VERTEX_ANIMATION);
        setFeature(features->list, kege::FeatureFlag::WIND_ANIMATION);

        features = getFeatures( "Fragment Shader Features" );
        setFeature(features->list, kege::FeatureFlag::NORMAL_MAPPING);
        setFeature(features->list, kege::FeatureFlag::TRIPLANAR_MAPPING);
        setFeature(features->list, kege::FeatureFlag::PARALLAX_MAPPING);
        setFeature(features->list, kege::FeatureFlag::PARALLAX_OCCLUSION);
        setFeature(features->list, kege::FeatureFlag::DETAIL_NORMAL);

        features = getFeatures( "Material Features" );
        setFeature(features->list, kege::FeatureFlag::MATERIAL);
        setFeature(features->list, kege::FeatureFlag::CLEAR_COAT);
        setFeature(features->list, kege::FeatureFlag::ANISOTROPIC_REFLECTION);
        setFeature(features->list, kege::FeatureFlag::SHEEN_LAYER);
        setFeature(features->list, kege::FeatureFlag::THIN_FILM);
        setFeature(features->list, kege::FeatureFlag::TRANSMISSION);
        setFeature(features->list, kege::FeatureFlag::SUBSURFACE_SCATTERING);

        features = getFeatures( "Lighting Features" );
        setFeature(features->list, kege::FeatureFlag::DIRECTIONAL_LIGHT);
        setFeature(features->list, kege::FeatureFlag::POINT_LIGHT);
        setFeature(features->list, kege::FeatureFlag::SPOT_LIGHT);
        setFeature(features->list, kege::FeatureFlag::AREA_LIGHTS);
        setFeature(features->list, kege::FeatureFlag::VOLUMETRIC_LIGHTING);
        setFeature(features->list, kege::FeatureFlag::MULTIPLE_BOUNCE_GI);
        setFeature(features->list, kege::FeatureFlag::IMAGE_BASE_LIGHTING);
        setFeature(features->list, kege::FeatureFlag::CONTACT_SHADOWS);

        features = getFeatures( "Tesselation Features" );
        setFeature(features->list, kege::FeatureFlag::TESSELLATION);
        setFeature(features->list, kege::FeatureFlag::PN_TRIANGLES);
        setFeature(features->list, kege::FeatureFlag::TESSELLATION_DISPLACEMENT);

        features = getFeatures( "Shadow Features" );
        setFeature(features->list, kege::FeatureFlag::CONTACT_HARDENING);
        setFeature(features->list, kege::FeatureFlag::VARIANCE_SHADOW_MAP);
        setFeature(features->list, kege::FeatureFlag::CASCADE_BLENDING);

        features = getFeatures( "Transparency Features" );
        setFeature(features->list, kege::FeatureFlag::ALPHA_TEST);
        setFeature(features->list, kege::FeatureFlag::ALPHA_TO_COVERAGE);
        setFeature(features->list, kege::FeatureFlag::DITHERED_OPACITY);
        setFeature(features->list, kege::FeatureFlag::PREMULTIPLIED_ALPHA);

        features = getFeatures( "Particle Features" );
        setFeature(features->list, kege::FeatureFlag::SOFT_PARTICLES);
        setFeature(features->list, kege::FeatureFlag::PARTICLE_ROTATION);

        features = getFeatures( "Environment Features" );
        setFeature(features->list, kege::FeatureFlag::SCREEN_SPACE_REFLECTION);
        setFeature(features->list, kege::FeatureFlag::REFRACTION);
        setFeature(features->list, kege::FeatureFlag::WATER_EFFECTS);
        setFeature(features->list, kege::FeatureFlag::DECAL);

        features = getFeatures( "Post Process/Screen Space Features" );
        features = getFeatures( "Debug Development" );
        setFeature(features->list, kege::FeatureFlag::GAMMA);
        setFeature(features->list, kege::FeatureFlag::TONE_MAPPING);
        setFeature(features->list, kege::FeatureFlag::MOTION_VECTORS);
        setFeature(features->list, kege::FeatureFlag::VELOCITY_BUFFER);
        setFeature(features->list, kege::FeatureFlag::DEPTH_ONLY);
        setFeature(features->list, kege::FeatureFlag::CUSTOM_DEPTH);

        features = getFeatures( "Terrain / Vegetation" );
        setFeature(features->list, kege::FeatureFlag::TERRAIN_BLENDING);
        setFeature(features->list, kege::FeatureFlag::VERTEX_GRASS_WIND);
        setFeature(features->list, kege::FeatureFlag::LOD_CROSSFADE);
        setFeature(features->list, kege::FeatureFlag::DECAL_LAYER);

        features = getFeatures( "Advance Rendering" );
        setFeature(features->list, kege::FeatureFlag::RAY_TRACING_OVERRIDE);
        setFeature(features->list, kege::FeatureFlag::VIRTUAL_TEXTURE);
        setFeature(features->list, kege::FeatureFlag::FLIPBOOK_ANIMATION);

        features = getFeatures( "UI Rendering" );
        setFeature(features->list, kege::FeatureFlag::ROUNDED_UI_CORNERS);
        setFeature(features->list, kege::FeatureFlag::SINE_DIST_TEXT);

        features = getFeatures( "Debug Development" );
        setFeature(features->list, kege::FeatureFlag::WIREFRAME_OVERLAY);
        setFeature(features->list, kege::FeatureFlag::DEBUG_NORMALS);
        setFeature(features->list, kege::FeatureFlag::DEBUG_UVS);
        setFeature(features->list, kege::FeatureFlag::DEBUG_TANGENTS);
    }

    void CreateMaterial::operator()(const std::string& type, void* data)
    {
        if (type == "OpenCreateMaterialWindow")
        {
            snprintf(_asset_name, 31, "material-%i", _count);
            _text = _ui->gui()->text(_asset_name, 20);

            _open_window = true;
            _window_rect.width = 600;
            _window_rect.height = 400;
            _window_rect.x = (_ui->gui()->getRect().width - _window_rect.width) * 0.5;
            _window_rect.y = (_ui->gui()->getRect().height - _window_rect.height) * 0.5;
        }
    }

    const kege::ui::Property< kege::MaterialParams > MaterialParameters[5] = {
        UI_PROP
        (
            kege::MaterialParams, albedo, kege::ui::PropertyMeta{
                .type = PropertyType::Vec4,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0 },
                .text = {0.f, 0.f, 100.f, 20, 0xFFFFFFFF, "Color:"},
            }
        ),
        UI_PROP
        (
            kege::MaterialParams, roughness, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0 },
                .text = {0.f, 0.f, 90.f, 20, 0xFFFFFFFF, "Roughness:" },
            }
        ),
        UI_PROP
        (
            kege::MaterialParams, metallic, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0 },
                .text = {0.f, 0.f, 90.f, 20, 0xFFFFFFFF, "Metallic:" },
            }
        ),
        UI_PROP
        (
            kege::MaterialParams, ao, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0 },
                .text = {0.f, 0.f, 90.f, 20, 0xFFFFFFFF, "AO:", },
            }
        ),
        UI_PROP
        (
            kege::MaterialParams, alpha_cutoff, kege::ui::PropertyMeta{
                .type = PropertyType::Float,
                .hint = {kege::ui::UIHint::DragNum, true, 0.0, 1.0 },
                .text = {0.f, 0.f, 90.f, 20, 0xFFFFFFFF, "Alpha Cutoff:", },
            }
        )
    };

    void CreateMaterial::update()
    {
        if (!_open_window) return;

        ui::ID id[3] = {_uid[WINDOW], _uid[BANNER], _uid[CLOSE_BUTN]};
        _ui->beginWindow(id, _window_rect, "Create Material", _open_window);
        {
            ui::ID scroll_id[2] = {_uid[SCROLL_CLIPPER], _uid[SCROLL_CONTAINER]};
            _ui->beginScrollContainer(scroll_id);
            {
                _ui->labelInput("Name:", _uid[NAMING], _text_input_mode, _text);
                _id_offset = OFFSET_COUNT;
                ui::drawProperties(_ui, _uid, _id_offset, _parameters, MaterialParameters);

                PipelineKey key;
                ui::Text text_feature = {0.f, 0.f, 100.f, 20, 0xFFFFFFFF, "Feature", };
                if( _ui->collapsableHeader(_uid[ _id_offset++ ], _expand_feature, text_feature) )
                {
                    _ui->push
                    ({
                        .wid = _ui->newElem({
                            .width = ui::extend(),
                            .height = ui::flexible(),
                            .quad_color = 0xFFFFFF00,
                            .padding = {20, 0, 20, 10,},
                            .alignment = {
                                .gap = {5,5},
                                .direction = {ui::AlignDir::DOWN}
                            },
                        })
                    });
                    for(auto& features : _features)
                    {
                        features.title.data = features.name.c_str();
                        if( _ui->collapsableHeader(_uid[ _id_offset++ ], features.expand, features.title) )
                        {
                            _ui->push
                            ({
                                .wid = _ui->newElem
                                ({
                                    .width = ui::extend(),
                                    .height = ui::flexible(),
                                    .quad_color = 0xFFFFFF00,
                                    .padding = {20, 0, 20, 10,},
                                    .alignment = {
                                        .direction = {ui::AlignDir::DOWN}
                                    },
                                })
                                //.gap.height = 5,
                            });
                            for(Feature& feature : features.list)
                            {
                                feature.label.data = feature.name.c_str();
                                if( _ui->checkbox(_uid[ _id_offset ], feature.label, feature.state) )
                                {
                                }
                                _id_offset += 1;
                            }
                            _ui->pop();
                        }
                    }
                    _ui->pop();
                }

                //uint64_t user_id = UI_BASE_ID();
                if( _ui->submit(_uid[ _id_offset++ ], "Submit") )
                {
//                    ref::AssetManager asset_manager = _manager->getManager()->getEditor()->getAssetManager();
//                    kege::RenderPassType pass;
//                    ref::ShaderPipeline pipeline;
//                    SetNames binding_names;
//
//                    kege::Ref< kege::Material > material = new kege::Material(pass, pipeline, binding_names);
//
//                    // Create metadata
//                    AssetMetadata metadata;
//                    metadata.handle         = asset_manager->add< kege::Ref< kege::Material > >(_asset_name, material);
//                    metadata.name           = _asset_name;
//                    metadata.display_name   = metadata.name;
//                    metadata.original_path  = "";
//                    metadata.type           = AssetType::MATERIAL;
//                    metadata.category       = "Material";
//                    metadata.loader_id      = ".mtl";
//                    metadata.import_time    = std::chrono::system_clock::now();
//                    metadata.last_modified  = std::filesystem::file_time_type::min();
//                    metadata.file_size      = 0;
//                    metadata.missing_source = false;
//                    metadata.loaded         = true;
//                    _manager->addAsset(metadata);
//                    _asset_name[0] = 0;
                }
            }
            _ui->endScrollContainer();
        }
        _ui->endWindow();
    }
    void CreateMaterial::setFeature( std::vector<Feature>& list, kege::FeatureFlag flag )
    {
        Feature feature;
        feature.name = featureFlagToString( flag );
        feature.label = kege::ui::Text
        {
            .x = 0.f, .y = 0.f, .width = 90.f, .font_size = 25,
            .color = 0xFFFFFF50, .data = feature.name.c_str(),
        };
        feature.state = false;
        feature.flag = flag;
        list.push_back(feature);
    }

    CreateMaterial::Features* CreateMaterial::getFeatures( const std::string& name )
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

    CreateMaterial::~CreateMaterial()
    {
//        _create_mesh_uis.clear();
    }
}


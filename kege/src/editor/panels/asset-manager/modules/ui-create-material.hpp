//
//  ui-create-material.hpp
//  editor
//
//  Created by Kenneth Esdaile on 4/4/26.
//

#ifndef ui_create_material_hpp
#define ui_create_material_hpp

#include "asset-manager-module.hpp"
#include "../mesh/create-mesh-ui.hpp"

namespace kege::ui{

    class CreateMaterial : public AssetManagerModule
    {
    public:

        enum{WINDOW, BANNER, CLOSE_BUTN, NAMING, SCROLL_CLIPPER, SCROLL_CONTAINER, OFFSET_COUNT, };
        CreateMaterial(AssetManagerUI* m,kege::GUI* g);
        void operator()(const std::string& type, void* data);
        void update();
        
        ~CreateMaterial();

    private:

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

        void setFeature( std::vector<Feature>& list, kege::FeatureFlag flag );
        Features* getFeatures( const std::string& name );
        kege::Material::Parameters _parameters;

        std::vector< Features > _features;
        bool _expand_feature;

        ui::Text _text;
        TextFieldMode _text_input_mode;
        uint32_t _count;

        ui::UID _uid;
        uint32_t _id_offset;

        kege::vec2 position;
        //ui::ID _close_butn;
        bool _open_window;
        ui::Rect _window_rect;
        int _selection;

        size_t _current_size = 0;
        size_t _buffer_capacity = 0;
        char _asset_name[32];
    };

}
#endif /* ui_create_material_hpp */

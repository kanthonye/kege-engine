//
//  ui-panel.hpp
//  editor
//
//  Created by Kenneth Esdaile on 11/9/25.
//

#ifndef ui_panel_hpp
#define ui_panel_hpp

#include "../common/dependencies.hpp"


namespace kege::ui{
    class DockManager;

    struct Panel : public kege::RefCounter
    {
        static void updateRectOp(ui::Layout* layout, ui::ID user_id, ui::WidgetId widget_id, void* data);
        virtual void operator()(const kege::ui::AssetMetadataDropOff& event);
        virtual void handle(const kege::ui::AssetMetadataDropOff& event){}

        const std::string getName()const{ return _name; }
        kege::ui::DockManager* getManager(){ return _manager; }
        virtual void updateLayout( int16_t layer ) = 0;
        virtual void update();

        Panel( const std::string& name, kege::ui::DockManager* e );
        virtual ~Panel(){}



        kege::ui::DockManager* _manager;
        kege::GUI* _gui;
        std::string _name;

        ui::WidgetId _widget_index;
        kege::ui::Rect _rect;
        kege::ui::UID _uid;
    };

}
#endif /* ui_panel_hpp */

//
//  asset-manager-folder-tree.hpp
//  editor
//
//  Created by Kenneth Esdaile on 3/15/26.
//

#ifndef asset_manager_folder_tree_hpp
#define asset_manager_folder_tree_hpp

#include "asset-manager-module.hpp"

namespace kege::ui{

    class AssetManagerFolderTree : public AssetManagerModule
    {
    public:

        // Folder/category structure
        struct FolderNode
        {
            std::string name;
            std::string full_path;
            std::vector<std::shared_ptr<FolderNode>> children;
            FolderNode* parent = nullptr;

            // Statistics
            size_t asset_count = 0;
            size_t subfolder_count = 0;
        };

        enum class ImportStatus { PENDING, PROCESSING, COMPLETE, FAILED };

        AssetManagerFolderTree(AssetManagerUI* m,kege::UI* g)
        : AssetManagerModule(m,g){}
        void operator()(const std::string& type, void* data);


        AssetManagerFolderTree::FolderNode* ensureFolderExists(const std::string& path);
        void rebuildFolderStructure();
        void refreshAssetList();

        void updateFilteredAssets();
        void update();

    private:

        std::vector<std::string> _string_categories;
        std::vector< kege::ui::Text > _categories;
        std::vector< size_t > _filtered_indices;

        kege::ui::Text _current_category;

        std::string _search_text;
        std::set<AssetType> _type_filters;
        bool _show_favorites_only = false;

        std::shared_ptr<FolderNode> _root_folder;
        std::unordered_map<std::string, FolderNode*> _folder_map;
    };

}

#endif /* asset_manager_folder_tree_hpp */

//
//  dependencies.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 3/16/26.
//

#ifndef editor_dependencies_hpp
#define editor_dependencies_hpp

#include <set>
#include <mutex>
#include <thread>
#include <chrono>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include <functional>
#include <unordered_map>

#include "../../core/app/app-layer.hpp"
#include "../../core/app/project-manager.hpp"
#include "../../core/scene/ecs.hpp"
#include "../../core/scene/scene.hpp"
#include "../../core/input/input-context-manager.hpp"

#include "../../core/scene/ecs.hpp"
#include "../../core/graphics/camera/camera.hpp"
#include "../../core/graphics/mesh/mesh-primitives.hpp"
#include "../../core/graphics/light/light.hpp"
#include "../../core/physics/3d/bodies/rigidbody.hpp"
#include "../../core/graphics/mesh/mesh-cylinder.hpp"
#include "../../core/graphics/mesh/mesh-cuboid.hpp"
#include "../../core/graphics/mesh/mesh-rect.hpp"
#include "../../core/graphics/mesh/mesh-cylinder.hpp"
#include "../../core/graphics/particle/effect/particle-effect.hpp"

#include "gui.hpp"

namespace kege{
    class EditorLayer;
}

namespace kege::ui{

    struct DockSplit;

    enum class ViewMode { GRID, LIST };

    namespace fs = std::filesystem;
    enum class AssetType
    {
        UNKNOWN,
        TEXTURE,
        MODEL,
        SHADER,
        SOUND,
        SCRIPT,
        MATERIAL,
        FONT,
        // Add more as needed
    };

    struct AssetMetadata
    {
        std::string name;          // Asset name (stem)
        std::string display_name;  // User-friendly display name
        std::string original_path; // Full original file path
        AssetType type;
        uint64_t handle;           // AssetManager handle
        std::string loader_id;     // Loader extension/type
        std::chrono::system_clock::time_point import_time;
        fs::file_time_type last_modified;
        size_t file_size;

        // User-defined metadata
        std::set<std::string> tags;
        std::string category;
        bool is_favorite = false;

        uint64_t asset_uid;
        uint64_t thumbnail_uid;
        uint64_t name_uid;


        // Import settings (type-specific)
        struct ImportSettings
        {
            // Texture settings
            struct TextureSettings
            {
                bool generate_mipmaps = true;
                bool srgb = false;
                bool compress = true;
            }
            texture;

            // Model settings
            struct ModelSettings
            {
                float scale = 1.0f;
                bool flip_uvs = true;
                bool generate_normals = false;
            }
            model;

            // Shader settings
            struct ShaderSettings
            {
                bool optimize = true;
                bool debug_info = false;
            }
            shader;

            // Sound settings
            struct SoundSettings
            {
                bool stream = false;
                bool loop = false;
                float volume = 1.0f;
            }
            sound;
        }
        import_settings;

        // Status
        bool loaded = false;
        bool missing_source = false;
        std::string last_error;

        // Thumbnail placeholder (you'll replace with actual thumbnails)
        uint32_t placeholder_color = 0x808080FF; // Gray

        // Get color based on asset type
        uint32_t type_color() const
        {
            switch (type)
            {
                case AssetType::TEXTURE: return 0xFF6B6BFF; // Red
                case AssetType::MODEL:   return 0x4ECDC4FF; // Teal
                case AssetType::SHADER:  return 0x45B7D1FF; // Blue
                case AssetType::SOUND:   return 0x96CEB4FF; // Green
                case AssetType::SCRIPT:  return 0xFFEAA7FF; // Yellow
                case AssetType::MATERIAL:return 0xDDA0DDFF; // Purple
                case AssetType::FONT:    return 0xFFA07AFF; // Orange
                default:                 return 0x808080FF; // Gray
            }
        }

        std::string type_string() const
        {
            switch (type)
            {
                case AssetType::TEXTURE: return "Texture";
                case AssetType::MODEL:    return "Model";
                case AssetType::SHADER:  return "Shader";
                case AssetType::SOUND:   return "Sound";
                case AssetType::SCRIPT:  return "Script";
                case AssetType::MATERIAL:return "Material";
                case AssetType::FONT:    return "Font";
                default:                 return "Unknown";
            }
        }
    };

    struct DragDropOffAssetMetadata
    {
        uint64_t handle;
        kege::dvec2 pointer;
    };


    typedef bool(*InspectorComponentUIBuilder)( kege::AssetManager* am, int16_t layer, kege::GUI*, kege::ECS* ecs, ecs::Entity& );
}

namespace kege::ui{

    struct TextFieldState
    {
        size_t cursor_pos = 0;
        size_t selection_start = 0;
        size_t selection_end = 0;
        bool has_focus = false;
        bool is_selected = false;
    };

    struct TreeNode
    {
        bool state[2];
        std::string name;
        std::vector< WidgetHandle > elements;
        std::vector< TreeNode > children;
        ui::TextFieldState text_state;
    };

    typedef std::map< void*, ui::TreeNode > Tree;
}

namespace kege::ui{

    struct Dock;

    enum DockZone {
        CENTER,
        LEFT,
        RIGHT,
        TOP,
        BOTTOM,
        TAB,
        FLOATING
    };

    struct TabElem
    {
        ui::WidgetHandle uids[3];
        ui::Text text;
    };

    struct Tab
    {
        /**
         * panels: hold the list of panels in this dock
         */
        std::vector< int > panel_indices;
        std::vector< TabElem > list;
        int selection = 0;
        WidgetHandle uid;
    };

    struct ResizeHandler
    {
        Dock* dock;
    };

    struct GhostObject
    {
        Rect rect;
        bool visible = false;
        bool dragging = false;
        int tab_selection;
        ui::Dock* dock;
    };

    struct GhostParam
    {
        GhostObject* ghost;
    };

    struct GhostDropoff
    {
        GhostObject* ghost;
        Dock* dock;
    };

    enum struct DropZone
    {
        NONE,
        LEFT,
        RIGHT,
        TOP,
        BOTTOM,
        CENTER  // For tab merging
    };

}

#endif // !editor_dependencies_hpp

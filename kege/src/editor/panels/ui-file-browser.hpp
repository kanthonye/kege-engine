//
//  file-browser.hpp
//  ui
//
//  Created by Kenneth Esdaile on 8/11/25.
//

#ifndef file_browser_hpp
#define file_browser_hpp

#include "ui-panel.hpp"

namespace kege::ui{

    namespace fs = std::filesystem;

    class FileBrowser : public ui::Panel
    {
    public:

        struct HotState{ bool* hot; };

        struct FileEntry
        {
            ui::UID uid;
            ui::WidgetId widget_id;

            ui::Text name_text;
            fs::path path;
            std::string name;
            std::string display_name;
            bool is_directory;
            uint64_t size; // in bytes
            std::chrono::system_clock::time_point modified_time;
            std::string extension;
            bool selected = false;

            // For drag and drop
            bool being_dragged = false;
        };

        struct NavigationHistory
        {
            std::vector<fs::path> history;
            size_t current_index = 0;

            void push(const fs::path& path)
            {
                // Remove any future history if we're navigating to a new path
                if (current_index + 1 < history.size())
                {
                    history.resize(current_index + 1);
                }
                history.push_back(path);
                current_index = history.size() - 1;
            }

            bool can_go_back() const { return current_index > 0; }
            bool can_go_forward() const { return current_index + 1 < history.size(); }

            fs::path go_back()
            {
                if (can_go_back())
                {
                    current_index--;
                    return history[current_index];
                }
                return history[current_index];
            }

            fs::path go_forward()
            {
                if (can_go_forward())
                {
                    current_index++;
                    return history[current_index];
                }
                return history[current_index];
            }
        };

    public:

        FileBrowser(kege::ui::DockManager* dm, const std::string& path);
        void updateLayout()override;

    private:

        void refreshDirectory();

        void applyFilter();

        void updateNavigationBar();

        void updateFileGrid();

        void updateSelectionBox();

        void updateTooltips();

        void startDragOperation(size_t initial_index);

        void completeDragOperation();

        void updateDragAndDrop();

        void updateContextMenu();

        void handleFileClick(size_t index, uint32_t modifiers);

        void updateSelectionFromBox();

        void clearSelection();

        void navigateTo(const fs::path& path);

        void createNewFolder();

        void moveFilesToFolder(const std::vector<size_t>& indices, const fs::path& target_folder);

        void processPendingOperations();

        std::string formatFileSize(uint64_t size);

    private:

        fs::path _current_path;
        NavigationHistory _history;

        std::string _current_path_str;

        // UI element IDs (persistent across frames)
        ui::UID _uid_main;
        ui::WidgetId _id_main_widget;

        ui::WidgetHandle _up_button_uid;
        ui::WidgetHandle _back_button_uid;
        ui::WidgetHandle _forward_button_uid;
        ui::WidgetHandle _refresh_button_uid;
        ui::WidgetHandle _new_folder_button_uid;
        ui::WidgetHandle _tool_tip_button_uid;
        uint64_t _path_field_id;


        // File list and state
        std::vector<FileEntry> _files;
        std::vector<FileEntry> _filtered_files; // Files after applying filter

        // Selection state
        ui::Rect _selection_box;
        kege::dvec2 _selection_start;
        std::vector<size_t> _selected_indices;

        // Drag and drop state
        bool _begin_dragging = false;
        bool _is_dragging = false;
        kege::dvec2 _drag_start;
        std::vector<size_t> _dragged_indices;
        fs::path _drop_target_folder;

        // Filter state
        std::string _filter_text;
        char _filter_buffer[64];
        bool _show_hidden = false;

        // Grid layout parameters
        static constexpr float ICON_SIZE = 80.0f;
        static constexpr float ICON_SPACING = 10.0f;
        static constexpr float TEXT_HEIGHT = 20.0f;
        static constexpr int COLUMNS = 6; // Will be calculated based on available width

        // Tooltip state
        std::optional<size_t> _hovered_file_index;
        kege::dvec2 _tooltip_position;

        // Pending operations
        struct PendingOperation
        {
            enum Type { RENAME, DELETE, CREATE_FOLDER, MOVE };
            Type type;
            fs::path source;
            fs::path target;
            std::string new_name;
        };
        std::vector<PendingOperation> _pending_operations;

        // Cache for directory contents
        std::unordered_map<std::string, std::vector<FileEntry>> _directory_cache;
        std::chrono::system_clock::time_point _last_cache_time;

        kege::ui::Style _spacer;
        std::string _tooltip_text;
        bool _show_tool_tip;

        bool _deselect_selection_box = false;
        bool _is_selecting = false;
        size_t _curr_selection;
        bool _button_down;
        bool _hot;
    };

}
#endif /* file_browser_hpp */

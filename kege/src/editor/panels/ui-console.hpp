//
//  console.hpp
//  editor
//
//  Created by Kenneth Esdaile on 1/17/26.
//

#ifndef console_hpp
#define console_hpp

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include "ui-panel.hpp"

namespace kege::ui{

    enum class LogLevel
    {
        Debug,
        INFO,
        WARNING,
        ERROR,
        COMMAND,  // Special level for command input/output
        SYSTEM    // Special level for system messages
    };

    struct LogEntry
    {
        std::string message;
        LogLevel level;
        std::chrono::system_clock::time_point timestamp;
        size_t line_number;

        // Color for this log level (RGBA hex)
        uint32_t color() const;

        std::string timestamp_string() const;

        std::string level_string() const;
    };

    class ConsoleCommand
    {
    public:

        using ExecuteFunc = std::function<std::string(const std::vector<std::string>&)>;

        ConsoleCommand
        (
            const std::string& name,
            const std::string& description,
            const std::string& usage,
            ExecuteFunc execute_func
        );

        std::string name() const { return _name; }
        std::string description() const { return _description; }
        std::string usage() const { return _usage; }

        std::string execute(const std::vector<std::string>& args);

    private:

        std::string _name;
        std::string _description;
        std::string _usage;
        ExecuteFunc _execute_func;
    };

    // Custom streambuf that forwards to Console
    class ConsoleStreambuf : public std::streambuf
    {
    public:

        ConsoleStreambuf(std::ostream& original_stream, std::function<void(const std::string&, LogLevel)> callback);
        ~ConsoleStreambuf() override;

    protected:

        std::streamsize xsputn(const char* s, std::streamsize n) override;
        int overflow(int c) override;
        int sync() override;

    private:
        
        std::ostream& _original_stream;
        std::streambuf* _original_buf;
        std::function<void(const std::string&, LogLevel)> _callback;
    };

}



namespace kege::ui{

    class Console : public ui::Panel
    {
    public:

        Console( kege::ui::DockManager* dm );

        ~Console() override;

        void updateLayout()override;

        // Public API for adding logs programmatically
        void log(const std::string& message, LogLevel level = LogLevel::INFO);

        void debug(const std::string& message);
        void info(const std::string& message);
        void warning(const std::string& message);
        void error(const std::string& message);

        // Command registration
        void registerCommand(const std::string& name,
                            const std::string& description,
                            const std::string& usage,
                            ConsoleCommand::ExecuteFunc execute_func);

        // Execute a command string
        std::string executeCommand(const std::string& command_str);

    private:

        void setupOutputRedirection();

        void processPendingLogs();

        void addLogEntry(const std::string& message, LogLevel level);

        void updateVisibleIndices();

        void updateControls();

        void updateLogArea();

        void updateInputField();

        void updateAutoScroll();

        void clearLogs();

        void exportLogs();

        std::vector<std::string> parseCommand(const std::string& command_str);

        void registerBuiltinCommands();

    private:

        // Log storage
        std::vector<LogEntry> _log_entries;
        std::vector<size_t> _visible_indices;  // Indices of visible entries after filtering
        std::mutex _log_mutex;

        // Pending logs from other threads
        std::vector<std::pair<std::string, LogLevel>> _pending_logs;
        std::mutex _pending_mutex;

        // Command system
        std::unordered_map<std::string, std::unique_ptr<ConsoleCommand>> _commands;
        std::mutex _command_mutex;


        ui::UID _uid;

        // UI state
        ui::WidgetHandle _log_area_uid;
        ui::WidgetHandle _input_field_uid;
        ui::WidgetHandle _clear_button_uid;
        ui::WidgetHandle _export_button_uid;
        ui::WidgetHandle _scroll_container_uid;

        // Filter UIDs
        ui::WidgetHandle _filter_debug_uid;
        ui::WidgetHandle _filter_info_uid;
        ui::WidgetHandle _filter_warning_uid;
        ui::WidgetHandle _filter_error_uid;
        ui::WidgetHandle _filter_command_uid;
        ui::WidgetHandle _filter_system_uid;
        ui::WidgetHandle _auto_scroll_uid;

        // Filter state
        std::unordered_map<LogLevel, bool> _filter_enabled;
        std::string _search_text;
        bool _auto_scroll = true;
        bool _new_logs_available = false;

        // Input state
        std::string _input_buffer;
        size_t _input_cursor_pos = 0;

        // Output redirection
        std::unique_ptr<ConsoleStreambuf> _cout_buf;
        std::unique_ptr<ConsoleStreambuf> _cerr_buf;

        // Performance
        static constexpr size_t MAX_LOG_ENTRIES = 1000;
        uint32_t _frame_counter = 0;
    };

}
#endif /* console_hpp */

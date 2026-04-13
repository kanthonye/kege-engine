//
//  console.cpp
//  editor
//
//  Created by Kenneth Esdaile on 1/17/26.
//

#include "../editor-layer.hpp"
#include "ui-console.hpp"

namespace kege::ui{

    uint32_t LogEntry::color() const
    {
        switch (level)
        {
            case LogLevel::Debug:    return 0x888888FF;   // Gray
            case LogLevel::INFO:     return 0xFFFFFFFF;   // White
            case LogLevel::WARNING:  return 0xFFFF00FF;   // Yellow
            case LogLevel::ERROR:    return 0xFF0000FF;   // Red
            case LogLevel::COMMAND:  return 0x00FFFFFF;   // Cyan
            case LogLevel::SYSTEM:   return 0x00FF00FF;   // Green
            default:                 return 0xFFFFFFFF;
        }
    }

    std::string LogEntry::timestamp_string() const
    {
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        std::tm* tm = std::localtime(&time_t);

        char buffer[64];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);

        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            timestamp.time_since_epoch()) % 1000;

        std::string result = buffer;
        result += "." + std::to_string(ms.count());
        return result;
    }

    std::string LogEntry::level_string() const
    {
        switch (level)
        {
            case LogLevel::Debug:   return "DEBUG";
            case LogLevel::INFO:    return "INFO";
            case LogLevel::WARNING: return "WARN";
            case LogLevel::ERROR:   return "ERROR";
            case LogLevel::COMMAND: return "CMD";
            case LogLevel::SYSTEM:  return "SYS";
            default:                return "UNKNOWN";
        }
    }
}


namespace kege::ui{

    ConsoleCommand::ConsoleCommand(const std::string& name,
                       const std::string& description,
                       const std::string& usage,
                       ExecuteFunc execute_func)
            : _name(name)
            , _description(description)
            , _usage(usage)
            , _execute_func(execute_func) {}

    std::string ConsoleCommand::execute(const std::vector<std::string>& args) {
        if (_execute_func) {
            return _execute_func(args);
        }
        return "Command execution failed: No execute function";
    }
}



namespace kege::ui{

    ConsoleStreambuf::ConsoleStreambuf(std::ostream& original_stream,
                     std::function<void(const std::string&, LogLevel)> callback)
        : _original_stream(original_stream)
        , _original_buf(original_stream.rdbuf())
        , _callback(callback) {
        original_stream.rdbuf(this);
    }

    ConsoleStreambuf::~ConsoleStreambuf()
    {
        _original_stream.rdbuf(_original_buf);
    }

    std::streamsize ConsoleStreambuf::xsputn(const char* s, std::streamsize n)
    {
        std::string msg(s, n);

        // Forward to original stream
        _original_buf->sputn(s, n);

        // Call callback if set
        if (_callback && !msg.empty()) {
            // Determine log level from stream type
            LogLevel level = LogLevel::INFO;
            if (&_original_stream == &std::cerr) {
                level = LogLevel::ERROR;
            }

            _callback(msg, level);
        }

        return n;
    }

    int ConsoleStreambuf::overflow(int c)
    {
        if (c != EOF)
        {
            char ch = static_cast<char>(c);

            // Forward to original stream
            _original_buf->sputc(ch);

            // Call callback if set
            if (_callback) {
                std::string msg(1, ch);
                LogLevel level = LogLevel::INFO;
                if (&_original_stream == &std::cerr) {
                    level = LogLevel::ERROR;
                }
                _callback(msg, level);
            }
        }
        return c;
    }

    int ConsoleStreambuf::sync()
    {
        return _original_buf->pubsync();
    }

}


namespace kege::ui{

    Console::Console( kege::ui::DockManager* dm )
    :   ui::Panel( "Console", dm )
    {
        // Generate persistent UIDs
//            _log_area_uid = _ui->layout()->generateUID();
//            _input_field_uid = _ui->layout()->generateUID();
//            _clear_button_uid = _ui->layout()->generateUID();
//            _export_button_uid = _ui->layout()->generateUID();
//            _scroll_container_uid = _ui->layout()->generateUID();
//
//            // Generate filter UIDs
//            _filter_debug_uid = _ui->layout()->generateUID();
//            _filter_info_uid = _ui->layout()->generateUID();
//            _filter_warning_uid = _ui->layout()->generateUID();
//            _filter_error_uid = _ui->layout()->generateUID();
//            _filter_command_uid = _ui->layout()->generateUID();
//            _filter_system_uid = _ui->layout()->generateUID();

        // Initialize filter states (all enabled by default)
        _filter_enabled[LogLevel::Debug] = true;
        _filter_enabled[LogLevel::INFO] = true;
        _filter_enabled[LogLevel::WARNING] = true;
        _filter_enabled[LogLevel::ERROR] = true;
        _filter_enabled[LogLevel::COMMAND] = true;
        _filter_enabled[LogLevel::SYSTEM] = true;

        // Setup output redirection
        setupOutputRedirection();

        // Register built-in commands
        registerBuiltinCommands();

        // Add initial system message
        addLogEntry("Console initialized", LogLevel::SYSTEM);
    }

    Console::~Console()
    {
        // Clean up streambufs
        _cout_buf.reset();
        _cerr_buf.reset();
    }

    void Console::updateLayout()
    {
        // Process any pending log entries from other threads
        processPendingLogs();

        // Update UI
        updateControls();
        updateLogArea();
//        updateInputField();
//
//        // Update auto-scroll state
//        updateAutoScroll();

        // Increment frame counter for unique IDs
        _frame_counter++;
    }

    // Public API for adding logs programmatically
    void Console::log(const std::string& message, LogLevel level)
    {
        std::lock_guard<std::mutex> lock(_log_mutex);
        addLogEntry(message, level);
    }

    void Console::debug(const std::string& message) { log(message, LogLevel::Debug); }
    void Console::info(const std::string& message) { log(message, LogLevel::INFO); }
    void Console::warning(const std::string& message) { log(message, LogLevel::WARNING); }
    void Console::error(const std::string& message) { log(message, LogLevel::ERROR); }

    // Command registration
    void Console::registerCommand(const std::string& name,
                        const std::string& description,
                        const std::string& usage,
                        ConsoleCommand::ExecuteFunc execute_func)
    {
        std::lock_guard<std::mutex> lock(_command_mutex);
        _commands[name] = std::make_unique<ConsoleCommand>(name, description, usage, execute_func);
    }

    // Execute a command string
    std::string Console::executeCommand(const std::string& command_str)
    {
        // Log the command input
        addLogEntry("#" + command_str, LogLevel::COMMAND);

        // Parse command
        std::vector<std::string> args = parseCommand(command_str);
        if (args.empty()) {
            return "Empty command";
        }

        std::string command_name = args[0];
        args.erase(args.begin());  // Remove command name from args

        // Find and execute command
        std::lock_guard<std::mutex> lock(_command_mutex);
        auto it = _commands.find(command_name);
        if (it != _commands.end()) {
            std::string result = it->second->execute(args);
            addLogEntry(result, LogLevel::COMMAND);
            return result;
        } else {
            std::string error_msg = "Unknown command: #" + command_name;
            addLogEntry(error_msg, LogLevel::ERROR);
            return error_msg;
        }
    }

    void Console::setupOutputRedirection() {
        // Redirect cout
        _cout_buf = std::make_unique<ConsoleStreambuf>(std::cout,
            [this](const std::string& msg, LogLevel level) {
                std::lock_guard<std::mutex> lock(_pending_mutex);
                _pending_logs.emplace_back(msg, level);
            });

        // Redirect cerr
        _cerr_buf = std::make_unique<ConsoleStreambuf>(std::cerr,
            [this](const std::string& msg, LogLevel level) {
                std::lock_guard<std::mutex> lock(_pending_mutex);
                _pending_logs.emplace_back(msg, LogLevel::ERROR);
            });
    }

    void Console::processPendingLogs() {
        std::vector<std::pair<std::string, LogLevel>> pending;
        {
            std::lock_guard<std::mutex> lock(_pending_mutex);
            pending.swap(_pending_logs);
        }

        std::lock_guard<std::mutex> lock(_log_mutex);
        for (const auto& [msg, level] : pending) {
            addLogEntry(msg, level);
        }
    }

    void Console::addLogEntry(const std::string& message, LogLevel level) {
        LogEntry entry;
        entry.message = message;
        entry.level = level;
        entry.timestamp = std::chrono::system_clock::now();
        entry.line_number = _log_entries.size();

        _log_entries.push_back(entry);

        // Trim if necessary
        if (_log_entries.size() > MAX_LOG_ENTRIES) {
            _log_entries.erase(_log_entries.begin(),
                             _log_entries.begin() + (_log_entries.size() - MAX_LOG_ENTRIES));
        }

        // Notify that new logs are available
        if (!_auto_scroll) {
            _new_logs_available = true;
        }

        // Update visible indices
        updateVisibleIndices();
    }

    void Console::updateVisibleIndices() {
        _visible_indices.clear();

        for (size_t i = 0; i < _log_entries.size(); i++)
        {
            const auto& entry = _log_entries[i];

            // Apply level filter
            if (!_filter_enabled[entry.level]) {
                continue;
            }

            // Apply search filter
            if (!_search_text.empty()) {
                std::string msg_lower = entry.message;
                std::string search_lower = _search_text;
                std::transform(msg_lower.begin(), msg_lower.end(), msg_lower.begin(), ::tolower);
                std::transform(search_lower.begin(), search_lower.end(), search_lower.begin(), ::tolower);

                if (msg_lower.find(search_lower) == std::string::npos) {
                    continue;
                }
            }

            _visible_indices.push_back(i);
        }
    }

    void Console::updateControls()
    {
        // Start controls row
        _ui->beginRow(0);

        // Clear button
        ui::Text text;
        text.color = 0xFFFFFFFF;
        text.font_size = 15;
        text.height = 15;

        text.width = 50;
        text.ptr = "Clear";
        if (_ui->button(_uid[0], text, &_ui->theme().button)) {
            clearLogs();
        }

        // Export button
        text.width = 50;
        text.ptr = "Export";
        if (_ui->button(_uid[1], text, &_ui->theme().button))
        {
            exportLogs();
        }

        // Search field
        char search_buffer[256] = "";
        strncpy(search_buffer, _search_text.c_str(), sizeof(search_buffer));
//        size_t search_size = _search_text.size() + 1;
//        uint64_t search_id = _ui->getAddressAsInt(&_search_text) + _frame_counter;
//
//        if (_ui->textField(0, search_id, search_buffer, search_size)) {
//            _search_text = search_buffer;
//            updateVisibleIndices();
//        }

        _ui->endRow();

        // Filter checkboxes row
        _ui->beginRow(0);

        // Debug filter
        text.width = 50;
        text.ptr = "Debug";
        if (_ui->radio(_uid[2], text, _filter_enabled[LogLevel::Debug]))
        {
            //updateVisibleIndices();
        }

        // Info filter
        text.width = 40;
        text.ptr = "Info";
        if (_ui->radio(_uid[3], text, _filter_enabled[LogLevel::INFO]))
        {
            //updateVisibleIndices();
        }

        // Warning filter
        text.width = 50;
        text.ptr = "Warning";
        if (_ui->radio(_uid[4], text, _filter_enabled[LogLevel::WARNING]))
        {
            //updateVisibleIndices();
        }

        // Error filter
        text.width = 50;
        text.ptr = "Error";
        if (_ui->radio(_uid[5], text, _filter_enabled[LogLevel::ERROR]))
        {
            //updateVisibleIndices();
        }

        // Command filter
        text.width = 50;
        text.ptr = "Cmd";
        if (_ui->radio(_uid[6], text, _filter_enabled[LogLevel::COMMAND]))
        {
            //updateVisibleIndices();
        }

        // System filter
        text.width = 50;
        text.ptr = "Sys";
        if (_ui->radio(_uid[0], text, _filter_enabled[LogLevel::SYSTEM]))
        {
            //updateVisibleIndices();
        }

        // Auto-scroll toggle
        text.width = 50;
        text.ptr = _auto_scroll ? "Auto" : "Manual";
        if (_ui->button(_uid[0], text, &_ui->theme().button))
        {
            _auto_scroll = !_auto_scroll;
            if (_auto_scroll)
            {
                _new_logs_available = false;
            }
        }

        _ui->endRow();

        updateVisibleIndices();
    }

    void Console::updateLogArea()
    {
        ui::ID id[2] = {_uid[0], _uid[1]};
        // Begin scroll container for log area
        _ui->beginScrollContainer(id);

        // Set special style if new logs are available while not auto-scrolling
        const ui::Style* container_style = &_ui->theme().panel;
        if (_new_logs_available && !_auto_scroll)
        {
            // TODO: You might want to create a special style for this state
            // For now, we'll just modify the border color
            container_style = &_ui->theme().button;  // Temporary, you should create proper style
        }

        // Display visible log entries
        std::lock_guard<std::mutex> lock(_log_mutex);

        for (size_t visible_idx : _visible_indices)
        {
            const LogEntry& entry = _log_entries[visible_idx];

            // Create formatted log line
            std::stringstream line;
            line << "[" << entry.timestamp_string() << "] "
                 << "[" << entry.level_string() << "] "
                 << entry.message;

            // Generate unique WidgetHandle for this log line
            //ui::WidgetHandle line_uid = _ui->layout()->generateUID();

            if ( line.str().empty()) continue;
            if ( line.str()[0] < 32 ) continue;

            // Create widget description
            ui::WidgetDesc line_desc;
            //line_desc.user_id = &line_uid;
            line_desc.text.width = 600;
            line_desc.text.font_size = 18;
            line_desc.text.color = 0xFFFFFFFF;
            line_desc.text.ptr = line.str().c_str();
            //line_desc.text = _ui->layout()->text(line.str().c_str(), 12);
            line_desc.color = entry.color();
            line_desc.style = &_ui->theme().label;

            // Highlight if search text matches
            if (!_search_text.empty())
            {
                std::string msg_lower = entry.message;
                std::string search_lower = _search_text;
                std::transform(msg_lower.begin(), msg_lower.end(), msg_lower.begin(), ::tolower);
                std::transform(search_lower.begin(), search_lower.end(), search_lower.begin(), ::tolower);

                if (msg_lower.find(search_lower) != std::string::npos)
                {
                    // TODO: You might want to create a highlight style
                    line_desc.color = 0xFFFFFF00;  // Yellow highlight
                }
            }

            _ui->put(line_desc);
        }

        _ui->endScrollContainer();
    }

    void Console::updateInputField()
    {
        // Input field at the bottom
        _ui->beginRow(0);

        // Command prompt
        _ui->label(_ui->layout()->text("#", 20));

        // Command input field
        char input_buffer[256] = "";
        strncpy(input_buffer, _input_buffer.c_str(), sizeof(input_buffer));
//        size_t input_size = _input_buffer.size() + 1;
//
//        if (_ui->textField(0, _ui->getAddressAsInt(*this), input_buffer, input_size)) {
//            _input_buffer = input_buffer;
//        }

        // Execute button
        ui::WidgetHandle execute_uid;// = _ui->layout()->generateUID();
        ui::Text execute_text = _ui->layout()->text("Execute", 20);
        if (_ui->button(_uid[0], execute_text, &_ui->theme().button)) {
            if (!_input_buffer.empty()) {
                executeCommand(_input_buffer);
                _input_buffer.clear();
            }
        }

        _ui->endRow();
    }

    void Console::updateAutoScroll() {
        // TODO: Check scroll position and update auto-scroll state
        // This would require accessing the scroll container's state
        // For now, we'll implement a simple version

        if (_auto_scroll) {
            // TODO: Scroll to bottom
            // This would require calling a method on the scroll container
            // _ui->layout()->scrollToBottom(_scroll_container_uid);
        }

        // Check if user scrolled manually
        // This would require checking the scroll container's scroll position
        // and comparing it to the maximum scroll position
    }

    void Console::clearLogs() {
        std::lock_guard<std::mutex> lock(_log_mutex);
        _log_entries.clear();
        _visible_indices.clear();
        addLogEntry("Logs cleared", LogLevel::SYSTEM);
    }

    void Console::exportLogs()
    {
        std::string filename = "console_log_" +
                              std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) +
                              ".txt";

        std::ofstream file(filename);
        if (!file.is_open())
        {
            addLogEntry("Failed to export logs: Cannot open file", LogLevel::ERROR);
            return;
        }

        std::lock_guard<std::mutex> lock(_log_mutex);
        for (const auto& entry : _log_entries)
        {
            file << "[" << entry.timestamp_string() << "] "
                 << "[" << entry.level_string() << "] "
                 << entry.message << "\n";
        }

        file.close();
        addLogEntry("Logs exported to: " + filename, LogLevel::SYSTEM);
    }

    std::vector<std::string> Console::parseCommand(const std::string& command_str)
    {
        std::vector<std::string> args;
        std::string current_arg;
        bool in_quotes = false;
        bool escape_next = false;

        for (char c : command_str) {
            if (escape_next) {
                current_arg += c;
                escape_next = false;
            } else if (c == '\\') {
                escape_next = true;
            } else if (c == '"') {
                in_quotes = !in_quotes;
                if (!in_quotes && !current_arg.empty()) {
                    args.push_back(current_arg);
                    current_arg.clear();
                }
            } else if (std::isspace(c) && !in_quotes) {
                if (!current_arg.empty()) {
                    args.push_back(current_arg);
                    current_arg.clear();
                }
            } else {
                current_arg += c;
            }
        }

        if (!current_arg.empty()) {
            args.push_back(current_arg);
        }

        return args;
    }

    void Console::registerBuiltinCommands()
    {
        // Clear command
        registerCommand("clear", "Clear the console", "#clear",
            [this](const std::vector<std::string>& args) -> std::string {
                clearLogs();
                return "Console cleared";
            });

        // Help command
        registerCommand("help", "Show help for commands", "#help [command_name]",
            [this](const std::vector<std::string>& args) -> std::string {
                std::lock_guard<std::mutex> lock(_command_mutex);

                if (args.empty()) {
                    // List all commands
                    std::string result = "Available commands:\n";
                    for (const auto& [name, cmd] : _commands) {
                        result += "  #" + cmd->name() + " - " + cmd->description() + "\n";
                    }
                    result += "Use '#help <command>' for more information";
                    return result;
                } else {
                    // Show help for specific command
                    std::string cmd_name = args[0];
                    auto it = _commands.find(cmd_name);
                    if (it != _commands.end()) {
                        return "Command: #" + it->second->name() + "\n" +
                               "Description: " + it->second->description() + "\n" +
                               "Usage: " + it->second->usage();
                    } else {
                        return "Unknown command: #" + cmd_name;
                    }
                }
            });

        // Echo command
        registerCommand("echo", "Echo arguments back", "#echo <message>",
            [](const std::vector<std::string>& args) -> std::string {
                std::string result;
                for (const auto& arg : args) {
                    if (!result.empty()) result += " ";
                    result += arg;
                }
                return result;
            });

        // Log test command
        registerCommand("logtest", "Test different log levels", "#logtest",
            [this](const std::vector<std::string>& args) -> std::string {
                debug("This is a debug message");
                info("This is an info message");
                warning("This is a warning message");
                error("This is an error message");
                return "Test messages logged";
            });

        // List commands (alias for help)
        registerCommand("list", "List all commands", "#list",
            [this](const std::vector<std::string>& args) -> std::string {
                return executeCommand("help");
            });
    }
}

// namespace kege

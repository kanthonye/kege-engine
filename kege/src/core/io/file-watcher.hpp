//
//  file-watcher.hpp
//  kege
//
//  Created by Kenneth Esdaile on 11/2/24.
//

#ifndef file_watcher_hpp
#define file_watcher_hpp

#include <iostream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <functional>
#include <string>

namespace kege{

    enum class FileStatus {Created, Modified, Deleted};

    /**
     * @class FileWatcher
     * @brief Monitors a specified directory for changes, including file creation, modification, and deletion.
     *
     * This class provides a simple interface to watch for changes in a directory.
     * It uses `std::filesystem` to track files' last modification times and reports changes
     * using a callback function. This implementation checks for changes at specified intervals,
     * making it cross-platform and straightforward.
     */
    class FileWatcher {
    public:

        typedef std::filesystem::path Path;

        /**
         * @typedef FileChangeCallback
         * @brief A callback type that is invoked on file changes.
         *
         * The callback receives the file path and a string describing the change type ("Created", "Modified", "Deleted").
         */
        using FileChangeCallback = std::function< void( const FileWatcher::Path&, const FileStatus& ) >;

        /**
         * @brief Constructs a FileWatcher for a specific directory with a delay interval.
         *
         * @param watchDir The directory to monitor.
         * @param delay The interval between checks for changes, e.g., `std::chrono::milliseconds(500)`.
         */
        FileWatcher(const FileWatcher::Path& watchDir, std::chrono::milliseconds delay);

        /**
         * @brief Starts the file watching loop.
         *
         * This function continuously checks for changes in the watched directory and calls the callback function
         * whenever a file is created, modified, or deleted.
         *
         * @param callback The function to call when a file change is detected.
         */
        void start( const FileChangeCallback& callback );

        /**
         * @brief Stops the file watching loop.
         *
         * This function allows for graceful stopping of the file watcher by setting `running_` to false.
         */
        void stop();

    private:

        /**
         * @brief Checks for changes in the directory and invokes the callback if any are found.
         *
         * This function iterates through the directory, comparing each file's last modification time
         * to the stored time. If a file is created, modified, or deleted, the callback is called.
         *
         * @param callback The function to call for each detected change.
         */
        void checkForChanges(const FileChangeCallback& callback);

        /**
         * @brief Initializes the file tracking map with existing files in the directory.
         *
         * This function is called in the constructor to populate `filesLastWriteTime_` with the initial
         * set of files and their modification times.
         */
        void initializeFiles();

    private:

        /**
         * @brief Tracks last known modification time for each file in the watched directory.
         *
         * Maps file paths to their last modification times to detect changes over time.
         */
        std::unordered_map<std::string, std::filesystem::file_time_type> _files_last_write_time;

        std::chrono::milliseconds _delay; // Delay interval between checks for changes.
        FileWatcher::Path _watch_directory; // The directory to watch for changes.
        bool _running = false; // Control flag for the file watcher loop.

    };
}
#endif /* file_watcher_hpp */

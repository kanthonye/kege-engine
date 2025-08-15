//
//  file-watcher.cpp
//  kege
//
//  Created by Kenneth Esdaile on 11/2/24.
//

#include "file-watcher.hpp"


namespace kege{

    FileWatcher::FileWatcher(const FileWatcher::Path& watchDir, std::chrono::milliseconds delay)
    :   _watch_directory(watchDir)
    ,   _delay(delay)
    {
        initializeFiles();
    }

    void FileWatcher::start( const FileChangeCallback& callback )
    {
        int count = 0;
        _running = true;
        while ( _running )
        {
            std::this_thread::sleep_for( _delay );
            checkForChanges( callback );
            count++;
            if(count >= 200 ) _running = false;
        }
    }

    void FileWatcher::stop()
    {
        _running = false;
    }

    void FileWatcher::initializeFiles()
    {
        for ( const auto& file : std::filesystem::recursive_directory_iterator( _watch_directory ) )
        {
            if ( std::filesystem::is_regular_file( file ) )
            {
                _files_last_write_time[ file.path().string() ] = std::filesystem::last_write_time( file );
            }
        }
    }

    void FileWatcher::checkForChanges(const FileChangeCallback& callback)
    {
        for ( const auto& file : std::filesystem::recursive_directory_iterator( _watch_directory ) )
        {
            const auto& path = file.path().string();
            auto current_last_write_time = std::filesystem::last_write_time(file);

            // File creation or modification
            if ( _files_last_write_time.find(path) == _files_last_write_time.end() )
            {
                _files_last_write_time[path] = current_last_write_time;
                callback( file.path(), FileStatus::Created );
            }
            else if ( _files_last_write_time[path] != current_last_write_time )
            {
                _files_last_write_time[path] = current_last_write_time;
                callback( file.path(), FileStatus::Modified );
            }
        }

        // Detect file deletion
        auto it = _files_last_write_time.begin();
        while (it != _files_last_write_time.end())
        {
            if ( !std::filesystem::exists( it->first ) )
            {
                callback( it->first, FileStatus::Deleted );
                it = _files_last_write_time.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

}



//int main() 
//{
//    // Directory to watch and delay between checks
//    kege::FileWatcher watcher( "/Users/kae/Downloads", std::chrono::milliseconds(500) );
//
//    watcher.start
//    (
//     []( const kege::FileWatcher::Path& file, const kege::FileStatus& action)
//     {
//         if (kege::FileStatus::Created == action )
//         {
//             std::cout << "File " << file.string() << " was Created" << std::endl;
//         }
//         else if (kege::FileStatus::Deleted == action )
//         {
//             std::cout << "File " << file.string() << " was Deleted" << std::endl;
//         }
//         else if (kege::FileStatus::Modified == action )
//         {
//             std::cout << "File " << file.string() << " was Modified" << std::endl;
//         }
//     }
//     );
//
//    return 0;
//}

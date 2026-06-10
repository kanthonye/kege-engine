/**
 * @file frames.hpp
 * @brief Container to hold N number of resource in flight
 *
 * @author Kenneth Esdaile
 * @date November 6, 2025
 * @version 1.0
 *
 * @copyright Copyright (c) 2025 KEGE. All rights reserved.
 *
 * @details
 * This class provides a RAII-based smart pointer for automatic resource
 * management. It handles ownership, copying, and destruction of allocated
 * resources while maintaining type safety.
 *
 * Usage example:
 * @code
 * Handle handle = asset_manager->add< Frames< Image > >("color", {{Image(),Image(),Image()}, 3});
 * if (handle == 0) return error;
 * 
 * Frames< Image >* frame_images = asset_manager->get< Image >( handle );
 * Image& image = frame_images->get(frame_index);
 * @endcode
 */
#ifndef kege_frames_hpp
#define kege_frames_hpp

#include <stdlib.h>
#include <algorithm>
#include <initializer_list>

namespace kege
{
//    template<typename T> struct Frames
//    {
//        Frames(const std::initializer_list<T>& a)
//        :   frames( static_cast< int >( a.size() ) )
//        {
//            size_t size = (a.size() > kege::MAX_FRAMES_IN_FLIGHT)
//            ? kege::MAX_FRAMES_IN_FLIGHT
//            : a.size();
//
//            const T* data = a.begin();
//            for (int i = 0; i < size; ++i) sources[i] = data[i];
//        }
//
//        Frames(): frames( 0 ) {}
//
//        T& get(int i){ return sources[ i % frames ]; }
//
//        T sources[ kege::MAX_FRAMES_IN_FLIGHT ];
//        uint32_t frames;
//    };
}
#endif // kege_frames_hpp

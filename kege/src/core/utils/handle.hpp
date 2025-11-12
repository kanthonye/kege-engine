/**
 * @file Handle.hpp
 * @brief Smart pointer implementation for resource management
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
 * Features:
 * - Automatic memory management
 * - Copy semantics with reference counting
 * - Null state handling
 * - Type-safe resource access
 *
 * Usage example:
 * @code
 * Handle handle = asset_manager->add< Image >("image.png", new Image{});
 * if (handle == 0) {
 *     return error;
 * }
 * Image* img = asset_manager->get< Image >( handle );
 * @endcode
 */
#ifndef kege_handle_hpp
#define kege_handle_hpp
namespace kege
{
    union Handle
    {
        struct
        {
            uint32_t index;
            uint16_t version;
            uint16_t type;
        };
        uint64_t value;
    };
}
#endif /* kege_handle_hpp */

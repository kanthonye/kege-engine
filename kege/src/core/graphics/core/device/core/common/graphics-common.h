//
//  graphics-enums-and-types.hpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/24/25.
//

/**
 * @file graphics_descriptors.h
 * @brief Core descriptors and structures for the graphics API
 *
 * This file contains comprehensive descriptors for configuring various
 * aspects of a modern graphics pipeline, including swapchains, device features,
 * and descriptor set updates.
 */

#ifndef graphics_enums_and_types_hpp
#define graphics_enums_and_types_hpp

#include <stdio.h>
#include <cstdint>
#include <vector>
#include <string>
#include <memory> // For potential smart pointers later, though using opaque handles initially
#include <variant> // Useful for certain descriptions
#include <functional>
#include <stdexcept>
#include <assert.h>

#include "../../../../../utils/communication.hpp"
#include "../../../../../memory/ref.hpp"
#include "../../../../../utils/log.hpp"
#include "../../../../../utils/array.hpp"
#include "../../../../../math/algebra/vectors.hpp"
#include "../../../../../math/algebra/matrices.hpp"
#include "../../../../../math/algebra/vmath.hpp"

#include "string-to-enum-types.hpp"

namespace kege{

    enum{ MAX_FRAMES_IN_FLIGHT = 2 };

    enum struct ErrorType
    {
        NULL_OBJECT,
        INVALID_OPERATION,
        INVALID_ID,
        UNSUPPORTED_QUEUE_TYPE,
        INVALID_DEVICE_QUEUE_TYPE,
        OUT_OF_BOUND_INDEX,
        FAILED_TO_ACQUIRE_NEXT_IMAGE,
        FAILED_TO_INITALIZED,
        FAILED_TO_SHUTDOWN,
    };

    enum struct Result
    {
        UNKNOWN_ERROR,
        ERROR_DEVICE_LOST,
        SUCCESS,
        INVALID_ID,
        INVALID_OPERATION,
        FAILED_CREATING_OBJECT,
        FAILED_TO_SHUTDOWN,
        OUT_OF_BOUND_INDEX,
        RECREATE_SWAPCHAIN,
    };

}

#endif /* graphics_enums_and_types_hpp */

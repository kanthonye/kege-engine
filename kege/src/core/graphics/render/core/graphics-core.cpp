//
//  graphics-enums-and-types.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/24/25.
//

#include "graphics-core.hpp"

namespace kege{

    /**
     * @brief Convert engine descriptor type to a human-readable C string
     *
     * @param type Engine descriptor type
     * @return Null-terminated C string name of the descriptor type
     */
    const char* descriptorTypeCString( DescriptorType type )
    {
        switch ( type )
        {
            case DescriptorType::CombinedImageSampler: return "CombinedImageSampler";
            case DescriptorType::SampledImage: return "SampledImage";
            case DescriptorType::StorageImage: return "StorageImage";
            case DescriptorType::UniformTexelBuffer: return "UniformTexelBuffer";
            case DescriptorType::StorageTexelBuffer: return "StorageTexelBuffer";
            case DescriptorType::UniformBuffer: return "UniformBuffer";
            case DescriptorType::StorageBuffer: return "StorageBuffer";
            case DescriptorType::UniformBufferDynamic: return "UniformBufferDynamic";
            case DescriptorType::StorageBufferDynamic: return "StorageBufferDynamic";
            case DescriptorType::InputAttachment: return "InputAttachment";
            case DescriptorType::Sampler: return "Sampler";
            default: return "Invalid";
        }
    }

}

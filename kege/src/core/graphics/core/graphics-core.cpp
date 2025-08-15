//
//  graphics-enums-and-types.cpp
//  ecs
//
//  Created by Kenneth Esdaile on 4/24/25.
//

#include "../../graphics/core/graphics-core.hpp"

namespace kege{

    
//    WriteDescriptorSet createUniformBufferWrite
//    (
//        DescriptorSetHandle set,
//        uint32_t binding,
//        BufferHandle buffer,
//        uint64_t offset,
//        uint64_t range
//    )
//    {
//        WriteDescriptorSet write;
//        write.dst_set = set;
//        write.dst_binding = binding;
//        write.descriptor_type = DescriptorType::UniformBuffer;
//        write.buffer_info.push_back({buffer, offset, range});
//        return write;
//    }
//
//    WriteDescriptorSet createImageWrite
//    (
//        DescriptorSetHandle set,
//        uint32_t binding,
//        ImageHandle texture,
//        SamplerHandle sampler
//    )
//    {
//        WriteDescriptorSet write;
//        write.dst_set = set;
//        write.dst_binding = binding;
//        write.descriptor_type = sampler.id ? DescriptorType::CombinedImageSampler : DescriptorType::SampledImage;
//        write.image_info.push_back({sampler, texture});
//        return write;
//    }
//    
}

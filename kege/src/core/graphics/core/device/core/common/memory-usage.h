//
//  memory-usage.h
//  KE-GE
//
//  Created by Kenneth Esdaile on 5/3/26.
//

#ifndef memory_usage_h
#define memory_usage_h

#include <stdlib.h>

namespace kege{

    /**
     * @brief Memory usage patterns for buffer and texture allocations.
     *
     * Defines how memory will be accessed by CPU and GPU, which affects:
     * - Performance characteristics
     * - Memory placement (device-local vs host-visible)
     * - Mapping requirements
     * - Coherency behavior
     */
    enum class MemoryUsage
    {
        /**
         * @brief Device-local memory, optimal for GPU-only resources.
         *
         * Characteristics:
         * - Fastest GPU access (discrete cards: VRAM)
         * - CPU cannot map or access directly
         * - Requires staging buffers for CPU updates
         *
         * Typical uses:
         * - Static vertex/index buffers
         * - Frequently sampled textures
         * - Render targets
         */
        GpuOnly,

        /**
         * @brief Host-visible memory for frequent CPU-to-GPU transfers.
         *
         * Characteristics:
         * - CPU-writable (may be coherent or require explicit flushes)
         * - Slower GPU access than GpuOnly
         * - Often unified memory on integrated GPUs
         *
         * Typical uses:
         * - Dynamic uniform buffers
         * - Frequently updated vertex buffers
         * - Staging buffers for GpuOnly resources
         */
        CpuToGpu,

        /**
         * @brief Host-visible memory for GPU-to-CPU readback.
         *
         * Characteristics:
         * - GPU-writable, CPU-readable
         * - Requires explicit synchronization (map/unmap)
         * - May need cache invalidation on CPU side
         *
         * Typical uses:
         * - Frame capture buffers
         * - Compute shader output
         * - Occlusion query results
         */
        GpuToCpu,

        /**
         * @brief CPU-only allocations (rarely needed for GPU resources).
         *
         * Characteristics:
         * - Not accessible by GPU
         * - May be pageable system memory
         * - Exists mainly for utility allocations
         *
         * Typical uses:
         * - Intermediate processing buffers
         * - Asset loading temporary storage
         * - Debugging helpers
         */
        CpuOnly,

        /**
         * @brief Transient GPU memory for short-lived attachments.
         *
         * Characteristics:
         * - May use lazy allocation
         * - Optimized for renderpass attachments
         * - Implementation may use memory pooling
         *
         * Typical uses:
         * - MSAA color/depth targets
         * - Intermediate render passes
         * - Tile-based rendering buffers
         */
        // GpuLazy,  // Reserved for future expansion
    };

    /**
     * @brief Check if memory usage requires host-visible allocation
     */
    inline constexpr bool isHostVisible(MemoryUsage usage) {
        return usage == MemoryUsage::CpuToGpu ||
               usage == MemoryUsage::GpuToCpu ||
               usage == MemoryUsage::CpuOnly;
    }

    /**
     * @brief Check if memory usage implies frequent CPU writes
     */
    inline constexpr bool isCpuWriteCombined(MemoryUsage usage) {
        return usage == MemoryUsage::CpuToGpu;
    }

    /**
     * @brief Check if memory usage implies CPU readback capability
     */
    inline constexpr bool supportsCpuReadback(MemoryUsage usage) {
        return usage == MemoryUsage::GpuToCpu ||
               usage == MemoryUsage::CpuOnly;
    }

}
#endif /* memory_usage_h */

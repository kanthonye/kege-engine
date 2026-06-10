//
//  access-flags.h
//  KE-GE
//
//  Created by Kenneth Esdaile on 5/3/26.
//

#ifndef access_flags_h
#define access_flags_h

#include <stdlib.h>

namespace kege{

    /**
     * @brief Namespace defining flags for render graph resource access.
     *
     * These flags represent the types of access that are performed on a
     * resource during a particular pipeline stage. They are crucial for
     * ensuring proper synchronization and data coherency between different
     * stages that might be accessing the same resource.
     */
    enum struct AccessFlags: uint32_t
    {
        None = 0,
        IndirectCommandRead = 1 << 0,
        IndexRead           = 1 << 1,
        VertexBufferRead    = 1 << 2,
        UniformRead         = 1 << 3,
        InputRead           = 1 << 4,
        ShaderRead          = 1 << 5,
        ShaderWrite         = 1 << 6,
        ColorRead           = 1 << 7,
        ColorWrite          = 1 << 8,
        DepthStencilRead    = 1 << 9,
        DepthStencilWrite   = 1 << 10,
        TransferRead        = 1 << 11,
        TransferWrite       = 1 << 12,
        HostRead            = 1 << 13,
        HostWrite           = 1 << 14,

        MemoryRead = HostRead | UniformRead | InputRead | ShaderRead |
                     ColorRead | DepthStencilRead | TransferRead |
                     IndirectCommandRead | IndexRead | VertexBufferRead,

        MemoryWrite = HostWrite | ShaderWrite | ColorWrite |
                      DepthStencilWrite | TransferWrite,

        All = 0xFFFFFFFF,
    };

    /**
     * @brief Enables bitwise OR operation for GraphAccessFlags.
     * @param a First flag.
     * @param b Second flag.
     * @return The bitwise OR of the two flags.
     */
    inline AccessFlags operator|(AccessFlags a, AccessFlags b)
    {
        return static_cast< AccessFlags >( static_cast<uint32_t>(a) | static_cast<uint32_t>(b) );
    }

    /**
     * @brief Enables bitwise AND operation for GraphAccessFlags.
     * @param a First flag.
     * @param b Second flag.
     * @return The bitwise AND of the two flags.
     */
    inline AccessFlags operator&(AccessFlags a, AccessFlags b)
    {
        return static_cast<AccessFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    /**
     * @brief Enables bitwise XOR operation for GraphAccessFlags.
     * @param a First flag.
     * @param b Second flag.
     * @return The bitwise XOR of the two flags.
     */
    inline AccessFlags operator^(AccessFlags a, AccessFlags b)
    {
        return static_cast<AccessFlags>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
    }

    /**
     * @brief Enables bitwise NOT operation for GraphAccessFlags.
     * @param a The flag to negate.
     * @return The bitwise NOT of the flag.
     */
    inline AccessFlags operator~(AccessFlags a)
    {
        return static_cast<AccessFlags>(~static_cast<uint32_t>(a));
    }

    /**
     * @brief Enables the |= assignment operator for AccessFlags.
     * @param a The flag to modify.
     * @param b The flag to OR with.
     * @return A reference to the modified flag.
     */
    inline AccessFlags& operator|=(AccessFlags& a, AccessFlags b)
    {
        return a = a | b;
    }

    /**
     * @brief Enables the &= assignment operator for AccessFlags.
     * @param a The flag to modify.
     * @param b The flag to AND with.
     * @return A reference to the modified flag.
     */
    inline AccessFlags& operator&=(AccessFlags& a, AccessFlags b)
    {
        return a = a & b;
    }

    /**
     * @brief Enables the ^= assignment operator for AccessFlags.
     * @param a The flag to modify.
     * @param b The flag to XOR with.
     * @return A reference to the modified flag.
     */
    inline AccessFlags& operator^=(AccessFlags& a, AccessFlags b)
    {
        return a = a ^ b;
    }

    /**
     * @brief Enables the != assignment operator for AccessFlags.
     * @param a The flag to modify.
     * @param i The flag to != with.
     * @return True if not equal, false otherwise.
     */
    inline bool operator|=(const AccessFlags& a, uint32_t i)
    {
        return static_cast<uint32_t>(a) != i;
    }

    /**
     * @brief Enables the != assignment operator for AccessFlags.
     * @param i The flag to != with.
     * @param a The flag to modify.
     * @return True if not equal, false otherwise.
     */
    inline bool operator|=(uint32_t i, const AccessFlags& a)
    {
        return static_cast<uint32_t>(a) != i;
    }

    /**
     * @brief Enables the == assignment operator for AccessFlags.
     * @param a The flag to modify.
     * @param i The flag to != with.
     * @return True if equal, false otherwise.
     */
    inline bool operator==(const AccessFlags& a, uint32_t i)
    {
        return static_cast<uint32_t>(a) == i;
    }

    /**
     * @brief Enables the != assignment operator for AccessFlags.
     * @param i The flag to != with.
     * @param a The flag to modify.
     * @return True if equal, false otherwise.
     */
    inline bool operator==(uint32_t i, const AccessFlags& a)
    {
        return static_cast<uint32_t>(a) == i;
    }

    inline bool hasFlag( AccessFlags flags, AccessFlags mask )
    {
        return static_cast< uint32_t >( flags & mask ) != 0;
    }

    inline bool isWriteAccess(AccessFlags access)
    {
        return (access & AccessFlags::MemoryWrite) != AccessFlags::None;
    }

    inline bool isReadAccess(AccessFlags access)
    {
        return (access & AccessFlags::MemoryRead ) != AccessFlags::None;
    }

}
#endif /* access_flags_h */

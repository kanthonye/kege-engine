//
//  rendering-info.hpp
//  editor
//
//  Created by Kenneth Esdaile on 2/16/26.
//

#ifndef rendering_info_hpp
#define rendering_info_hpp

#include "image/image.hpp"

namespace kege{

    /**
     * @brief Specifies how the attachment's contents should be handled at the beginning of the render pass.
     */
    enum class AttachmentLoadOp
    {
        Load,     ///< Load existing contents of the attachment.
        Clear,    ///< Clear the attachment to a specified value.
        DontCare  ///< Contents of the attachment are undefined at the start.
    };
    
    AttachmentLoadOp stringToAttachmentLoadOp( const std::string& name );


    /**
     * @brief Specifies how the attachment's contents should be handled at the end of the render pass.
     */
    enum class AttachmentStoreOp
    {
        Store,    ///< Store the rendered contents back to memory.
        DontCare, ///< Contents of the attachment are undefined after the render pass.
        None      ///< Do not store the results (e.g., for read-only attachments in some APIs).
    };

    AttachmentStoreOp stringToAttachmentStoreOp( const std::string& name );

    
    /**
     * @brief Describes a color or depth/stencil attachment for the `beginRendering` command.
     *
     * This struct contains information about the textures that will serve as
     * color or depth/stencil attachments during a dynamic rendering pass. It
     * includes the handle to the physical image view, load and store operations,
     * clear values, and the initial and final layouts of the attachment.
     *
     * The Render Graph is responsible for ensuring that the image layouts are
     * transitioned to the `initial_layout` *before* `beginRendering` is called.
     * Similarly, after `endRendering`, the layouts will be in the `final_layout`.
     */
    struct RenderingAttachmentInfo
    {
        ref::Image image{}; ///< Handle to the physical TextureView used as the attachment.

        ClearValue clear_value; ///< The value to clear the attachment to if `load_op` is `Clear`.
        AttachmentLoadOp load_op = AttachmentLoadOp::DontCare;   ///< The load operation for this attachment.
        AttachmentStoreOp store_op = AttachmentStoreOp::Store;     ///< The store operation for this attachment.

        ImageLayout image_layout = ImageLayout::Undefined;
        //ResourceState initial_layout = ResourceState::Undefined; ///< Expected layout of the attachment at the start of `beginRendering`.
        //ResourceState final_layout = ResourceState::Undefined;   ///< Desired layout of the attachment after `endRendering`.
    };

    /**
     * @brief Information structure for beginning a dynamic rendering pass.
     *
     * This struct contains all the necessary details to start rendering without
     * relying on traditional render pass objects. It includes the render area,
     * layer count, and the descriptions of the color and depth/stencil attachments.
     */
    struct RenderingInfo
    {
        Rect2D render_area{};       ///< Specifies the rectangular rendering bounds.
        uint32_t layer_count = 1; ///< Number of layers to render (for array textures or multiview).
        // uint32_t view_mask = 0; // For multiview rendering (API-specific).

        std::vector<RenderingAttachmentInfo> color_attachments; ///< Vector of color attachments.
        std::optional<RenderingAttachmentInfo> depth_attachment;   ///< Optional depth attachment.
        std::optional<RenderingAttachmentInfo> stencil_attachment; ///< Optional stencil attachment.
    };

}

#endif /* rendering_info_hpp */

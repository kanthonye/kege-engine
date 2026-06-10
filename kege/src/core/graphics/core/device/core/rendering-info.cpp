//
//  rendering-info.cpp
//  editor
//
//  Created by Kenneth Esdaile on 2/16/26.
//

#include "rendering-info.hpp"

namespace kege{

    AttachmentLoadOp stringToAttachmentLoadOp( const std::string& name )
    {
        if ( name == "Load" ) return AttachmentLoadOp::Load;
        if ( name == "Clear" ) return AttachmentLoadOp::Clear;
        if ( name == "DontCare" ) return AttachmentLoadOp::DontCare;
        kege::Log::error << "unsupported AttachmentLoadOp -> " <<name <<kege::Log::nl;
        return AttachmentLoadOp::Clear;
    }

    AttachmentStoreOp stringToAttachmentStoreOp( const std::string& name )
    {
        if ( name == "Store" ) return AttachmentStoreOp::Store;
        if ( name == "None" ) return AttachmentStoreOp::None;
        if ( name == "DontCare" ) return AttachmentStoreOp::DontCare;
        kege::Log::error << "unsupported AttachmentLoadOp -> " <<name <<kege::Log::nl;
        return AttachmentStoreOp::None;
    }

}

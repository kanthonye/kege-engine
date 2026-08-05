//
//  ui-post-layout-ops-executor.cpp
//  editor
//
//  Created by Kenneth Esdaile on 1/9/26.
//

#include "ui-post-layout-ops.hpp"
namespace kege::ui{

    void* PostLayoutOpsExecutor::getDst(AllocParam param)
    {
        if (param.index + param.size > _state_buffer.size()) return nullptr;
        return &_state_buffer[ param.index ];
    }

    AllocParam PostLayoutOpsExecutor::alloc(size_t size)
    {
        size_t index = _state_buffer_size;
        if (index + size >= _state_buffer.size())
        {
            _state_buffer.resize( index + size + 1024 );
        }
        _state_buffer_size += size;
        return AllocParam{ .index = index, .size = size };
    }

    void PostLayoutOpsExecutor::execute(kege::GUI* gui)
    {
        for (int i=0; i<_op_count; i++)
        {
            void* params = getDst(_operations[i].alloc);
            _operations[i].fn( gui, _operations[i].user_id, _operations[i].node_id, params );
        }
    }

    void PostLayoutOpsExecutor::reset()
    {
        _state_buffer_size = 0;
        _op_count = 0;
    }

    PostLayoutOpsExecutor::~PostLayoutOpsExecutor()
    {
        _operations.clear();
        _state_buffer.clear();
    }

    PostLayoutOpsExecutor::PostLayoutOpsExecutor()
    :   _state_buffer_size(0)
    {}

}

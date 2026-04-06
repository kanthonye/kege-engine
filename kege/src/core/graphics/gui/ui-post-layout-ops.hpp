//
//  ui-post-layout-ops-executor.hpp
//  editor
//
//  Created by Kenneth Esdaile on 1/9/26.
//

#ifndef ui_post_layout_ops_hpp
#define ui_post_layout_ops_hpp

#include "ui-core.hpp"

namespace kege::ui{

    struct AllocParam
    {
        size_t index;
        size_t size;
    };

    typedef void (*DeferredOperation)(Layout* layout, ui::ID user_id, WidgetId widget_id, void* data);

    class PostLayoutOpsExecutor
    {
    public:

        struct OpEntry
        {
            DeferredOperation fn;
            AllocParam alloc;
            ui::ID user_id;
            WidgetId widget_id;
        };

    public:

        template<typename Params>void pushPtr(ui::ID user_id, WidgetId widget_id, DeferredOperation fn, Params* params)
        {
            AllocParam alloc = this->alloc(sizeof(Params*));
            void* dst = getDst(alloc);

            // Copy the pointer value itself
            Params** storage = static_cast<Params**>(dst);
            *storage = params;  // Store the pointer

            if (_op_count >= _operations.size())
            {
                _operations.resize((_operations.empty()) ? 32 : 2 * _operations.size());
            }
            _operations[_op_count] = OpEntry{.fn = fn, .alloc = alloc, .user_id = user_id, .widget_id = widget_id};
            _op_count += 1;
        }

        template<typename Params>void push(ui::ID user_id, WidgetId widget_id, DeferredOperation fn, const Params& params)
        {
            AllocParam alloc = this->alloc(sizeof(params));
            void* dst = getDst(alloc);
            memcpy(dst, &params, alloc.size);

            if (_op_count >= _operations.size())
            {
                _operations.resize((_operations.empty()) ? 32 : 2 * _operations.size());
            }
            _operations[_op_count] = OpEntry{.fn = fn, .alloc = alloc, .user_id = user_id, .widget_id  = widget_id};
            _op_count += 1;
        }

    public:

        void* getDst(AllocParam param);
        AllocParam alloc(size_t size);
        void execute(Layout* layout);
        void reset();
        
        ~PostLayoutOpsExecutor();
        PostLayoutOpsExecutor();

    private:

        std::vector<OpEntry> _operations;
        size_t _op_count;

        std::vector<char> _state_buffer;
        size_t _state_buffer_size;
    };


}
#endif /* ui_post_layout_ops_hpp */

//
//  ui-elem.hpp
//  editor
//
//  Created by Kenneth Esdaile on 11/14/25.
//

#ifndef ui_elem_hpp
#define ui_elem_hpp

#include "../font/font.hpp"
#include "ui-input.hpp"
#include "ui-aligner.hpp"
#include "ui-core.hpp"
#include "ui-style-manager.hpp"
#include "ui-widget-manager.hpp"

namespace kege::ui{

    class Elem
    {
    public:

        friend bool operator ==( const Elem& a, const Elem& b );

        friend bool operator !=( const Elem& a, const Elem& b );

        /**
         * Retrieves a UI element by its index (const version).
         *
         * @return The UI element at the specified index.
         */
        const kege::ui::Widget* operator->() const;

        /**
         * Retrieves a UI element by its index (non-const version).
         *
         * @return The UI element at the specified index.
         */
        kege::ui::Widget* operator->();

        Elem& operator =(Elem&& other) noexcept;

        Elem& operator =( const Elem& other );

        operator size_t()const;
        operator bool()const;

        // Move constructor
        Elem( Elem&& other ) noexcept;

        Elem( Handle handle, Layout* layout );

        Elem( const Elem& other );

        Elem();

        ~Elem();

    private:

        kege::ui::Layout* _layout;
        kege::Handle _handle;
        friend Layout;
        friend Cursor;
    };

}
#endif /* ui_elem_hpp */

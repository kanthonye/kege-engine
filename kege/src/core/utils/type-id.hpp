//
//  KEGE (Kenneth Esdaile Game Engine)
//  TypeID.hpp
//
//  Created by Kenneth A Esdaile on 8/6/20.
//  Copyright © 2020 Kenneth A Esdaile. All rights reserved.
//

#ifndef kege_type_id_hpp
#define kege_type_id_hpp

#include <stdlib.h>

namespace kege
{
    template<typename T> class TypeCounter
    {
        protected:
        static int generate(){ return _counter++; }
        static uint32_t _counter;
    };
    template<typename T> uint32_t TypeCounter< T >::_counter = 0;



    template<typename T, typename G> struct TypeId : private TypeCounter< T >
    {
        public: static int get(){ return _type; }
        private: static int _type;
    };
    template<typename T, typename G> int TypeId< T, G >::_type = TypeCounter< T >::generate();
}

#endif // kege_type_id_hpp

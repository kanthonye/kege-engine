//
//  semaphore.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/14/25.
//

#ifndef kege_semaphore_hpp
#define kege_semaphore_hpp

#include "../common/graphics-common.h"

namespace kege
{
    class Semaphore;
}

namespace kege::vk
{
    class Semaphore;
}

namespace kege::ref
{
    typedef kege::Ref< kege::Semaphore > Semaphore;
}

namespace kege::cref
{
    typedef kege::Ref< const kege::Semaphore > Semaphore;
}

namespace kege{

    class Semaphore : public kege::RefCounter
    {
    public:

        virtual const vk::Semaphore* vk()const{ return nullptr; };
        virtual vk::Semaphore* vk(){ return nullptr; };
        virtual ~Semaphore() = default;

    protected:

        Semaphore() = default;
    };

}
#endif /* kege_semaphore_hpp */

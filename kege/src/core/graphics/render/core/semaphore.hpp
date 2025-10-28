//
//  semaphore.hpp
//  physics
//
//  Created by Kenneth Esdaile on 10/14/25.
//

#ifndef kege_semaphore_hpp
#define kege_semaphore_hpp

#include "graphics-core.hpp"

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

//
//  simulator.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/28/25.
//

#ifndef kege_physics_simulator_hpp
#define kege_physics_simulator_hpp

#include "../../../-/component-dependencies.hpp"
#include "../../debug/draw-commands.hpp"
#include "../collision/collider/collider.hpp"
#include "../collision/contact/contact.hpp"
#include "../collision/rayhit/rayhit.hpp"
#include "../collision/contact/contact.hpp"

namespace kege::physics{

    class Simulation;

    class Simulator : public kege::RefCounter
    {
    public:

        virtual void simulate( double time_step ) = 0;
        virtual ~Simulator(){};

    protected:

        Simulator(): _simulator( nullptr ) {};

    protected:

        Simulation* _simulator;
        friend class Simulation;
    };

}
#endif /* kege_physics_simulator_hpp */

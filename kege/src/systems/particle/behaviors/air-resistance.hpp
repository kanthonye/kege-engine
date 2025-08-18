//
//  air-resistance.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/27/25.
//

#ifndef air_resistance_hpp
#define air_resistance_hpp

#include "behavior.hpp"

namespace kege{

    struct AirResistance : public Behavior
    {
        void update( double dms, Particle& particle );
        AirResistance
        (
            float coefficient = 0.47f, // Drag coefficient (default for a sphere)
            float fluid_density = 1.225f, // Air density (kg/m³, 1.225 at sea level)
            bool use_quadratic_drag = true  // Toggle between quadratic (realistic) or linear (game) drag
        );

        float fluid_density;
        float coefficient;
        bool use_quadratic_drag;
    };

}

#endif /* air_resistance_hpp */

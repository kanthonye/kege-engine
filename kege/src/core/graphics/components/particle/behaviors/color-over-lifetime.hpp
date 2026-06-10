//
//  color-over-lifetime.hpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/27/25.
//

#ifndef color_over_lifetime_hpp
#define color_over_lifetime_hpp

#include "behavior.hpp"

namespace kege{

    struct GradientKey
    {
        float t; // Time (0 to 1)
        vec4 color; // RGBA
    };
    class Gradient {
    public:

        void addKey( float t, const vec4& color);
        vec4 evaluate(float t) const;
        size_t size()const;
        void sortKeys();

        //Gradient();
        Gradient() = default;

    private:

        std::vector< GradientKey > _keys;
    };
    struct ColorOverLifetime : public Behavior
    {
        void update( double dms, Particle& particle );
        ColorOverLifetime( const Gradient& colors );
        Gradient gradient;
    };

}

#endif /* color_over_lifetime_hpp */

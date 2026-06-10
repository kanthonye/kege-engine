//
//  color-over-lifetime.cpp
//  graphics
//
//  Created by Kenneth Esdaile on 6/27/25.
//

#include "color-over-lifetime.hpp"

namespace kege{

    void Gradient::addKey( float t, const vec4& color)
    {
        _keys.push_back({ t, color });
    }
    vec4 Gradient::evaluate(float t) const
    {
        if (_keys.empty())
            throw std::runtime_error("No keys in gradient");

        if (t <= _keys.front().t)
            return _keys.front().color;
        if (t >= _keys.back().t)
            return _keys.back().color;

        // Binary search for the interval [a, b] where a.t <= t < b.t
        auto it = std::upper_bound(_keys.begin(), _keys.end(), t, [](float value, const GradientKey& key) -> bool {
            return value < key.t;
        });

        const GradientKey& b = *it;
        const GradientKey& a = *(it - 1);
        float range = b.t - a.t;
        float localT = (t - a.t) / range;

        return a.color * (1 - localT) + b.color * localT;
    }
    size_t Gradient::size()const
    {
        return _keys.size();
    }
    void Gradient::sortKeys()
    {
        std::sort(_keys.begin(), _keys.end(), [](const GradientKey& key1, const GradientKey& key2) -> bool {
            return key1.t < key2.t;
        });
    }

    void ColorOverLifetime::update( double dms, Particle& particle )
    {
        if ( gradient.size() == 0) return;

        float t = particle.health / particle.max_health;
        particle.color = gradient.evaluate( t );
    }

    ColorOverLifetime::ColorOverLifetime( const Gradient& gradient )
    :   gradient( gradient )
    {}

}

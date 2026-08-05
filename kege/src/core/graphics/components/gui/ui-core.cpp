//
//  ui-primatives.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/2/25.
//

#include "ui-core.hpp"

namespace kege::ui{

    Rect& Rect::operator=(const Quad& q)
    {
        x = q.x;
        y = q.y;
        width = q.width;
        height = q.height;
        // color is intentionally dropped
        return *this;
    }

    Quad& Quad::operator=(const Rect& r)
    {
        x = r.x;
        y = r.y;
        width = r.width;
        height = r.height;
        // color left untouched — preserves whatever it was
        return *this;
    }

//    Quad::Quad(float x, float y, float width, float height, uint32_t color)
//    :   x(x)
//    ,   y(x)
//    ,   width(width)
//    ,   height(height)
//    ,   color(color)
//    {}
//
//    Quad::Quad(float x, float y, float width, float height)
//    :   x(x)
//    ,   y(x)
//    ,   width(width)
//    ,   height(height)
//    {}
//
//    Quad::Quad(const kege::ui::Rect& rect)
//    :   x(rect.x)
//    ,   y(rect.x)
//    ,   width(rect.width)
//    ,   height(rect.height)
//    {}
//
//    Quad::Quad(const kege::ui::Quad& quad)
//    :   x(quad.x)
//    ,   y(quad.x)
//    ,   width(quad.width)
//    ,   height(quad.height)
//    {}
//
//    Quad::Quad()
//    {}

    ui::Sizing fixed(float size)
    {
        return ui::Sizing
        {
            .size = size,
            .type = ui::SizingType::Fixed
        };
    }

    ui::Sizing extend()
    {
        return ui::Sizing
        {
            .size = 1.f,
            .type = ui::SizingType::Extend
        };
    }

    ui::Sizing flexible()
    {
        return ui::Sizing
        {
            .size = 0.f,
            .type = ui::SizingType::Flexible
        };
    }

    ui::Sizing percent(float percent)
    {
        return ui::Sizing
        {
            .size = (percent / 100.0f),
            .type = ui::SizingType::Percent
        };
    }

    Background::Background(uint32_t img_index, const ui::Rect& texel)
    {
        this->type = BackgroundType::IMAGE;
        this->texel = texel;
    }
    ui::Background::Background(const kege::vec4& color)
    {
        this->type = BackgroundType::COLOR;
        this->color = packRGBA8(color);
    }
    ui::Background::Background(uint32_t color)
    {
        this->type = BackgroundType::COLOR;
        this->color = color;
    }


    kege::vec3 rgb(uint32_t hex_color)
    {
      float r = ((hex_color >> 16) & 0xFF) / 255.0;
      float g = ((hex_color >> 8) & 0xFF) / 255.0;
      float b = (hex_color & 0xFF) / 255.0;
      //float a = 1.0; // Default alpha is 1.0 (fully opaque)
      return kege::vec3{r, g, b};
    }

    // To handle alpha in the hex code (e.g., 0x800080FF for purple with full alpha):
    kege::vec4 rgba(uint32_t hex_color)
    {
      float r = ((hex_color >> 24) & 0xFF) / 255.0; // Changed bit shifts for RGBA order
      float g = ((hex_color >> 16) & 0xFF) / 255.0;
      float b = ((hex_color >>  8) & 0xFF) / 255.0;
      float a = (hex_color & 0xFF)         / 255.0;
      return kege::vec4{r, g, b, a};
    }

    uint32_t packRGBA8(float r, float g, float b, float a)
    {
        uint32_t R = uint32_t(kege::clamp(r, 0.0f, 1.0f) * 255.0f);
        uint32_t G = uint32_t(kege::clamp(g, 0.0f, 1.0f) * 255.0f);
        uint32_t B = uint32_t(kege::clamp(b, 0.0f, 1.0f) * 255.0f);
        uint32_t A = uint32_t(kege::clamp(a, 0.0f, 1.0f) * 255.0f);

        return (A << 24) | (B << 16) | (G << 8) | R;
    }

    bool testPointVsRect( const kege::dvec2& p, const ui::Rect& rect )
    {
        return
        (
            p.x > rect.x &&
            p.y > rect.y &&
            p.x < rect.x + rect.width &&
            p.y < rect.y + rect.height
        );
    }

    bool checkOverlap(const kege::ui::Rect& a, const kege::ui::Rect& b)
    {
        // Check if one rectangle is to the left of the other
        if (a.x + a.width < b.x || b.x + b.width < a.x) {
            return false;
        }

        // Check if one rectangle is above the other
        // Note: This logic works regardless of whether Y increases up or down,
        // as long as it is consistent for both rectangles.
        if (a.y + a.height < b.y || b.y + b.height < a.y) {
            return false;
        }

        return true;
    }

    bool testPointVsRect( const kege::dvec2& p, const kege::ui::Quad& quad )
    {
        return
        (
            p.x > quad.x &&
            p.y > quad.y &&
            p.x < quad.x + quad.width &&
            p.y < quad.y + quad.height
        );
    }

    bool checkOverlap(const kege::ui::Quad& a, const kege::ui::Quad& b)
    {
        // Check if one rectangle is to the left of the other
        if (a.x + a.width < b.x || b.x + b.width < a.x) {
            return false;
        }

        // Check if one rectangle is above the other
        // Note: This logic works regardless of whether Y increases up or down,
        // as long as it is consistent for both rectangles.
        if (a.y + a.height < b.y || b.y + b.height < a.y) {
            return false;
        }

        return true;
    }
    
    bool checkOverlap(const kege::ui::Quad& a, const kege::ui::Rect& b)
    {
        // Check if one rectangle is to the left of the other
        if (a.x + a.width < b.x || b.x + b.width < a.x) {
            return false;
        }

        // Check if one rectangle is above the other
        // Note: This logic works regardless of whether Y increases up or down,
        // as long as it is consistent for both rectangles.
        if (a.y + a.height < b.y || b.y + b.height < a.y) {
            return false;
        }

        return true;
    }
}

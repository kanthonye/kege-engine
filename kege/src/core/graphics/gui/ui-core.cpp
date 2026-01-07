//
//  ui-primatives.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/2/25.
//

#include "ui-core.hpp"

namespace kege::ui{

    ui::Sizing fixed(float size)
    {
        return ui::Sizing
        {
            .type = ui::SizingType::Fixed,
            .size = size
        };
    }

    ui::Sizing extend()
    {
        return ui::Sizing
        {
            .type = ui::SizingType::Extend,
            .size = 1.f
        };
    }

    ui::Sizing flexible()
    {
        return ui::Sizing
        {
            .type = ui::SizingType::Flexible,
            .size = 0.f
        };
    }

    ui::Sizing percent(float percent)
    {
        return ui::Sizing
        {
            .type = ui::SizingType::Percent,
            .size = (percent / 100.0f)
        };
    }

    Background::Background(uint32_t img_index, const ui::Rect& texel)
    {
        this->type = BackgroundType::IMAGE;
        this->texel = texel;
    }
    ui::Background::Background(const ui::Color& color)
    {
        this->type = BackgroundType::COLOR;
        this->color = packRGBA8(color);
    }
    ui::Background::Background(uint32_t color)
    {
        this->type = BackgroundType::COLOR;
        this->color = color;
    }


    ui::Color rgb(uint32_t hex_color)
    {
      float r = ((hex_color >> 16) & 0xFF) / 255.0;
      float g = ((hex_color >> 8) & 0xFF) / 255.0;
      float b = (hex_color & 0xFF) / 255.0;
      float a = 1.0; // Default alpha is 1.0 (fully opaque)
      return ui::Color{r, g, b, a};
    }

    // To handle alpha in the hex code (e.g., 0x800080FF for purple with full alpha):
    ui::Color rgba(uint32_t hex_color)
    {
      float r = ((hex_color >> 24) & 0xFF) / 255.0; // Changed bit shifts for RGBA order
      float g = ((hex_color >> 16) & 0xFF) / 255.0;
      float b = ((hex_color >>  8) & 0xFF) / 255.0;
      float a = (hex_color & 0xFF)         / 255.0;
      return ui::Color{r, g, b, a};
    }

    uint32_t packRGBA8(float r, float g, float b, float a)
    {
        uint32_t R = uint32_t(kege::clamp(r, 0.0f, 1.0f) * 255.0f);
        uint32_t G = uint32_t(kege::clamp(g, 0.0f, 1.0f) * 255.0f);
        uint32_t B = uint32_t(kege::clamp(b, 0.0f, 1.0f) * 255.0f);
        uint32_t A = uint32_t(kege::clamp(a, 0.0f, 1.0f) * 255.0f);

        return (A << 24) | (B << 16) | (G << 8) | R;
    }
}

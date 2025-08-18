//
//  ui-primatives.cpp
//  flat-landscape
//
//  Created by Kenneth Esdaile on 3/2/25.
//

#include "ui-primatives.hpp"

namespace kege::ui{

    ui::Sizing fixed(float size)
    {
        return ui::Sizing
        {
            .type = ui::SIZE_FIXED,
            .size = size
        };
    }

    ui::Sizing extend()
    {
        return ui::Sizing
        {
            .type = ui::SIZE_EXTEND,
            .size = 1.f
        };
    }

    ui::Sizing extend(float percent)
    {
        return ui::Sizing
        {
            .type = ui::SIZE_EXTEND,
            .size = (percent / 100.0f)
        };
    }

    ui::Sizing flexible()
    {
        return ui::Sizing
        {
            .type = ui::SIZE_FLEXIBLE,
            .size = 0.f
        };
    }

    ui::Sizing percent(float percent)
    {
        return ui::Sizing
        {
            .type = ui::SIZE_PERCENT,
            .size = (percent / 100.0f)
        };
    }

    ui::Background bgImage(const ui::Rect2D& texel)
    {
        return ui::Background
        {
            .type = ui::BACKGROUND_IMAGE,
            //.texel = texel
        };
    }
    ui::Background bgColor(const ui::Color& color)
    {
        return ui::Background
        {
            .type = ui::BACKGROUND_COLOR,
            .color = color
        };
    }

    ui::Background bgColor(uint32_t color)
    {
        return ui::Background
        {
            .type = ui::BACKGROUND_COLOR,
            .color = rgba(color)
        };
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

}

//
//  ui-style-loader.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/27/25.
//

#include "ui-style-loader.hpp"

namespace kege::ui{

    bool isNumeric(const std::string& str)
    {
        if (str.empty()) return false;

        size_t start = 0;
        size_t n = str.size();

        // Handle optional leading sign
        if (str[0] == '+' || str[0] == '-')
        {
            if (n == 1) return false;  // Just "+" or "-" is invalid
            start = 1;
        }

        bool hasDigit = false;
        bool hasDecimal = false;
        bool hasExponent = false;

        for (size_t i = start; i < n; ++i) {
            char c = str[i];

            if (std::isdigit(c)) {
                hasDigit = true;
                continue;
            }

            if (c == '.') {
                if (hasDecimal || hasExponent) return false;  // Multiple decimals or after 'e'
                hasDecimal = true;
                continue;
            }

            if (c == 'e' || c == 'E') {
                if (hasExponent || !hasDigit) return false;  // Multiple 'e' or no digit before
                hasExponent = true;
                hasDigit = false;  // Need digit after exponent

                // Handle optional sign after 'e'
                if (i + 1 < n && (str[i + 1] == '+' || str[i + 1] == '-')) {
                    ++i;
                    if (i + 1 >= n) return false;  // Must have digit after sign
                }
                continue;
            }

            return false;  // Invalid character
        }

        return hasDigit;  // Must have at least one digit
    }
    Sizing parseSizing( kege::Json json )
    {
        if ( json )
        {
            const char* length = json.value();
            if ( strcmp( "extend", length ) == 0)
            {
                return ui::extend();
            }
            else if ( strcmp( "flexible", length ) == 0)
            {
                return ui::flexible();
            }
            else if ( strcmp( "vh", length ) == 0)
            {
                return ui::fixed( 896 );
            }
            else if ( strcmp( "vw", length ) == 0)
            {
                return ui::fixed( 1536 );
            }
            else if ( strchr( length, '%' ) != nullptr )
            {
                const char* s = strchr( length, '%' );
                size_t size = s - length;
                char num[size + 1];
                for (int i=0; i<size; ++i) num[i] = length[i];
                num[size] = 0;
                return ui::percent( atof( num ) );
            }
            else
            {
                if( isNumeric( length ) )
                {
                    return ui::fixed( atof( length ) );
                }
                else
                {
                    kege::Log::warning <<"unsupported ui-sizing term -> " <<length <<kege::Log::nl;
                }
            }
        }
        return {};
    }
    Padding parsePadding( kege::Json json )
    {
        Padding padding = {};
        if ( json )
        {
            arr< double, 4 > arr = json.getArray<double, 4>( atof );
            padding.left  = arr.data[0];
            padding.above = arr.data[1];
            padding.right = arr.data[2];
            padding.below = arr.data[3];
        }
        return padding;
    }
    
    uint32_t parseColor( kege::Json json )
    {
        Color color = {};
        if ( json )
        {
            if ( json.count() == 4 )
            {
                arr< double, 4 > arr = json.getArray<double, 4>( atof );
                return packRGBA8(arr.data[0], arr.data[1], arr.data[2], arr.data[3]);
            }
            else if ( json.count() == 3 )
            {
                arr< double, 3 > arr = json.getArray<double, 3>( atof );
                return packRGBA8(arr.data[0], arr.data[1], arr.data[2], 1.f);
            }
            else
            {
                size_t len = strlen( json.value() );
                uint32_t hex = (uint32_t)std::stoul( json.value(), 0, 16 );
                return hex;
            }
        }
        return 0xFFFFFFFF;
    }

    Background parseBackground( kege::Json json )
    {
        Background background = {};
        background.color = parseColor( json );
        return background;
    }

    Alignment parseAlignment( kege::Json json )
    {
        Alignment alignment;
        alignment.content = {ui::AlignPosX::LEFT, ui::AlignPosY::TOP};
        alignment.direction = AlignDir::HORIZONTAL;

        static std::map< std::string, AlignPosX > origins;
        if ( json )
        {
            kege::Json direction = json[ "direction" ];
            if ( direction )
            {
                if ( strcmp( direction.value(), "right to left" ) == 0)
                {
                    alignment.direction = AlignDir::HORIZONTAL;
                }
                else if ( strcmp( direction.value(), "top to bottom" ) == 0)
                {
                    alignment.direction = AlignDir::VERTICAL;
                }
                else if ( strcmp( direction.value(), "bottom to top" ) == 0)
                {
                    alignment.direction = AlignDir::VERTICAL;
                }
            }
            kege::Json origin = json[ "origin" ];
            if ( origin )
            {
                if ( origins.empty() )
                {
//                    origins[ "top left" ] = AlignPos::TOP_LEFT;
//                    origins[ "top center" ] = AlignPos::TOP_CENTER;
//                    origins[ "top right" ] = AlignPos::TOP_RIGHT;
//                    origins[ "left center" ] = AlignPos::LEFT_CENTER;
//                    origins[ "center center" ] = AlignPos::CENTER_CENTER;
//                    origins[ "right center" ] = AlignPos::RIGHT_CENTER;
//                    origins[ "bottom left" ] = AlignPos::BOTTOM_LEFT;
//                    origins[ "bottom center" ] = AlignPos::BOTTOM_CENTER;
//                    origins[ "bottom right" ] = AlignPos::BOTTOM_RIGHT;
                }
                auto m = origins.find( origin.value() );
                if ( m != origins.end() )
                {
//                    alignment.position = m->second;
                }
            }
        }
        return alignment;
    }
    Border parseBorderRadius( kege::Json json )
    {
        Border border = {};
        if ( json )
        {
            if ( json.count() == 4 )
            {
                arr< int, 4 > arr = json.getArray<int, 4>( atoi );
                border.corner_curves.top_left     = arr.data[0];
                border.corner_curves.top_right    = arr.data[1];
                border.corner_curves.bottom_right = arr.data[2];
                border.corner_curves.bottom_right = arr.data[3];
            }
            else if ( json.count() == 3 )
            {
                arr< int, 3 > arr = json.getArray<int, 3>( atoi );
                border.corner_curves.top_left     = arr.data[0];
                border.corner_curves.top_right    = arr.data[1];
                border.corner_curves.bottom_right = arr.data[2];
            }
            else if ( json.count() == 2 )
            {
                arr< int, 2 > arr = json.getArray<int, 2>( atoi );
                border.corner_curves.top_left     = arr.data[0];
                border.corner_curves.top_right    = arr.data[0];
                border.corner_curves.bottom_right = arr.data[1];
                border.corner_curves.bottom_right = arr.data[1];
            }
            else
            {
                int b = atoi( json.value() );
                border.corner_curves.top_left     = b;
                border.corner_curves.top_right    = b;
                border.corner_curves.bottom_right = b;
                border.corner_curves.bottom_right = b;
            }
        }
        return border;
    }
    Extent parseGap( kege::Json json )
    {
        Extent gap = {0,0};
        if ( json )
        {
            if ( json.count() == 2 )
            {
                arr< double, 2 > arr = json.getArray<double, 2>( atof );
                gap.width  = arr.data[0];
                gap.height = arr.data[1];
            }
            else
            {
                double g = atof( json.value() );
                gap.width  = g;
                gap.height = g;
            }
        }
        return gap;
    }
    Positioning parsePositioning( kege::Json json )
    {
        Positioning position = Positioning::Relative;
        if ( json )
        {
            if ( strcmp( json.value(), "absolute" ) == 0 )
            {
                position = Positioning::Absolute;
            }
        }
        return position;
    }
    AlignText parseAlignText( kege::Json json )
    {
        AlignText align = AlignText::Left;
        if ( json )
        {
            if ( strcmp( json.value(), "center" ) == 0 )
            {
                align = AlignText::Center;
            }
            else if ( strcmp( json.value(), "right" ) == 0 )
            {
                align = AlignText::Right;
            }
        }
        return align;
    }
    std::vector< std::pair< std::string, Style > > StyleLoader::load( const std::string& filename )
    {
        kege::Json json = kege::JsonParser::load( filename.data() );
        if ( !json )
        {
            kege::Log::error <<"fail to open file -> " << filename << Log::nl;
            return {};
        }

        std::vector< std::pair< std::string, Style > > style_sheet;
        json[ "styles" ].foreach([ &style_sheet ](const std::string& name, const Json& json)
        {
            Style style = {};

            style.background = parseBackground( json[ "background" ] );
            style.text_color = parseColor( json[ "color" ] );
            style.padding = parsePadding( json[ "padding" ] );
            style.width = parseSizing( json[ "width" ] );
            style.height = parseSizing( json[ "height" ] );
            style.align = parseAlignment( json[ "align" ] );
            style.border = parseBorderRadius( json[ "border_radius" ] );
            style.gap = parseGap( json[ "gap" ] );
            style.position = parsePositioning( json[ "position" ] );
            style.align_text = parseAlignText( json[ "align_text" ] );
            style.clip_overflow = json.getBool( "clip_overflow" );
            style.zindex = json.getInt( "zindex" );
            style.font_size = json.getInt( "font_size", 14 );

            style_sheet.push_back( std::pair< std::string, Style >{ name, style } );
        });
        return style_sheet;
    }
}

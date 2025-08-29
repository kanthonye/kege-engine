//
//  ui-style-loader.cpp
//  physics
//
//  Created by Kenneth Esdaile on 8/27/25.
//

#include "ui-style-loader.hpp"

namespace kege::ui{

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
                return ui::fixed( atof( length ) );
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
    Color parseColor( kege::Json json )
    {
        Color color = {};
        if ( json )
        {
            if ( json.count() == 4 )
            {
                arr< double, 4 > arr = json.getArray<double, 4>( atof );
                color.r = arr.data[0];
                color.g = arr.data[1];
                color.b = arr.data[2];
                color.a = arr.data[3];
            }
            else if ( json.count() == 3 )
            {
                arr< double, 3 > arr = json.getArray<double, 3>( atof );
                color.r = arr.data[0];
                color.g = arr.data[1];
                color.b = arr.data[2];
                color.a = 1.0;
            }
            else
            {
                size_t len = strlen( json.value() );
                uint64_t hex = std::stoul( json.value(), 0, 16 );
                if ( len == 10 )
                {
                    color = ui::rgba( (uint32_t)hex );
                }
                else if ( len == 8 )
                {
                    color = ui::rgb( (uint32_t)hex );
                }
                else
                {
                    color = Color( (uint32_t)hex );
                }
            }
        }
        return color;
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
        alignment.origin = ALIGN_TOP_LEFT;
        alignment.direction = DIRECTION_LEFT_TO_RIGHT;

        static std::map< std::string, AlignOrigin > origins;
        if ( json )
        {
            kege::Json direction = json[ "direction" ];
            if ( direction )
            {
                if ( strcmp( direction.value(), "right to left" ) == 0)
                {
                    alignment.direction = DIRECTION_RIGHT_TO_LEFT;
                }
                else if ( strcmp( direction.value(), "top to bottom" ) == 0)
                {
                    alignment.direction = DIRECTION_TOP_TO_BOTTOM;
                }
                else if ( strcmp( direction.value(), "bottom to top" ) == 0)
                {
                    alignment.direction = DIRECTION_BOTTOM_TO_TOP;
                }
            }
            kege::Json origin = json[ "origin" ];
            if ( origin )
            {
                if ( origins.empty() )
                {
                    origins[ "top left" ] = ALIGN_TOP_LEFT;
                    origins[ "top center" ] = ALIGN_TOP_CENTER;
                    origins[ "top right" ] = ALIGN_TOP_RIGHT;
                    origins[ "left center" ] = ALIGN_LEFT_CENTER;
                    origins[ "center center" ] = ALIGN_CENTER_CENTER;
                    origins[ "right center" ] = ALIGN_RIGHT_CENTER;
                    origins[ "bottom left" ] = ALIGN_BOTTOM_LEFT;
                    origins[ "bottom center" ] = ALIGN_BOTTOM_CENTER;
                    origins[ "bottom right" ] = ALIGN_BOTTOM_RIGHT;
                }
                auto m = origins.find( origin.value() );
                if ( m != origins.end() )
                {
                    alignment.origin = m->second;
                }
            }
        }
        return alignment;
    }
    Corners parseBorderRadius( kege::Json json )
    {
        Corners corners = {};
        if ( json )
        {
            if ( json.count() == 4 )
            {
                arr< int, 4 > arr = json.getArray<int, 4>( atoi );
                corners.top_left     = arr.data[0];
                corners.top_right    = arr.data[1];
                corners.bottom_right = arr.data[2];
                corners.bottom_right = arr.data[3];
            }
            else if ( json.count() == 3 )
            {
                arr< int, 3 > arr = json.getArray<int, 3>( atoi );
                corners.top_left     = arr.data[0];
                corners.top_right    = arr.data[1];
                corners.bottom_right = arr.data[2];
            }
            else if ( json.count() == 2 )
            {
                arr< int, 2 > arr = json.getArray<int, 2>( atoi );
                corners.top_left     = arr.data[0];
                corners.top_right    = arr.data[0];
                corners.bottom_right = arr.data[1];
                corners.bottom_right = arr.data[1];
            }
            else if ( json.count() == 1 )
            {
                int b = atoi( json.value() );
                corners.top_left     = b;
                corners.top_right    = b;
                corners.bottom_right = b;
                corners.bottom_right = b;
            }
        }
        return corners;
    }
    Extent2D parseGap( kege::Json json )
    {
        Extent2D gap = {0,0};
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
            KEGE_LOG_ERROR <<"fail to open file -> " << filename << Log::nl;
            return {};
        }

        std::vector< std::pair< std::string, Style > > style_sheet;
        json.foreach([ &style_sheet ](const std::string& name, const Json& json)
        {
            Style style = {};

            style.background = parseBackground( json[ "background" ] );
            style.color = parseColor( json[ "color" ] );
            style.padding = parsePadding( json[ "padding" ] );
            style.width = parseSizing( json[ "width" ] );
            style.height = parseSizing( json[ "height" ] );
            style.align = parseAlignment( json[ "align" ] );
            style.border_radius = parseBorderRadius( json[ "border_radius" ] );
            style.gap = parseGap( json[ "gap" ] );
            style.position = parsePositioning( json[ "position" ] );
            style.align_text = parseAlignText( json[ "align_text" ] );
            style.clip_overflow = json[ "clip_overflow" ].getBool();
            style.wrap_around = json[ "wrap_around" ].getBool();
            style.zindex = json[ "zindex" ].getInt();
            style.font_size = json[ "font_size" ].getInt(14);

            style_sheet.push_back( std::pair< std::string, Style >{ name, style } );
        });
        return style_sheet;
    }
}

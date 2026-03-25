//
//  feature-flags.cpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 2/22/26.
//


#include "feature-flags.hpp"
namespace kege{

    const char* featureFlagToString(FeatureFlag flag)
    {
        switch (flag)
        {
            case NONE:                     return "NONE";
            case VERTEX_COLOR:             return "VERTEX_COLOR";
            //case BILLBOARD:                return "BILLBOARD";
            case INSTANCED_RENDERING:      return "INSTANCED_RENDERING";
            case VERTEX_DISPLACEMENT:      return "VERTEX_DISPLACEMENT";
            case GPU_SKINNING:             return "GPU_SKINNING";
            case MORPH_TARGETS:            return "MORPH_TARGETS";
            case VERTEX_ANIMATION:         return "VERTEX_ANIMATION";
            case WIND_ANIMATION:           return "WIND_ANIMATION";

            case NORMAL_MAPPING:           return "NORMAL_MAPPING";
            case TRIPLANAR_MAPPING:        return "TRIPLANAR_MAPPING";
            case PARALLAX_MAPPING:         return "PARALLAX_MAPPING";
            case PARALLAX_OCCLUSION:       return "PARALLAX_OCCLUSION";
            case DETAIL_NORMAL:            return "DETAIL_NORMAL";
            case IMAGE_BASE_LIGHTING:      return "IMAGE_BASE_LIGHTING";

            case TESSELLATION:             return "TESSELLATION";
            case PN_TRIANGLES:             return "PN_TRIANGLES";
            case TESSELLATION_DISPLACEMENT:return "TESSELLATION_DISPLACEMENT";

            case CLEAR_COAT:               return "CLEAR_COAT";
            case ANISOTROPIC_REFLECTION:   return "ANISOTROPIC_REFLECTION";
            case SHEEN_LAYER:              return "SHEEN_LAYER";
            case THIN_FILM:                return "THIN_FILM";
            case TRANSMISSION:             return "TRANSMISSION";
            case SUBSURFACE_SCATTERING:    return "SUBSURFACE_SCATTERING";

            case ALPHA_TEST:               return "ALPHA_TEST";
            case ALPHA_TO_COVERAGE:        return "ALPHA_TO_COVERAGE";
            case DITHERED_OPACITY:         return "DITHERED_OPACITY";
            case PREMULTIPLIED_ALPHA:      return "PREMULTIPLIED_ALPHA";

            case SOFT_PARTICLES:           return "SOFT_PARTICLES";
            //case DISTORTION_PARTICLES:     return "DISTORTION_PARTICLES";
            //case PARTICLE_STRETCH:         return "PARTICLE_STRETCH";
            //case PARTICLE_ROTATION:        return "PARTICLE_ROTATION";

            case SCREEN_SPACE_REFLECTION:  return "SCREEN_SPACE_REFLECTION";
            case REFRACTION:               return "REFRACTION";
            case WATER_EFFECTS:            return "WATER_EFFECTS";
            case DECAL:                    return "DECAL";

            case DIRECTIONAL_LIGHT:        return "DIRECTIONAL_LIGHT";
            case POINT_LIGHT:              return "POINT_LIGHT";
            case SPOT_LIGHT:               return "SPOT_LIGHT";
            case AREA_LIGHTS:              return "AREA_LIGHTS";
            case VOLUMETRIC_LIGHTING:      return "VOLUMETRIC_LIGHTING";
            case CONTACT_SHADOWS:          return "CONTACT_SHADOWS";
            case MULTIPLE_BOUNCE_GI:       return "MULTIPLE_BOUNCE_GI";

            case MOTION_VECTORS:           return "MOTION_VECTORS";
            case VELOCITY_BUFFER:          return "VELOCITY_BUFFER";
            case DEPTH_ONLY:               return "DEPTH_ONLY";
            case CUSTOM_DEPTH:             return "CUSTOM_DEPTH";

            case TERRAIN_BLENDING:         return "TERRAIN_BLENDING";
            case VERTEX_GRASS_WIND:        return "VERTEX_GRASS_WIND";
            case LOD_CROSSFADE:            return "LOD_CROSSFADE";
            case DECAL_LAYER:              return "DECAL_LAYER";

            case CONTACT_HARDENING:        return "CONTACT_HARDENING";
            case VARIANCE_SHADOW_MAP:      return "VARIANCE_SHADOW_MAP";
            case CASCADE_BLENDING:         return "CASCADE_BLENDING";

            case RAY_TRACING_OVERRIDE:     return "RAY_TRACING_OVERRIDE";
            case VIRTUAL_TEXTURE:          return "VIRTUAL_TEXTURE";
            //case MEGA_TEXTURE:             return "MEGA_TEXTURE";
            case FLIPBOOK_ANIMATION:       return "FLIPBOOK_ANIMATION";

            case ROUNDED_UI_CORNERS:       return "ROUNDED_UI_CORNERS";
            case SINE_DIST_TEXT:           return "SINE_DIST_TEXT";

            case WIREFRAME_OVERLAY:        return "WIREFRAME_OVERLAY";
            case DEBUG_NORMALS:            return "DEBUG_NORMALS";
            case DEBUG_UVS:                return "DEBUG_UVS";
            case DEBUG_TANGENTS:           return "DEBUG_TANGENTS";

            default:                       return "UNKNOWN_FEATURE";
        }
    }

}

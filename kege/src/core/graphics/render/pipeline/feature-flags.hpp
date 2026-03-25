//
//  feature-flags.hpp
//  KE-GE
//
//  Created by Kenneth Esdaile on 2/7/26.
//

#ifndef kege_feature_flags_hpp
#define kege_feature_flags_hpp

#include <string>
#include <vector>
#include <sstream>

namespace kege{

    enum FeatureFlag : uint64_t
    {
        NONE = 0,

        // ====== VERTEX SHADER FEATURES ======
        VERTEX_COLOR             = 1ULL << 0,
        INSTANCED_RENDERING      = 1ULL << 2,
        VERTEX_DISPLACEMENT      = 1ULL << 3,
        GPU_SKINNING             = 1ULL << 4,
        MORPH_TARGETS            = 1ULL << 5,
        VERTEX_ANIMATION         = 1ULL << 6,
        WIND_ANIMATION           = 1ULL << 7,

        // ====== FRAGMENT SHADER FEATURES ======
        NORMAL_MAPPING           = 1ULL << 8,
        TRIPLANAR_MAPPING        = 1ULL << 9,
        PARALLAX_MAPPING         = 1ULL << 10,
        PARALLAX_OCCLUSION       = 1ULL << 11,
        DETAIL_NORMAL            = 1ULL << 12,
        IMAGE_BASE_LIGHTING      = 1ULL << 13,

        // ====== TESSELLATION FEATURES ======
        TESSELLATION             = 1ULL << 14,
        PN_TRIANGLES             = 1ULL << 15,
        TESSELLATION_DISPLACEMENT = 1ULL << 16,

        // ====== MATERIAL LAYERS ======
        CLEAR_COAT               = 1ULL << 17,
        ANISOTROPIC_REFLECTION   = 1ULL << 18,
        SHEEN_LAYER              = 1ULL << 19,
        THIN_FILM                = 1ULL << 20,
        TRANSMISSION             = 1ULL << 21,
        SUBSURFACE_SCATTERING    = 1ULL << 22,

        // ====== TRANSPARENCY/ALPHA ======
        ALPHA_TEST               = 1ULL << 23,
        ALPHA_TO_COVERAGE        = 1ULL << 24,
        DITHERED_OPACITY         = 1ULL << 25,
        PREMULTIPLIED_ALPHA      = 1ULL << 26,

        // ====== PARTICLE EFFECTS ======
        SOFT_PARTICLES           = 1ULL << 27,
        PARTICLE_ROTATION        = 1ULL << 28,

        // ====== ENVIRONMENT INTERACTION ======
        SCREEN_SPACE_REFLECTION  = 1ULL << 31,
        REFRACTION               = 1ULL << 32,
        WATER_EFFECTS            = 1ULL << 33,
        DECAL                    = 1ULL << 34,

        // ====== LIGHTING FEATURES ======
        DIRECTIONAL_LIGHT        = 1ULL << 35,
        POINT_LIGHT              = 1ULL << 36,
        SPOT_LIGHT               = 1ULL << 37,
        AREA_LIGHTS              = 1ULL << 38,
        VOLUMETRIC_LIGHTING      = 1ULL << 39,
        CONTACT_SHADOWS          = 1ULL << 40,
        MULTIPLE_BOUNCE_GI       = 1ULL << 41,

        // ====== POST-PROCESS/SCREEN SPACE ======
        MOTION_VECTORS           = 1ULL << 42,
        VELOCITY_BUFFER          = 1ULL << 43,
        DEPTH_ONLY               = 1ULL << 44,
        CUSTOM_DEPTH             = 1ULL << 45,

        // ====== TERRAIN/VEGETATION ======
        TERRAIN_BLENDING         = 1ULL << 46,
        VERTEX_GRASS_WIND        = 1ULL << 47,
        LOD_CROSSFADE            = 1ULL << 48,
        DECAL_LAYER              = 1ULL << 49,

        // ====== SHADOW FEATURES ======
        CONTACT_HARDENING        = 1ULL << 50,
        VARIANCE_SHADOW_MAP      = 1ULL << 51,
        CASCADE_BLENDING         = 1ULL << 52,

        // ====== ADVANCED RENDERING ======
        RAY_TRACING_OVERRIDE     = 1ULL << 53,
        VIRTUAL_TEXTURE          = 1ULL << 54,
        FLIPBOOK_ANIMATION       = 1ULL << 55,

        // ====== UI RENDERING ======
        ROUNDED_UI_CORNERS       = 1ULL << 56,
        SINE_DIST_TEXT           = 1ULL << 57,

        // ====== DEBUG/DEVELOPMENT ======
        WIREFRAME_OVERLAY        = 1ULL << 58,
        DEBUG_NORMALS            = 1ULL << 59,
        DEBUG_UVS                = 1ULL << 60,
        DEBUG_TANGENTS           = 1ULL << 61,

        MATERIAL                = 1ULL << 1,
        GAMMA                   = 1ULL << 62,
        TONE_MAPPING            = 1ULL << 29,

        FEATURE_COUNT
    };


    using FeaturesBitmask = uint64_t;


    const char* featureFlagToString(FeatureFlag flag);
}
#endif /* feature_flags_hpp */

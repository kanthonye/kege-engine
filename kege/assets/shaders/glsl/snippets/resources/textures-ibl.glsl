// This defines the IBL texture samplers for image-based lighting in the PBR shader
layout(set = IBL_SET, binding = 0) uniform sampler2D IrradianceMap; // Diffuse IBL
layout(set = IBL_SET, binding = 1) uniform sampler2D PrefilterMap; // Specular IBL
layout(set = IBL_SET, binding = 2) uniform sampler2D BRDF_LUT; // BRDF lookup


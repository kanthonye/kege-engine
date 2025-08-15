#version 450

/**
 * tells the driver how to distribute the workload on the GPU. In our case, we are processing tiles of 16x16 pixels
 */
layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

/**
 * The only buffer binding that we need to specify is the output image. Image2D means that the result
 * variable is a two-dimensional array whose elements are pixels. The writeonly layout qualifier instructs
 * the compiler to assume we will not read from this image in the shader:
 */
layout(set = 0, binding = 0, r32f) uniform writeonly image2D Heightmap;

/**
 * CubeFacePoints are the 6 quads that make forms a cube. Each quad is made up of 4 points
 * is represented as a mat4 matrix. There should be a total of 6 matrices.
 */
layout( set = 1, binding = 0 ) buffer CubeFacePoints
{
    mat4 faces[];
};

// layout( set = 2, binding = 0 ) buffer HeightmapParameters
// {
//     vec3  OFFSET;
//     int   OCTAVES = 6;
//     float PERSISTENCE = 0.5;
//     float LACUNARITY = 2.0;
//     float SCALE = 0.1;
//     float STEEPNESS = 2.0;
//     float STRENGTH = 1.0;
//     float SMAX = 6.0;
//     float SMIN = 0.0;
// };

/**
 * HeightmapParameters contains the parameters to modify the height map results.
 */
layout( push_constant ) uniform HeightmapParameters
{
    vec3  OFFSET;
    int   OCTAVES;
    float PERSISTENCE;
    float LACUNARITY;
    float SCALE;
    float STEEPNESS;
    float STRENGTH;
    float SMAX;
    float SMIN;
    int   FACE_INDEX;
    float NORMAL_MAP_STRENGTH;
};




vec3 random3(vec3 c) 
{
    float j = 4096.0 * sin(dot(c,vec3(17.0, 59.4, 15.0)));
    vec3 r;
    r.z = fract(512.0*j);
    j *= .125;
    r.x = fract(512.0*j);
    j *= .125;
    r.y = fract(512.0*j);
    return r-0.5;
}

/* skew constants for 3d simplex functions */
const float F3 =  0.3333333;
const float G3 =  0.1666667;

/* 3d simplex noise */
float simplex3d(vec3 p) 
{
     /* 1. find current tetrahedron T and it's four vertices */
     /* s, s+i1, s+i2, s+1.0 - absolute skewed (integer) coordinates of T vertices */
     /* x, x1, x2, x3 - unskewed coordinates of p relative to each of T vertices*/
     
     /* calculate s and x */
     vec3 s = floor(p + dot(p, vec3(F3)));
     vec3 x = p - s + dot(s, vec3(G3));
     
     /* calculate i1 and i2 */
     vec3 e = step(vec3(0.0), x - x.yzx);
     vec3 i1 = e*(1.0 - e.zxy);
     vec3 i2 = 1.0 - e.zxy*(1.0 - e);
         
     /* x1, x2, x3 */
     vec3 x1 = x - i1 + G3;
     vec3 x2 = x - i2 + 2.0*G3;
     vec3 x3 = x - 1.0 + 3.0*G3;
     
     /* 2. find four surflets and store them in d */
     vec4 w, d;
     
     /* calculate surflet weights */
     w.x = dot(x, x);
     w.y = dot(x1, x1);
     w.z = dot(x2, x2);
     w.w = dot(x3, x3);
     
     /* w fades from 0.6 at the center of the surflet to 0.0 at the margin */
     w = max(0.6 - w, 0.0);
     
     /* calculate surflet components */
     d.x = dot(random3(s), x);
     d.y = dot(random3(s + i1), x1);
     d.z = dot(random3(s + i2), x2);
     d.w = dot(random3(s + 1.0), x3);
     
     /* multiply d by w^4 */
     w *= w;
     w *= w;
     d *= w;
     
     /* 3. return the sum of the four surflets */
     return dot(d, vec4(64.0));
}

void main()
{
    ivec2 texel = ivec2( gl_GlobalInvocationID.xy );
    ivec2 dim = imageSize( Heightmap );

    // Check if the thread is within the bounds of the image
    if ( all( lessThanEqual( texel, dim ) ) )
    {
        vec2  uv = vec2(texel) / vec2(dim);
        mat4 face = faces[ FACE_INDEX ];
        
        /**
         * Here we interpolate across each quad points to calculate a position on the
         * quad plane which is then use to calculate the noise value
         */
        vec3 p[3];
        p[0] = mix( face[ 1 ].xyz, face[ 0 ].xyz, uv.x );
        p[1] = mix( face[ 2 ].xyz, face[ 3 ].xyz, uv.x );
        p[2] = mix( p[ 0 ], p[ 1 ], uv.y );

        /**
         * Compute the noise value
         */
        float height = 1.0;
        p[2] = p[2] * SCALE + OFFSET;
        for ( int i = 0; i < OCTAVES ; i++ )
        {
            float frequency = pow( LACUNARITY,  float(i) );
            float amplitude = pow( PERSISTENCE, float(i) );
            height += simplex3d( p[2] * frequency ) * amplitude;
        }

        height = max(0.0, height);
        /**
         * The noise value is stored in the alpha channel of the texture
         */
        //height = (height * 0.5 + 0.5);
        height = pow( height, STEEPNESS );
        //height = height * STRENGTH;
        //height = smoothstep( SMIN, SMAX, height );

       imageStore( Heightmap, texel, vec4(vec3(height) / 6.0,1.0) );
    }
    return;
}

# Scene File Format Documentation

This document describes the JSON scene file format used by YourEngine. This format is designed to define and manage various resources and entities within a 3D scene, providing a structured way to store scene data.

The scene file is a JSON array, where each element in the array represents a distinct resource manager or a global scene setting.

-   **`id` and `data` Pattern**: All resource types (`cameras`, `lights`, `materials`, `textures`, `meshs`, `effects`) will follow the `id` and `data` pattern. The example for `cameras` will be adjusted to reflect this.

-   **"Target" Mechanism**: The `target` mechanism shown in the `entities` section is the correct and consistent way to reference pre-defined resources. The `materials` section's `textures` reference will be updated to match this.

-   **Default Values**: There are no implicit default values for any properties. If a property isn't specified, it's assumed to be undefined or will result in an error, rather than falling back to a default.
-   **Units**: The `defaultUnits` specified in the `meta` section apply globally to all spatial properties within the scene file.

___
## Meta
The `meta` section provides general information about the scene file itself, such as its format version and the generating application.

#### Purpose
This section ensures compatibility and provides essential metadata for parsing and interpreting the scene file.

### Structure
```
{
    "type": "meta",
    "meta":
    {
        "type": "meta",
        "formatVersion": "1.0",
        "generator": "YourEngine",
        "defaultUnits": "meters"
    }
}
```
### Elements

**`type`**:

- **Purpose**: Identifies this block as containing meta-information.
- **Expected Value**: String, always `"meta"`.
- **Usage**: Fixed string value.

**`meta`**:

- **Purpose**: Contains the actual metadata properties.
- **Expected Value**: Object.
- **Usage**: This object holds the following properties.

- **`type`**:

    -   **Purpose**: Redundantly identifies the content type within the `meta` object.
    -   **Expected Value**: String, always `"meta"`.
    -   **Usage**: Fixed string value.

-   **`formatVersion`**:

    -   **Purpose**: Specifies the version of the scene file format. This is crucial for backward compatibility.
    -   **Expected Value**: String (e.g., `"1.0"`).
    -   **Usage**: Defines the expected parser version.

-   **`generator`**:

    - **Purpose**: Identifies the application or tool that generated this scene file.
    - **Expected Value**: String (e.g., `"YourEngine"`, `"BlenderExporter"`).
     - **Usage**: Informational field.

-   **`defaultUnits`**:

    -   **Purpose**: Defines the default unit of measurement for all spatial properties (positions, sizes, radii, distances, etc.) throughout the entire scene file, unless explicitly overridden.
    -   **Expected Value**: String (e.g., `"meters"`, `"centimeters"`, `"inches"`).
    -   **Usage**: All numerical values representing distances or sizes should adhere to this unit.

___
## Cameras

The `cameras` section defines a list of camera that can be referenced by your entities. These cameras act as viewpoints, determining what is visible and how it's projected onto the screen.

**Purpose**

This section allows you to pre-define various camera perspectives (e.g., for different views, cinematic shots, or gameplay cameras) that can then be referenced by entities in the scene.

### Structure
```
{
    "type": "cameras",
    "cameras":
    [
        {
            "id":"persp-camera",
            "data":
            {
                "type": "<projection-type>",
                "<projection-type>":
                {...}
            }
        }
    ]
}
```
**Elements**

**`id`**:

- **Purpose**: A unique identifier for this camera resource. This ID is used by `camera-component`s within `entities` to reference this specific camera.
- **Expected Value**: String.
- **Usage**: Must be unique across all camera resources. Example: `"player-camera"`, `"scene-camera"`.

**`data`**:

- **Purpose**: Contains the specific properties for this camera.
- **Expected Value**: Object.
- **Usage**: This object defines the camera's projection type and its parameters:

  - **`type`**:

    - **Purpose**: Specifies the projection type of the camera.
    - **Expected Value**: String. Can be either `"perspective"` or `"orthographic"`.
    - **Usage**: Determines which sub-object (`perspective` or `orthographic`) will contain the camera's detailed settings.


#### Defining Perspective Projection
```
{
    "type": "perspective",
    "perspective":
    {
        "aspect_ratio": 1.3,
        "fov": 45,
        "znear": 0.1,
        "zfar": 10000.0
    }
}
```
Properties for a **perspective projection** camera.
**`perspective`**:

- **`aspect_ratio`**:

  - **Purpose**: The aspect ratio of the camera's viewport (width divided by height).
  - **Expected Value**: Number (float).
  - **Usage**: A common value for a 16:9 screen is `1.777`.

- **`fov`**:

  - **Purpose**: The field of view in degrees, defining the vertical angle of the camera's vision.
  - **Expected Value**: Number (float).
  - **Usage**: Higher values create a wider, more distorted view.

- **`znear`**:

  - **Purpose**: The distance from the camera to the near clipping plane. Objects closer than this will not be rendered.
  - **Expected Value**: Number (float).
  - **Usage**: Typically a small positive value (e.g., `0.1`).

- **`zfar`**:

  - **Purpose**: The distance from the camera to the far clipping plane. Objects farther than this will not be rendered.
  - **Expected Value**: Number (float).
  - **Usage**: Should be a large value to encompass the scene (e.g., `10000.0`).

#### Defining Orthographic Projection
```
{
    "type": "orthographic",
    "orthographic":
    {
        "left": -200,
        "right": 200,
        "bottom": -200,
        "top": 200,
        "znear": 0.1,
        "zfar": 10000.0
    }
}
```
Properties for an **orthographic projection** camera.

**`orthographic`**:

- **`left`**:
  - **Purpose**: The x-coordinate of the left side of the orthographic view volume.
  - **Expected Value**: Number (float).

- **`right`**:
  - **Purpose**: The x-coordinate of the right side of the orthographic view volume.
  - **Expected Value**: Number (float).

- **`bottom`**:
  - **Purpose**: The y-coordinate of the bottom side of the orthographic view volume.
  - **Expected Value**: Number (float).

- **`top`**:
  - **Purpose**: The y-coordinate of the top side of the orthographic view volume.
  - **Expected Value**: Number (float).

- **`znear`**:
- **Purpose**: The distance from the camera to the near clipping plane.
- **Expected Value**: Number (float).

- **`zfar`**:
  - **Purpose**: The distance from the camera to the far clipping plane.
  - **Expected Value**: Number (float).

#### Example:
A compltete example of usage of the cameras section. Defining a perspective camera resource.
```
{
    "type": "cameras",
    "cameras":
    [
        {
            "id":"persp-camera",
            "data":
            {
                "type": "perspective",
                "perspective":
                {
                    "aspect_ratio": 1.3,
                    "fov": 45,
                    "znear": 0.1,
                    "zfar": 10000.0
                }
            }
        }
    ]
}
```
___
## Lights

The `lights` section defines a list of light resources. Lights which can then be referenced by entities in the scene. These lights illuminate the scene, affecting how objects appear and cast shadows. This section allows you to pre-define various types of light sources with their specific properties,

### Structure
```
{
    "type": "lights",
    "lights":
    [
        {...},
        {...},
        {...}
    ]
}
```
**Elements**

**`type`**:
  -   **Purpose**: Identifies this block as containing light definitions.
  -   **Expected Value**: String, always `"lights"`.
  -   **Usage**: Fixed string value.

**`lights`**:
  -   **Purpose**: An array holding individual light source definitions.
  -   **Expected Value**: Array of objects.
  -   **Usage**: Each object in this array defines a single light:

To define a light object you first need to define the resource that will hold the light object. This will contain an **`id`** and a **`data`** parameter.

```
{
    "id":"main-light",
    "data":
    {
        "type": "<type-name>",
        "type-name":
        {}
    }
}
```
**`id`**:

  - **Purpose**: A unique identifier for this light resource. This ID is used by `light-component`s within `entities` to reference this specific light.
  - **Expected Value**: String.
  - **Usage**: Must be unique across all light resources. Example: `"sun-light"`, `"room-lamp"`.

**`data`**:

  - **Purpose**: Contains the specific properties for this light source.
  - **Expected Value**: Object.
  - **Usage**: This object defines the light's type and its parameters:

**`type`**:
  - **Purpose**: Specifies the type of the light source.
  - **Expected Value**: String. Can be `"directional"`, `"point"`, or `"spot"`.
  - **Usage**: Determines which properties are relevant for the light.

### Directional Light

Defining the **`data`** parameters for directional light.

```
{
    "id":"sun-light",
    "data":
    {
        "type": "directional",
        "directional":
        {
            "color": [1.0, 0.9, 0.8],
            "direction":[1.0, 0.9, 0.8]
        }
    }
}
```

**`color`**:
  - **Purpose**: The color of the light in RGB format.
  - **Expected Value**: Array of three numbers (floats), representing [Red, Green, Blue] values from `0.0` to `1.0`.
  - **Usage**: Example: `[1.0, 1.0, 1.0]` for white light.

**`direction`**:
  - **Purpose**: (For `directional` lights) The direction vector of the light. This light illuminates uniformly from a specific direction, like the sun.
  - **Expected Value**: Array of three numbers (floats), representing [X, Y, Z] components of the direction vector.
  - **Usage**: Only present if `"type"` is `"directional"`. The length of the vector does not matter, only its direction.

### Point Light

Defining the **`data`** parameters for point light.

```
{
    "id":"lamp-light",
    "data":
    {
        "type": "point",
        "point":
        {
            "color": [1.0, 0.9, 0.8],
            "cconstant_attenuation": 1.0,
            "linear_attenuation": 0.20,
            "quadratic_attenuation": 0.125
        }
    }
}
```

**`color`**:

  - **Purpose**: The color of the light in RGB format.
  - **Expected Value**: Array of three numbers (floats), representing [Red, Green, Blue] values from `0.0` to `1.0`.
  - **Usage**: Example: `[1.0, 1.0, 1.0]` for white light.

**`cconstant_attenuation`**:

  - **Purpose**: (For `directional` lights) The direction vector of the light. This light illuminates uniformly from a specific direction, like the sun.
  - **Expected Value**: Array of three numbers (floats), representing [X, Y, Z] components of the direction vector.
  - **Usage**: Only present if `"type"` is `"directional"`. The length of the vector does not matter, only its direction.

**`linear_attenuation`**:

  - **Purpose**: The brightness of the light.
  - **Expected Value**: Number (float).
  - **Usage**: Higher values mean brighter light.

**`quadratic_attenuation`**:

  - **Purpose**: (For `directional` lights) The direction vector of the light. This light illuminates uniformly from a specific direction, like the sun.
  - **Expected Value**: Array of three numbers (floats), representing [X, Y, Z] components of the direction vector.
  - **Usage**: Only present if `"type"` is `"directional"`. The length of the vector does not matter, only its direction.

### Spot Light

Defining the **`data`** parameters for spot light.

```
{
    "id":"flash-light",
    "data":
    {
        "type": "spot",
        "spot":
        {
            "color": [1.0, 0.9, 0.8],
            "direction":[1.0, 0.9, 0.8]
            "exponent":4,
            "cutoff": 1,
            "cconstant_attenuation": 1.0,
            "linear_attenuation": 0.20,
            "quadratic_attenuation": 0.125
        }
    }
}
```

**`color`**:

  - **Purpose**: The color of the light in RGB format.
  - **Expected Value**: Array of three numbers (floats), representing [Red, Green, Blue] values from `0.0` to `1.0`.
  - **Usage**: Example: `[1.0, 1.0, 1.0]` for white light.

**`direction`**:

  - **Purpose**: (For `directional` lights) The direction vector of the light. This light illuminates uniformly from a specific direction, like the sun.
  - **Expected Value**: Array of three numbers (floats), representing [X, Y, Z] components of the direction vector.
  - **Usage**: Only present if `"type"` is `"directional"`. The length of the vector does not matter, only its direction.

**`exponent`**:

  - **Purpose**: (For `spot` lights) The inner angle (in degrees) of the spot light's cone. Light within this angle has full intensity.
  - **Expected Value**: Number (float).
  - **Usage**: Only present if `"type"` is `"spot"`.

**`cutoff`**:

  - **Purpose**: (For `spot` lights) The outer angle (in degrees) of the spot light's cone. Light intensity falls off between the inner and outer angles.
  - **Expected Value**: Number (float).
  - **Usage**: Only present if `"type"` is `"spot"`. Must be greater than or equal to `cone_inner_angle`.

**`cconstant_attenuation`**:

  - **Purpose**: The brightness of the light.
  - **Expected Value**: Number (float).
  - **Usage**: Higher values mean brighter light.

**`linear_attenuation`**:

  - **Purpose**: The brightness of the light.
  - **Expected Value**: Number (float).
  - **Usage**: Higher values mean brighter light.

**`quadratic_attenuation`**:

  - **Purpose**: The brightness of the light.
  - **Expected Value**: Number (float).
  - **Usage**: Higher values mean brighter light.

___
## Materials

The `materials` section defines various surface properties and visual characteristics for objects in your scene. These definitions control how light interacts with the surfaces, affecting their color, shininess, roughness, and other visual attributes. This section allows you to create reusable material definitions (like "rusty metal" or "glowing orb") that can be applied to different meshes or entities within the scene, ensuring consistent appearance and efficient asset management

```
{
    "type": "materials",
    "materials":
    [
    ]
}
```

**`type`**:

- **Purpose**: Identifies this block as containing material definitions.
- **Expected Value**: String, always `"materials"`.
- **Usage**: Fixed string value.

**`materials`**:

- **Purpose**: An array holding individual material resource definitions.
- **Expected Value**: Array of objects.
- **Usage**: Each object in this array defines a single material:

```
{
    "id":"rusty_metal",
    "data":
    {
        "type": "<type-name>",
        "type-name":
        {}
    }
}
```

**`id`**:

- **Purpose**: A unique identifier for this material resource. This ID is used by `material-component`s within `entities` to reference this specific material.
- **Expected Value**: String.
- **Usage**: Must be unique across all material resources. Example: `"plastic_red"`, `"glass_clear"`.

**`data`**:

- **Purpose**: Contains the specific properties for this material.
- **Expected Value**: Object.
- **Usage**: This object defines the material's rendering model and its parameters:

**`type`**:

- **Purpose**: Specifies the type of the material rendering model.
- **Expected Value**: String. Common types might include `"PBR"` (Physically Based Rendering), `"Ambient"`, `"Phong"`, `"Lambert"`, etc. The example shows `"PBR"` and `"Ambient"`.
- **Usage**: Determines which sub-object (e.g., `"PBR"`, `"Ambient"`) contains the material's detailed settings.

```
{
    "type": "materials",
    "materials":
    [
        {
            "id":"rusty_metal",
            "data":
            {
                "type": "PBR",
                "PBR":
                {
                    "properties":
                    {
                        "albedo": [1, 1, 1, 1],
                        "roughness": 0.7,
                        "metallic": 1.0
                    },
                    "textures":
                    {
                        "albedo_map":
                        {
                            "type": "source",
                            "source":
                            {
                                "target": "textures",
                                "id": "rusty_metal"
                            }
                        },
                        "normal_map":
                        {
                            "type": "source",
                            "source":
                            {
                                "target": "textures",
                                "id": "rusty_metal"
                            }
                        }
                    },
                    "passes":
                    [
                        {
                            "pass": "main",
                            "shader": "shaders/pbr.json"
                        },
                        {
                            "pass": "shadow",
                            "shader": "shaders/shadow.json",
                        }
                    ],
                    "shader_defines":
                    [
                        "HAS_BASE_COLOR_MAP",
                        "HAS_ROUGHNESS_PARAM",
                        "USE_FOG",
                        "HAS_SPECULAR"
                    ]
                }
            }
        }
    ]
}
```
**`PBR`**:

  - **Purpose**: Defines properties for a Physically Based Rendering material.
  - **Expected Value**: Object.
  - **Usage**: Only present if `"type"` is `"PBR"`.

- **`properties`**:

  - **Purpose**: Numerical or color properties that define the material's core appearance.
  - **Expected Value**: Object.
  - **Usage**:

- **`albedo`**:

  - **Purpose**: The base color of the material, also known as diffuse color. Can include an alpha channel.
  - **Expected Value**: Array of four numbers (floats) `[R, G, B, A]`, from `0.0` to `1.0`.

- **`roughness`**:

  - **Purpose**: How rough the surface is. A higher value leads to a wider and softer specular reflection (less shiny).
  - **Expected Value**: Number (float), from `0.0` (smooth) to `1.0` (rough).

- **`metallic`**:

  - **Purpose**: How metallic the surface is. `0.0` is dielectric (non-metal), `1.0` is metallic.
  - **Expected Value**: Number (float), from `0.0` to `1.0`.

- **`textures`**:

  - **Purpose**: References to texture maps that provide detailed surface information.
  - **Expected Value**: Object.
  - **Usage**: Each property within `textures` refers to a specific texture map type:

- **`albedo_map`**:

  - **Purpose**: A texture defining the base color of the surface.
  - **Expected Value**: Object with `type: "source"` and `source` object.
  - **Usage (Target Reference)**:

___
## Textures

The `textures` section defines all the image resources that can be used within your scene. These images are typically applied to materials to provide visual detail like color, normal mapping, roughness, or other surface properties.

#### Purpose

This section allows you to pre-load and manage various image files (e.g.,  `.png`,  `.jpg`,  `.dds`) as reusable texture resources. Materials then reference these textures by their unique IDs. Below are 3 example detailing texture resources

#### Structure
```
{
    "type": "textures",
    "textures":
    [
        {
            "id": "rusty_metal",
            "data":
            {
                "type": "image_file",
                "image_file":
                {
                    "path": "textures/rusty_metal.png",
                    "wrap_mode": "clamp",
                    "filter": "linear"
                }
            }
        },
        {
            "id": "metal",
            "data":
            {
                "type": "image_data",
                "texture_data":
                {
                    "data": "base64encodeddata...",
                    "format": "png",
                    "width": 64,
                    "height": 64,
                    "mip_levels": 1,
                    "wrap_mode": "clamp",
                    "filter": "linear"
                }
            }
        },
        {
            "id": "procedural_noise",
            "data":
            {
                "type": "procedural_noise_pattern",
                "procedural_noise_pattern":
                {
                    "parameters":
                    {
                        "size":
                        {
                            "width": 256,
                            "height": 256
                        },
                        "planes":
                        [
                            {
                                "id": "front",
                                "type": "axes",
                                "axes":
                                [
                                    [1,0,0],
                                    [0,1,0]
                                ]
                            }
                        ],
                        "layers":
                        [
                            {
                                "name": "base",
                                "noise": "ridged_multifractal_noise",
                                "seed": 42,
                                "frequency": 0.01,
                                "amplitude": 1.0,
                                "octaves": 4,
                                "lacunarity": 2.0,
                                "persistence": 0.5,
                                "offset": [0,5,79],
                                "seamless": true
                            },
                            {
                                "name": "mountains",
                                "noise": "ridged_multifractal_noise",
                                "seed": 42,
                                "frequency": 0.01,
                                "amplitude": 1.0,
                                "octaves": 4,
                                "lacunarity": 2.0,
                                "persistence": 0.5,
                                "offset": [0,5,79],
                                "seamless": true
                            }
                        ]
                    },
                    "post_processing":
                    {
                        "smoothingPasses": 1,
                        "clamp":
                        {
                            "min": 0.0,
                            "max": 1.0
                        }
                    },
                    "output":
                    {
                        "mip_levels": 1,
                        "format": "rgb_i8",
                        "wrap_mode": "clamp",
                        "filter": "linear",
                    }
                }
            }
        }
    ]
}
```
#### Elements

**`type`**:

- **Purpose**: Identifies this block as containing texture definitions.
- **Expected Value**: String, always `"textures"`.
- **Usage**: Fixed string value.

**`textures`**:

- **Purpose**: An array holding individual texture resource definitions.

- **Expected Value**: Array of objects.

- **Usage**: Each object in this array defines a single texture:

- **`id`**:

    - **Purpose**: A unique identifier for this texture resource. This ID is used by `materials` (and potentially other components) to reference this specific texture.

    - **Expected Value**: String.

    - **Usage**: Must be unique across all texture resources. Example: `"wall_brick_diffuse"`, `"water_normal_map"`.

- **`data`**:

    - **Purpose**: Contains the specific properties for this texture.

    - **Expected Value**: Object.

    - **Usage**: This object defines the texture's source and how it should be interpreted and sampled:


### Data Object - `image_file`

**Example:**
```
{
    "type": "image_file",
    "image_file":
    {
        "path": "textures/rusty_metal.png",
        "wrap_mode": "clamp",
        "filter": "linear"
    }
}
```

**`type`**:

- **Purpose**: Identifies this block as containing texture definitions.
- **Expected Value**: String, always `"textures"`.
- **Usage**: Fixed string value.

**`image_file`**:

- **`path`**:

    - **Purpose**: The file path to the image asset.
    - **Expected Value**: String (path).
    - **Usage**: This path is relative to the scene file or a defined asset root.


### Data Object - `image_data`

**Example:**
```
    {
        "type": "image_data",
        "texture_data":
        {
            "data": "base64encodeddata...",
            "format": "png",
            "width": 64,
            "height": 64,
            "mip_levels": 1,
            "wrap_mode": "clamp",
            "filter": "linear"
        }
    }
```


**`type`**:

- **Purpose**: Identifies this block as containing texture definitions.
- **Expected Value**: String, always `"textures"`.
- **Usage**: Fixed string value.

**`image_file`**:

-   **`type`**:

    -   **Purpose**: (Optional) Specifies special texture types like cubemaps or 3D textures. If omitted, it defaults to a 2D texture.

    -   **Expected Value**: String
        - `image_1d`
        - `image_2d`
        - `image_3d`
        - `image_1d_array`
        - `image_2d_array`
        - `image_cube`
        - `image_cube_array`

- **`data`**:

    - The unsigned byte data of the image.

- **`format`**:

    - The internal format the texture should use when loaded into GPU memory. This affects memory usage and quality. Choose a format appropriate for the image content and desired quality/performance.

    - **Expected Value**: String. Common values include:

        - `"rgba_i8"`, `"rgba_u8"`: 8 bits per channel for Red, Green, Blue, Alpha (standard).
        - `"rgb_i8"`, `"rgb_u8"`: 8 bits per channel for Red, Green, Blue.
        - `"rg_i8"`, `"rg_u8"`: 8 bits per channel for Red, Green.
        - `"r_i8"`, `"r_u8"`: 8-bit single channel (e.g., for grayscale masks).

        - `"SRGBA8"`: sRGB version of RGBA8 (for color textures).

        - `"RGBA_BC1"`, `"RGBA_BC3"`, `"RGBA_BC7"`: Various compressed texture formats.

-   **`width`**:
-   **`height`**:
-   **`mipmaps`**:

    -   **Purpose**: Specifies whether mipmaps should be generated for this texture. Mipmaps are smaller, pre-filtered versions of the texture used for objects further away, improving rendering performance and reducing aliasing.

    -   **Expected Value**: Boolean (`true` or `false`).

    -   **Usage**: Generally `true` for diffuse/albedo textures, often `false` for normal maps or if a specific mipmap chain is already provided in the asset (e.g., `.dds`).

-   **`wrap_s`**:

    -   **Purpose**: Defines the wrapping behavior along the S (horizontal) texture coordinate axis when the coordinate falls outside the `[0, 1]` range.

    -   **Expected Value**: String. Common values:

        -   `"repeat"`: The texture repeats.

        -   `"clamp_to_edge"`: The edge pixels are extended.

        -   `"mirrored_repeat"`: The texture repeats, mirroring each consecutive repetition.

    -   **Usage**: Useful for tiling textures (`repeat`) or preventing artifacts at UV seams (`clamp_to_edge`).

-   **`wrap_t`**:

    -   **Purpose**: Defines the wrapping behavior along the T (vertical) texture coordinate axis.

    -   **Expected Value**: String. Same values as `wrap_s`.

    -   **Usage**: Works in conjunction with `wrap_s`.

-   **`filter_min`**:

    -   **Purpose**: Defines the filtering method used when the texture is minified (rendered smaller than its native resolution).

    -   **Expected Value**: String. Common values:

        -   `"nearest"`: Nearest pixel sampling (blocky).

        -   `"linear"`: Bilinear filtering (blurry).

        -   `"nearest_mipmap_nearest"`: Nearest mipmap, then nearest pixel.

        -   `"linear_mipmap_nearest"`: Nearest mipmap, then linear filtering.

        -   `"nearest_mipmap_linear"`: Linear interpolation between mipmaps, then nearest pixel.

        -   `"linear_mipmap_linear"`: Trilinear filtering (linear between mipmaps, then linear).

    -   **Usage**: `linear_mipmap_linear` provides the best visual quality for minified textures.

-   **`filter_mag`**:

    -   **Purpose**: Defines the filtering method used when the texture is magnified (rendered larger than its native resolution).

    -   **Expected Value**: String. Common values:

        -   `"nearest"`: Nearest pixel sampling.

        -   `"linear"`: Bilinear filtering.

    -   **Usage**: `linear` is generally preferred for smoother magnification.

#### Data Object - Image Generation
**Example:**
```
{
    "id": "procedural_noise",
    "data":
    {
        "type": "procedural_noise_pattern",
        "procedural_noise_pattern":
        {
            "parameters":
            {
                "size":
                {
                    "width": 256,
                    "height": 256
                },
                "planes":
                [
                    {
                        "id": "front",
                        "type": "axes",
                        "axes":
                        [
                            [1,0,0],
                            [0,1,0]
                        ]
                    }
                ],
                "layers":
                [
                    {
                        "name": "base",
                        "noise": "ridged_multifractal_noise",
                        "seed": 42,
                        "frequency": 0.01,
                        "amplitude": 1.0,
                        "octaves": 4,
                        "lacunarity": 2.0,
                        "persistence": 0.5,
                        "offset": [0,5,79],
                        "seamless": true
                    },
                    {
                        "name": "mountains",
                        "noise": "ridged_multifractal_noise",
                        "seed": 42,
                        "frequency": 0.01,
                        "amplitude": 1.0,
                        "octaves": 4,
                        "lacunarity": 2.0,
                        "persistence": 0.5,
                        "offset": [0,5,79],
                        "seamless": true
                    }
                ]
            },
            "post_processing":
            {
                "smoothingPasses": 1,
                "clamp":
                {
                    "min": 0.0,
                    "max": 1.0
                }
            },
            "output":
            {
                "mip_levels": 1,
                "format": "rgb_i8",
                "wrap_mode": "clamp",
                "filter": "linear"
            }
        }
    }
}
```
___
## Meshes

The `meshes` section defines all the 3D geometric data (vertices, faces, normals, UVs, etc.) that can be rendered in your scene. These are the fundamental building blocks for all visible objects.

### Purpose

This section allows you to import and manage reusable 3D models. These meshes are then instanced by entities in the scene, which apply transformations, materials, and other components to them.

### Structure
```
{
    "type": "meshes",
    "meshes": [
        {
            "id": "cube_model",
            "data": {
                "path": "assets/models/cube.gltf",
                "format": "gltf",
                "primitives": [
                    {
                        "material_index": 0,
                        "attributes": {
                            "POSITION": "POSITION",
                            "NORMAL": "NORMAL",
                            "TEXCOORD_0": "TEXCOORD_0"
                        }
                    }
                ]
            }
        },
        {
            "id": "character_model",
            "data": {
                "path": "assets/models/character.glb",
                "format": "gltf",
                "primitives": [
                    {
                        "material_index": 0,
                        "attributes": {
                            "POSITION": "POSITION",
                            "NORMAL": "NORMAL",
                            "TEXCOORD_0": "TEXCOORD_0",
                            "JOINTS_0": "JOINTS_0",
                            "WEIGHTS_0": "WEIGHTS_0"
                        }
                    },
                    {
                        "material_index": 1,
                        "attributes": {
                            "POSITION": "POSITION",
                            "NORMAL": "NORMAL",
                            "TEXCOORD_0": "TEXCOORD_0"
                        }
                    }
                ]
            }
        }
    ]
}
```
----------

## Meshes

The `meshes` section defines all the 3D geometric data (vertices, faces, normals, UVs, etc.) that can be rendered in your scene. These are the fundamental building blocks for all visible objects.

### Purpose

This section allows you to import and manage reusable 3D models. These meshes are then instanced by entities in the scene, which apply transformations, materials, and other components to them.

### Structure

JSON

```
{
    "type": "meshes",
    "meshes": [
        {
            "id": "cube_model",
            "data": {
                "path": "assets/models/cube.gltf",
                "format": "gltf",
                "primitives": [
                    {
                        "material_index": 0,
                        "attributes": {
                            "POSITION": "POSITION",
                            "NORMAL": "NORMAL",
                            "TEXCOORD_0": "TEXCOORD_0"
                        }
                    }
                ]
            }
        },
        {
            "id": "character_model",
            "data": {
                "path": "assets/models/character.glb",
                "format": "gltf",
                "primitives": [
                    {
                        "material_index": 0,
                        "attributes": {
                            "POSITION": "POSITION",
                            "NORMAL": "NORMAL",
                            "TEXCOORD_0": "TEXCOORD_0",
                            "JOINTS_0": "JOINTS_0",
                            "WEIGHTS_0": "WEIGHTS_0"
                        }
                    },
                    {
                        "material_index": 1,
                        "attributes": {
                            "POSITION": "POSITION",
                            "NORMAL": "NORMAL",
                            "TEXCOORD_0": "TEXCOORD_0"
                        }
                    }
                ]
            }
        }
    ]
}

```

### Elements

-   **`type`**:

    -   **Purpose**: Identifies this block as containing mesh definitions.

    -   **Expected Value**: String, always `"meshes"`.

    -   **Usage**: Fixed string value.

-   **`meshes`**:

    -   **Purpose**: An array holding individual mesh resource definitions.

    -   **Expected Value**: Array of objects.

    -   **Usage**: Each object in this array defines a single 3D mesh:

        -   **`id`**:

            -   **Purpose**: A unique identifier for this mesh resource. This ID is used by `mesh-component`s within `entities` to reference this specific mesh.

            -   **Expected Value**: String.

            -   **Usage**: Must be unique across all mesh resources. Example: `"tree_pine_hi_res"`, `"rock_low_poly"`.

        -   **`data`**:

            -   **Purpose**: Contains the specific properties for this mesh, primarily its source file and structure.

            -   **Expected Value**: Object.

            -   **Usage**: This object defines where the mesh data comes from and how it's organized:

                -   **`path`**:

                    -   **Purpose**: The file path to the 3D model asset.

                    -   **Expected Value**: String (path).

                    -   **Usage**: This path is relative to the scene file or a defined asset root. Common formats include `.gltf`, `.glb`, `.obj`, `.fbx`.

                -   **`format`**:

                    -   **Purpose**: The format of the 3D model file. This helps the engine choose the correct loader.

                    -   **Expected Value**: String (e.g., `"gltf"`, `"obj"`, `"fbx"`).

                    -   **Usage**: Must match the file format of the `path`.

                -   **`primitives`**:

                    -   **Purpose**: An array defining the individual renderable sub-parts (primitives) of the mesh. A single model file can contain multiple primitives, each potentially using a different material.

                    -   **Expected Value**: Array of objects.

                    -   **Usage**: Each object in this array represents a primitive:

                        -   **`material_index`**:

                            -   **Purpose**: The index of the material to be applied to this primitive within the context of the mesh. This index usually refers to a material specified within the model file itself, or an engine's internal material lookup if the model provides material names.

                            -   **Expected Value**: Integer (non-negative).

                            -   **Usage**: Allows assigning different materials to different parts of a complex model (e.g., body and clothing on a character).

                        -   **`attributes`**:

                            -   **Purpose**: A map indicating which vertex attributes are present in this primitive and how they are named internally. This helps map generic attribute names (e.g., "POSITION") to specific data streams from the loaded model.

                            -   **Expected Value**: Object.

                            -   **Usage**: Each key is a standard attribute name (e.g., `"POSITION"`, `"NORMAL"`, `"TEXCOORD_0"`, `"COLOR_0"`, `"JOINTS_0"`, `"WEIGHTS_0"`), and its value is the corresponding name or identifier within the loaded model's data.

___
## Effects

The `effects` section defines various post-processing or special visual effects that can be applied in your scene. These typically modify the final rendered image or contribute to specific visual behaviors.

### Purpose

This section allows you to pre-configure different visual effects (e.g., fog, bloom, depth of field) that can then be activated or customized for specific cameras or parts of the scene.

```
{
    "type": "effects",
    "effects":
    [
        {
            "id": "global_fog",
            "data": {
                "type": "fog",
                "color": [0.7, 0.7, 0.7],
                "density": 0.05,
                "start_distance": 10.0,
                "end_distance": 100.0,
                "mode": "linear"
            }
        },
        {
            "id": "cinematic_bloom",
            "data": {
                "type": "bloom",
                "threshold": 1.5,
                "intensity": 0.8,
                "radius": 0.5
            }
        },
        {
            "id": "vignette_effect",
            "data": {
                "type": "vignette",
                "color": [0.0, 0.0, 0.0],
                "intensity": 0.5,
                "smoothness": 0.8
            }
        }
    ]
}
```
### Elements

-   **`type`**:

    -   **Purpose**: Identifies this block as containing effect definitions.

    -   **Expected Value**: String, always `"effects"`.

    -   **Usage**: Fixed string value.

-   **`effects`**:

    -   **Purpose**: An array holding individual effect resource definitions.

    -   **Expected Value**: Array of objects.

    -   **Usage**: Each object in this array defines a single effect:

        -   **`id`**:

            -   **Purpose**: A unique identifier for this effect resource. This ID could be used by `camera-component`s or other scene elements to enable or apply this effect.

            -   **Expected Value**: String.

            -   **Usage**: Must be unique across all effect resources. Example: `"depth_of_field_blur"`, `"motion_blur"`.

        -   **`data`**:

            -   **Purpose**: Contains the specific properties for this effect.

            -   **Expected Value**: Object.

            -   **Usage**: This object defines the effect's type and its parameters:

                -   **`type`**:

                    -   **Purpose**: Specifies the type of visual effect.

                    -   **Expected Value**: String. Examples include `"fog"`, `"bloom"`, `"vignette"`, `"depth_of_field"`, `"color_grading"`.

                    -   **Usage**: Determines which sub-object contains the effect's detailed settings.

                -   **`fog`**:

                    -   **Purpose**: Defines properties for a fog effect, which simulates atmospheric haze. This value is affected by `defaultUnits` in the `meta` section.

                    -   **Expected Value**: Object.

                    -   **Usage**: Only present if `"type"` is `"fog"`.

                        -   **`color`**:

                            -   **Purpose**: The color of the fog.

                            -   **Expected Value**: Array of three numbers (floats) `[R, G, B]`, from `0.0` to `1.0`.

                        -   **`density`**:

                            -   **Purpose**: How dense the fog is. Affects how quickly objects fade into the fog.

                            -   **Expected Value**: Number (float).

                        -   **`start_distance`**:

                            -   **Purpose**: The distance from the camera where the fog begins to appear.

                            -   **Expected Value**: Number (float).

                        -   **`end_distance`**:

                            -   **Purpose**: The distance from the camera where objects are fully obscured by fog.

                            -   **Expected Value**: Number (float).

                        -   **`mode`**:

                            -   **Purpose**: The type of fog falloff.

                            -   **Expected Value**: String. Common values: `"linear"`, `"exponential"`, `"exponential2"`.

                -   **`bloom`**:

                    -   **Purpose**: Defines properties for a bloom effect, which simulates light bleeding from bright areas.

                    -   **Expected Value**: Object.

                    -   **Usage**: Only present if `"type"` is `"bloom"`.

                        -   **`threshold`**:

                            -   **Purpose**: The luminance value above which pixels contribute to the bloom effect.

                            -   **Expected Value**: Number (float).

                        -   **`intensity`**:

                            -   **Purpose**: The overall strength of the bloom effect.

                            -   **Expected Value**: Number (float).

                        -   **`radius`**:

                            -   **Purpose**: The size or spread of the bloom effect.

                            -   **Expected Value**: Number (float).

                -   **`vignette`**:

                    -   **Purpose**: Defines properties for a vignette effect, which darkens the edges of the screen.

                    -   **Expected Value**: Object.

                    -   **Usage**: Only present if `"type"` is `"vignette"`.

                        -   **`color`**:

                            -   **Purpose**: The color of the vignette overlay.

                            -   **Expected Value**: Array of three numbers (floats) `[R, G, B]`, from `0.0` to `1.0`.

                        -   **`intensity`**:

                            -   **Purpose**: The strength of the darkening effect.

                            -   **Expected Value**: Number (float).

                        -   **`smoothness`**:

                            -   **Purpose**: How smoothly the vignette transitions from the center to the edges.

                            -   **Expected Value**: Number (float).

___
## Entities

The `entities` section is where the actual objects in your 3D scene are defined and composed. Each entity represents a distinct object, which can have a transformation (position, rotation, scale) and various components that give it behavior and visual properties by referencing the resources defined in the preceding sections (meshes, materials, lights, cameras, effects). This is the core of your scene composition, allowing you to instantiate 3D models, place lights and cameras, and define their relationships and properties within the world.

### Structure
```
{
    "type": "entities",
    "entities":
    [
        {
            "id": "player_character",
            "data":
            {
                "name": "Player Character",
                "transform":
                {
                    "position": [0, 0, 0],
                    "rotation": [0, 0, 0, 1],
                    "scale": [1, 1, 1]
                },
                "components":
                [
                    {...},
                    {...},
                    {...}
                ],
                "children":
                [
                    {
                        "id": "player_camera_pivot",
                        "data":
                        {
                            "name": "Player Camera Pivot",
                            "transform":
                            {
                                "position": [0, 1.8, 0],
                                "rotation": [0, 0, 0, 1],
                                "scale": [1, 1, 1]
                            },
                            "components":
                            [
                                {...},
                                {...},
                                {...}
                            ]
                        }
                    }
                ]
            }
        },
        {...},
        {...},
        {...}
    ]
}
```

**`type`**:

  - **Purpose**: Identifies this block as containing entity definitions.
  - **Expected Value**: String, always `"entities"`.
  - **Usage**: Fixed string value.

**`entities`**:

  - **Purpose**: An array holding individual entity definitions. Entities can be nested, forming a scene graph hierarchy.
  - **Expected Value**: Array of objects.
  - **Usage**: Each object in this array defines a single entity:

Each entity object has an `id`, `data`. and each **`id`** element has an entity `name`, entity `transform`, and the entity `components`.


**`id`**:

- **Purpose**: A unique identifier for this entity. Used for programmatic access or linking within the scene.
- **Expected Value**: String.
- **Usage**: Must be unique within its parent's children and at the root level.

**`data`**:

- **Purpose**: Contains the specific properties and components of this entity.
- **Expected Value**: Object.
- **Usage**: This object defines the entity's attributes:



```
{
    "type": "entities",
    "entities":
    [
        {
            "id": "player_character",
            "data":
            {
                "name": "Player Character",
                "transform":
                {
                    "position": [0, 0, 0],
                    "rotation": [0, 0, 0, 1],
                    "scale": [1, 1, 1]
                },
                "components":
                [
                    {
                        "type": "mesh-component",
                        "mesh":
                        {
                            "type": "source",
                            "source":
                            {
                                "location": "meshes",
                                "id": "character_model"
                            }
                        },
                        "material":
                        {
                            "type": "source",
                            "source":
                            {
                                "location": "materials",
                                "id": "rusty_metal"
                            }
                        }
                    },
                    {
                        "type": "light-component",
                        "light": {
                            "type": "source",
                            "source": {
                                "location": "lights",
                                "id": "point-lamp-light"
                            }
                        }
                    },
                    {
                        "type": "script-component",
                        "script": "scripts/player_controller.js"
                    }
                ],
                "children": [
                    {
                        "id": "player_camera_pivot",
                        "data": {
                            "name": "Player Camera Pivot",
                            "transform": {
                                "position": [0, 1.8, 0],
                                "rotation": [0, 0, 0, 1],
                                "scale": [1, 1, 1]
                            },
                            "components": [
                                {
                                    "type": "camera-component",
                                    "camera": {
                                        "type": "source",
                                        "source": {
                                            "location": "cameras",
                                            "id": "main-perspective-camera"
                                        }
                                    },
                                    "active": true
                                }
                            ]
                        }
                    }
                ]
            }
        },
        {
            "id": "static_cube",
            "data": {
                "name": "Level Cube",
                "transform": {
                    "position": [5, 0, 0],
                    "rotation": [0, 0.707, 0, 0.707],
                    "scale": [2, 2, 2]
                },
                "components": [
                    {
                        "type": "mesh-component",
                        "mesh": {
                            "type": "source",
                            "source": {
                                "location": "meshes",
                                "id": "cube_model"
                            }
                        },
                        "material": {
                            "type": "source",
                            "source": {
                                "location": "materials",
                                "id": "glowing_orb"
                            }
                        }
                    }
                ]
            }
        },
        {
            "id": "scene_directional_light_entity",
            "data": {
                "name": "Sun Light Entity",
                "transform": {
                    "position": [0, 10, 0],
                    "rotation": [-0.707, 0, 0, 0.707],
                    "scale": [1, 1, 1]
                },
                "components": [
                    {
                        "type": "light-component",
                        "light": {
                            "type": "source",
                            "source": {
                                "location": "lights",
                                "id": "directional-sun-light"
                            }
                        },
                        "active": true
                    }
                ]
            }
        }
    ]
}
```
### Elements

                -   **`name`**:

                    -   **Purpose**: A human-readable name for the entity, useful for debugging or scene editors.
                    -   **Expected Value**: String.
                    -   **Usage**: Optional.

                -   **`transform`**:

                    -   **Purpose**: Defines the entity's local position, rotation, and scale relative to its parent. All spatial values are affected by `defaultUnits` in the `meta` section.

                    -   **Expected Value**: Object.

                    -   **Usage**:

                        -   **`position`**:
                            -   **Purpose**: The entity's translation in 3D space.
                            -   **Expected Value**: Array of three numbers (floats) `[X, Y, Z]`.

                        -   **`rotation`**:

                            -   **Purpose**: The entity's rotation expressed as a quaternion.
                            -   **Expected Value**: Array of four numbers (floats) `[X, Y, Z, W]`.

                            -   **Usage**: Represents a rotation around an axis (X, Y, Z) with a W component.

                        -   **`scale`**:

                            -   **Purpose**: The entity's scaling factors along each axis.
                            -   **Expected Value**: Array of three numbers (floats) `[X, Y, Z]`.
                            -   **Usage**: A value of `1` means no scaling.

                -   **`components`**:

                    -   **Purpose**: An array of components attached to this entity, providing its functionality and appearance.

                    -   **Expected Value**: Array of objects.

                    -   **Usage**: Each object defines a component:

                        -   **`type`**:

                            -   **Purpose**: The type of component (e.g., `"mesh-component"`, `"light-component"`, `"camera-component"`, `"script-component"`).

                            -   **Expected Value**: String.

                        -   **`mesh-component`**:

                            -   **Purpose**: Attaches a 3D mesh resource to the entity for rendering.

                            -   **Expected Value**: Object.

                            -   **Usage**:

                                -   **`mesh`**:

                                    -   **Purpose**: Reference to a mesh defined in the `meshes` section.

                                    -   **Expected Value**: Object with `type: "source"` and `source` object.

                                    -   **Usage (Target Reference)**:

                                        JSON

                                        ```
                                        "mesh": {
                                            "type": "source",
                                            "source": {
                                                "location": "meshes",
                                                "id": "mesh_id_from_meshes_section"
                                            }
                                        }

                                        ```

                                        -   **`location`**: String, always `"meshes"`.

                                        -   **`id`**: String, the `id` of a mesh.

                                -   **`material`**:

                                    -   **Purpose**: Reference to a material defined in the `materials` section, applied to the mesh. This will override any default material specified in the mesh itself.

                                    -   **Expected Value**: Object with `type: "source"` and `source` object.

                                    -   **Usage (Target Reference)**:

                                        JSON

                                        ```
                                        "material": {
                                            "type": "source",
                                            "source": {
                                                "location": "materials",
                                                "id": "material_id_from_materials_section"
                                            }
                                        }

                                        ```

                                        -   **`location`**: String, always `"materials"`.

                                        -   **`id`**: String, the `id` of a material.

                        -   **`light-component`**:

                            -   **Purpose**: Attaches a light source defined in the `lights` section to the entity. The entity's transform will define the light's position/direction.

                            -   **Expected Value**: Object.

                            -   **Usage**:

                                -   **`light`**:

                                    -   **Purpose**: Reference to a light defined in the `lights` section.

                                    -   **Expected Value**: Object with `type: "source"` and `source` object.

                                    -   **Usage (Target Reference)**:

                                        JSON

                                        ```
                                        "light": {
                                            "type": "source",
                                            "source": {
                                                "location": "lights",
                                                "id": "light_id_from_lights_section"
                                            }
                                        }

                                        ```

                                        -   **`location`**: String, always `"lights"`.

                                        -   **`id`**: String, the `id` of a light.

                                -   **`active`**:

                                    -   **Purpose**: Whether this light component is currently enabled.

                                    -   **Expected Value**: Boolean (`true` or `false`).

                                    -   **Usage**: Optional, defaults to `true` if omitted.

                        -   **`camera-component`**:

                            -   **Purpose**: Attaches a camera definition from the `cameras` section to the entity. The entity's transform defines the camera's viewpoint.

                            -   **Expected Value**: Object.

                            -   **Usage**:

                                -   **`camera`**:

                                    -   **Purpose**: Reference to a camera defined in the `cameras` section.
                                    -   **Expected Value**: Object with `type: "source"` and `source` object.
                                    -   **Usage (Target Reference)**:

                                        JSON

                                        ```
                                        "camera": {
                                            "type": "source",
                                            "source": {
                                                "location": "cameras",
                                                "id": "camera_id_from_cameras_section"
                                            }
                                        }

                                        ```

                                        -   **`location`**: String, always `"cameras"`.

                                        -   **`id`**: String, the `id` of a camera.

                                -   **`active`**:

                                    -   **Purpose**: Whether this camera component is currently the active rendering camera. Only one camera should typically be active at a time.
                                    -   **Expected Value**: Boolean (`true` or `false`).
                                    -   **Usage**: Optional, defaults to `false` if omitted.

                        -   **`script-component`**:

                            -   **Purpose**: Attaches a script to the entity, allowing for custom logic and behavior.

                            -   **Expected Value**: Object.

                            -   **Usage**:

                                -   **`script`**:

                                    -   **Purpose**: The file path to the script asset.
                                    -   **Expected Value**: String (path).
                                    -   **Usage**: This path is relative to the scene file or a defined asset root.

                -   **`children`**:

                    -   **Purpose**: An optional array of child entities. Child entities inherit the transform of their parent and are transformed relative to it, forming a scene graph hierarchy.

                    -   **Expected Value**: Array of objects (each following the same entity structure).

                    -   **Usage**: Allows for complex object compositions (e.g., a character entity with child entities for its weapons or attached cameras).


## Entity Component

###  `transform-component`

The Transform Component is an attachable entity component that defines an entity's position, orientation, and scale in 3D space. Defines the entity's local position, rotation, and scale relative to its parent. All spatial values are affected by `defaultUnits` in the `meta` section.

- **`transform`**:

    -   **`position`**:
        -   **Purpose**: The entity's translation in 3D space.
        -   **Expected Value**: Array of three numbers (floats) `[X, Y, Z]`.
    -   **`rotation`**:
        -   **Purpose**: The entity's rotation expressed as a quaternion.
        -   **Expected Value**: Array of four numbers (floats) `[X, Y, Z, W]`.
        -   **Usage**: Represents a rotation around an axis (X, Y, Z) with a W component.
    -   **`scale`**:
        -   **Purpose**: The entity's scaling factors along each axis.
        -   **Expected Value**: Array of three numbers (floats) `[X, Y, Z]`.
        -   **Usage**: A value of `1` means no scaling.

Example:
```
{
    "type": "transform-component",
    "transform-component":
    {
        "type": "transform",
        "transform":
        {
            "position": [0, 10, 0],
            "rotation": [0, 45, 0],
            "scale": [1, 1, 1]
        }
    }
}
```
___
###  `camera-component`

The `camera-component` is an essential attachable entity component that transforms an otherwise generic entity into a camera within the scene. Unlike many components that define an entity's physical presence or interactive behavior, the `camera-component` defines how the world is viewed. When attached to an entity, this entity's transform (position, rotation, scale) will dictate the camera's viewpoint and orientation in 3D space. Each entity can host only one `camera-component` at a time, defining its singular role as a camera.

This component encapsulates all the necessary parameters for defining a camera's projection, which determines how 3D objects are projected onto the 2D viewing plane. There are two primary types of projections supported: Perspective and Orthographic.

```
{
    "type": "camera-component",
    "camera-component":
    {
        "type": "<projection_type>",
        "<projection_type>":
        {
            // ... projection-specific parameters
        }
    }
}
```

-   **`type` (String):**

    -   **Parent Level:** Always `"camera-component"`. This top-level `type` field identifies the component itself.
    -   **Nested Level:** Specifies the type of projection to be used. Valid values are `"perspective"` or `"orthographic"`.

-   **`camera-component` (Object):**

    -   This nested object holds the actual camera configuration. It contains a `type` field to declare the projection method and then a corresponding object (`perspective` or `orthographic`) with the specific parameters for that projection.

___
#### Projection Type: `perspective`

**Description:** A `perspective` projection simulates how the human eye perceives depth. Objects further away from the camera appear smaller, creating a sense of realism and depth. This is the most common projection type for 3D games and simulations.

**Parameters:**
```
{
    "type": "camera-component",
    "camera-component": {
        "type": "perspective",
        "perspective": {
            "aspect_ratio": 1.3,
            "fov": 45,
            "znear": 0.1,
            "zfar": 10000.0
        }
    }
}
```

-   **`aspect_ratio` (Number):**

    -   **Purpose:** Defines the ratio of the camera's view frustum width to its height. This is crucial for preventing stretching or squishing of the rendered scene, especially when the display window's dimensions change.

    -   **Calculation:** Calculated as `width / height` (e.g., a 16:9 widescreen display would have an aspect ratio of approximately 1.777, while a 4:3 display would be 1.333).

    -   **Usage Notes:** It is highly recommended to set this value to match the aspect ratio of the rendering surface (e.g., the game window or viewport) to ensure correct visual proportions. If not provided or incorrect, the rendered scene might appear distorted.

-   **`fov` (Number):**

    -   **Purpose:** Stands for "Field of View." This value defines the vertical angle (in degrees) of the camera's view frustum. A larger FOV captures more of the scene, but can also introduce a "fish-eye" effect. A smaller FOV narrows the view, akin to zooming in.

    -   **Units:** Expressed in **degrees**.

    -   **Usage Notes:** Typical values range from 60 to 90 degrees for first-person games. Cinematic cameras might use smaller FOVs, while action-oriented games might opt for larger values to provide a wider situational awareness.

-   **`znear` (Number):**

    -   **Purpose:** Specifies the distance from the camera to the near clipping plane. Any geometry closer than this plane will be clipped (not rendered).

    -   **Units:** Scene units (e.g., meters).

    -   **Usage Notes:** Must be a positive value. Setting `znear` too small (e.g., close to 0) can lead to precision issues with depth buffering, causing "z-fighting" artifacts where polygons at similar depths flicker. It also means more geometry is processed, even if it's very close and might not be visible.

-   **`zfar` (Number):**

    -   **Purpose:** Specifies the distance from the camera to the far clipping plane. Any geometry further away than this plane will be clipped (not rendered).

    -   **Units:** Scene units (e.g., meters).

    -   **Usage Notes:** Must be a positive value and greater than `znear`. A very large `zfar` value can exacerbate depth precision issues, especially in conjunction with a small `znear`. It's generally best to set `zfar` to the minimum necessary distance to encompass the desired viewable range of the scene.

___
#### Projection Type: `orthographic`

**Description:** An `orthographic` projection renders a scene without perspective distortion. Parallel lines remain parallel, and objects retain their true size regardless of their distance from the camera. This is commonly used for 2D games, architectural blueprints, technical diagrams, or top-down/isometric views where accurate scale and absence of vanishing points are desired.

**Parameters:**
```
{
    "type": "camera-component",
    "camera-component": {
        "type": "orthographic",
        "orthographic": {
            "left": -200,
            "right": 200,
            "bottom": -200,
            "top": 200,
            "znear": 0.1,
            "zfar": 10000.0
        }
    }
}
```
- **`left` (Number):**

    - **Purpose:** The x-coordinate of the left boundary of the orthographic view frustum.

    - **Units:** Scene units.

- **`right` (Number):**

    - **Purpose:** The x-coordinate of the right boundary of the orthographic view frustum.

    - **Units:** Scene units.

    - **Usage Notes:** Typically `right` > `left`.

- **`bottom` (Number):**

    - **Purpose:** The y-coordinate of the bottom boundary of the orthographic view frustum.

    - **Units:** Scene units.

- **`top` (Number):**

    - **Purpose:** The y-coordinate of the top boundary of the orthographic view frustum.

    - **Units:** Scene units.

    - **Usage Notes:** Typically `top` > `bottom`. The values `left`, `right`, `bottom`, and `top` together define the size and aspect ratio of the 2D viewing plane at the camera's position.

- **`znear` (Number):**

    - **Purpose:** Specifies the distance from the camera to the near clipping plane. Any geometry closer than this plane will be clipped.

    - **Units:** Scene units.

    - **Usage Notes:** Can be positive or negative. Similar to perspective projection, values very close to zero or a vast range between `znear` and `zfar` can impact depth buffer precision.

- **`zfar` (Number):**

    - **Purpose:** Specifies the distance from the camera to the far clipping plane. Any geometry further away than this plane will be clipped.

    - **Units:** Scene units.

    - **Usage Notes:** Can be positive or negative, but must ensure that the range (`zfar - znear`) is positive. Similar to perspective projection, a very large difference between `znear` and `zfar` can lead to depth precision issues.

___
#### Usage Example in a Scene:

To use a `camera-component`, you would attach it to an entity within your scene definition. This entity would also likely have a `transform-component` (or equivalent) to define its position and orientation in the world.
```
{
    "entities":
    [
        {
            "name": "main_camera_entity",
            "components":
            [
                {
                    "type": "transform-component",
                    "transform-component":
                    {
                        "position": [0, 5, 10],   // Position the camera
                        "rotation": [0, 180, 0], // Point the camera towards the origin
                        "scale": [1, 1, 1]
                    }
                },
                {
                    "type": "camera-component",
                    "camera-component":
                    {
                        "type": "perspective",
                        "perspective":
                        {
                            "aspect_ratio": 1.777, // e.g., for 16:9 display
                            "fov": 75,
                            "znear": 0.5,
                            "zfar": 5000.0
                        }
                    }
                }
            ]
        },
        {
            "name": "ortho_ui_camera",
            "components":
            [
                {
                    "type": "transform-component",
                    "transform-component":
                    {
                        "position": [0, 0, 100], // Position far back for UI
                        "rotation": [0, 0, 0],
                        "scale": [1, 1, 1]
                    }
                },
                {
                    "type": "camera-component",
                    "camera-component":
                    {
                        "type": "orthographic",
                        "orthographic": {
                            "left": -640,  // Example for a 1280x720 UI resolution
                            "right": 640,
                            "bottom": -360,
                            "top": 360,
                            "znear": 0.1,
                            "zfar": 200
                        }
                    }
                }
            ]
        }
    ]
}
```
___
### Component: `rigidbody-component`

**Description:** The `rigidbody-component` is an attachable entity component that defines the physical properties and behavior of an entity within the game's physics simulation. It dictates whether an entity is influenced by forces like gravity, can move or rotate, interacts physically with other objects (colliding and generating forces), or acts purely as a trigger for events. An entity equipped with a `rigidbody-component` becomes a "rigid body" in the physics world.

The state defined within this component (e.g., position, orientation, velocity) represents the _initial_ state of the rigid body at scene load. During runtime, the physics engine will simulate and update these properties. When an entity possesses both a `rigidbody-component` and a `transform-component`, the `rigidbody-component` becomes the authoritative source for the entity's position and orientation in the physics simulation. The `transform-component` is typically updated by the physics engine to reflect the rigid body's current state.

**Structure:**
```
{
    "type": "rigidbody-component",
    "rigidbody-component":
    {
        "type": "<body_type>",
        "<body_type>":
        {
            // ... body-type specific and common parameters
        }
    }
}
```
- **`type` (String):**

    - **Parent Level:** Always `"rigidbody-component"`. This top-level `type` field identifies the component itself.

    - **Nested Level (`rigidbody-component.type`):** Specifies the fundamental behavior type of the rigid body. Valid values are `"dynamic"`, `"static"`, and `"kinematic"`.

- **`rigidbody-component` (Object):**

    - This nested object holds the specific configuration for the rigid body. It contains a `type` field to declare the body behavior and then a corresponding object (e.g., `rigidbody`) with the specific parameters for that body type.

#### Rigid Body Type: `rigidbody` (Dynamic Body)

**Description:** A `dynamic` rigid body is fully simulated by the physics engine. It is affected by forces (like gravity), collisions, and impulses, and its position, orientation, and velocity are automatically updated by the simulation. This is suitable for interactive objects, characters, projectiles, and anything that should respond realistically to physical interactions.

**Parameters:**
```
{
    "type": "rigidbody-component",
    "rigidbody-component": {
        "type": "rigidbody", // Implies "dynamic" behavior
        "rigidbody": {
            "body": "dynamic", // Explicitly states the body type
            "linear": {
                "velocity": [0.0, 0.0, 0.0],
                "mass": 10.0,
                "damping": 0.01
            },
            "angular": {
                "velocity": [0.0, 0.0, 0.0],
                "inertia": [2.1, 2.1, 2.1],
                "damping": 0.05
            },
            "position": [0.0, 2.0, 0.0],
            "orientation": [0.0, 0.0, 0.0, 1.0],
            "sleeping": false,
            "gravityScale": 1.0,
            "collider": {
                // ... (details to be covered separately)
            }
        }
    }
}
```
Understood! Thanks for the detailed clarifications. This information is crucial for writing accurate and helpful documentation.

Let's proceed. I will now start documenting the `rigidbody-component`, focusing on the overall structure and the `dynamic`, `static`, and `kinematic` body types, as well as the shared properties like linear/angular characteristics, initial state, `sleeping`, and `gravityScale`.

Then, as you requested, we will go through each `collider.shape` type one by one in subsequent turns.

---

### Component: `rigidbody-component`

**Description:** The `rigidbody-component` is an attachable entity component that defines the physical properties and behavior of an entity within the game's physics simulation. It dictates whether an entity is influenced by forces like gravity, can move or rotate, interacts physically with other objects (colliding and generating forces), or acts purely as a trigger for events. An entity equipped with a `rigidbody-component` becomes a "rigid body" in the physics world.

The state defined within this component (e.g., position, orientation, velocity) represents the _initial_ state of the rigid body at scene load. During runtime, the physics engine will simulate and update these properties. When an entity possesses both a `rigidbody-component` and a `transform-component`, the `rigidbody-component` becomes the authoritative source for the entity's position and orientation in the physics simulation. The `transform-component` is typically updated by the physics engine to reflect the rigid body's current state.

**Structure:**

JSON

```
{
    "type": "rigidbody-component",
    "rigidbody-component": {
        "type": "<body_type>",
        "<body_type>": {
            // ... body-type specific and common parameters
        }
    }
}
```

- **`type` (String):**

    - **Parent Level:** Always `"rigidbody-component"`. This top-level `type` field identifies the component itself.

    - **Nested Level (`rigidbody-component.type`):** Specifies the fundamental behavior type of the rigid body. Valid values are `"dynamic"`, `"static"`, and `"kinematic"`.

- **`rigidbody-component` (Object):**

    - This nested object holds the specific configuration for the rigid body. It contains a `type` field to declare the body behavior and then a corresponding object (e.g., `rigidbody`) with the specific parameters for that body type.


```
{
    "type": "rigidbody-component",
    "rigidbody-component": {
        "type": "rigidbody", // Implies "dynamic" behavior
        "rigidbody": {
            "body": "dynamic", // Explicitly states the body type
            "linear": {
                "velocity": [0.0, 0.0, 0.0],
                "mass": 10.0,
                "damping": 0.01
            },
            "angular": {
                "velocity": [0.0, 0.0, 0.0],
                "inertia": [2.1, 2.1, 2.1],
                "damping": 0.05
            },
            "position": [0.0, 2.0, 0.0],
            "orientation": [0.0, 0.0, 0.0, 1.0],
            "sleeping": false,
            "gravityScale": 1.0,
            "collider": {
                // ... (details to be covered separately)
            }
        }
    }
}
```

- **`body` (String):**

    - **Purpose:** Explicitly declares this rigid body as `dynamic`. While `rigidbody-component.type` also indicates this, `body` serves as an internal property for the physics system.

    - **Value:** `"dynamic"`

- **`linear` (Object):**

    - **Purpose:** Defines properties related to the rigid body's translational motion.

    - **`velocity` (Array of Numbers `[x, y, z]`):**

        - **Purpose:** The initial linear velocity of the rigid body in meters per second (m/s).

        - **Units:** Scene units per second.

        - **Usage Notes:** The physics engine will update this value during simulation.

    - **`mass` (Number):**

        - **Purpose:** The total mass of the rigid body. Mass directly influences how the body responds to forces and impacts.

        - **Units:** User-defined (e.g., kilograms). Consistency in unit definition across all masses is crucial for realistic simulation.

        - **Usage Notes:** Can be positive or negative, depending on the desired physics behavior (though typically positive for real-world simulation). A mass of `0` or negative mass might lead to undefined or unusual physics behavior, depending on the engine's implementation.

    - **`damping` (Number):**

        - **Purpose:** A linear damping factor applied to the rigid body's linear velocity. It simulates air resistance or friction, causing the body to slow down over time.

        - **Range:** Typically `0.0` (no damping) to `1.0` (heavy damping, potentially halting motion quickly). Values greater than `1.0` might cause exaggerated damping.

        - **Usage Notes:** Applied per physics step, reducing the linear velocity by a percentage.

- **`angular` (Object):**

    - **Purpose:** Defines properties related to the rigid body's rotational motion.

    - **`velocity` (Array of Numbers `[x, y, z]`):**

        - **Purpose:** The initial angular velocity of the rigid body, representing rotation around its local X, Y, and Z axes.

        - **Units:** Radians per second.

        - **Usage Notes:** The physics engine will update this value during simulation.

    - **`inertia` (Array of Numbers `[x, y, z]`):**

        - **Purpose:** Represents the principal moments of inertia along the rigid body's local X, Y, and Z axes. This is effectively the diagonal trace of the inertia tensor. It describes the body's resistance to rotational changes around these axes.

        - **Units:** Mass * (Length)^2 (e.g., kg * m^2).

        - **Usage Notes:** For simple shapes like boxes or spheres, this can often be automatically calculated by the physics engine if not provided, based on `mass` and `collider` dimensions. For complex shapes or specific behaviors, manually setting it provides fine-grained control.

    - **`damping` (Number):**

        - **Purpose:** An angular damping factor applied to the rigid body's angular velocity. It simulates rotational friction, causing the body to stop spinning over time.

        - **Range:** Typically `0.0` (no damping) to `1.0` (heavy damping).

        - **Usage Notes:** Applied per physics step, reducing the angular velocity by a percentage.

- **`position` (Array of Numbers `[x, y, z]`):**

    - **Purpose:** The initial position of the rigid body's center of mass in world space when the scene is loaded.

    - **Units:** Scene units.

    - **Usage Notes:** This value will be updated by the physics engine during simulation. If an entity has a `transform-component`, its position will be synchronized with this `rigidbody.position`.

- **`orientation` (Array of Numbers `[x, y, z, w]`):**

    - **Purpose:** The initial orientation of the rigid body in world space, represented as a unit quaternion.

    - **Usage Notes:** This value will be updated by the physics engine during simulation. If an entity has a `transform-component`, its rotation will be synchronized with this `rigidbody.orientation`. It is crucial that this is a normalized (unit) quaternion to avoid issues with rotation and scaling during physics simulation.

- **`sleeping` (Boolean):**

    - **Purpose:** If `true`, the rigid body starts in a "sleeping" state, meaning it is initially inactive in the physics simulation. This is an optimization to reduce CPU overhead for objects that are at rest.

    - **Usage Notes:** A sleeping body will be automatically "woken up" by the physics engine if it experiences a significant force, impulse, or collision with an active body.

- **`gravityScale` (Number):**

    - **Purpose:** A multiplier applied to the global gravity vector for this specific rigid body.

    - **Usage Notes:** A value of `1.0` means the body is fully affected by global gravity. A value of `0.0` means it is unaffected by gravity. Values less than `0.0` will make it "float" upwards (anti-gravity), and values greater than `1.0` will make it fall faster.

- **`collider` (Object):**

    - **Purpose:** Defines the collision shape and material properties associated with this rigid body. (Details for this object and its `shape` sub-object will be covered in subsequent sections as per your request).a

#### Rigid Body Type: `static`

**Description:** A `static` rigid body is an immovable object that participates in the physics simulation by providing collision geometry, but it is not affected by forces, gravity, or collisions. It has infinite mass and inertia. This is ideal for scene geometry like terrain, buildings, walls, or any fixed environment element that dynamic objects should collide with.

- **`body` (String):**

    - **Purpose:** Declares this rigid body as `static`.

    - **Value:** `"static"`

- **`linear`, `angular`, `sleeping`, `gravityScale`:** These properties are **ignored** for static bodies as they do not move or respond to physics forces.

- **`position` (Array of Numbers `[x, y, z]`):** The initial world space position. Static bodies remain at this position.

- **`orientation` (Array of Numbers `[x, y, z, w]`):** The initial world space orientation. Static bodies maintain this orientation.

- **`collider` (Object):** Defines the collision shape. Static bodies _must_ have a collider to interact with dynamic or kinematic bodies.

#### Rigid Body Type: `kinematic`

**Description:** A `kinematic` rigid body is one that is moved and controlled by animation or script, rather than being driven by physics forces. However, it _does_ interact with dynamic rigid bodies, pushing them and generating collision events. It essentially acts as if it has infinite mass and is being directly controlled. This is useful for moving platforms, doors, or animated characters that need to influence the physics world without being physically simulated themselves.

- **`body` (String):**

    - **Purpose:** Declares this rigid body as `kinematic`.

    - **Value:** `"kinematic"`

- **`linear.velocity` (Array of Numbers `[x, y, z]`):**

    - **Usage Notes:** While the kinematic body's position is set externally (e.g., by script), the physics engine often derives an internal velocity from changes in its position between physics steps. This derived velocity is used to influence dynamic bodies it collides with. Setting an initial value here might represent a starting scripted motion.

- **`angular.velocity` (Array of Numbers `[x, y, z]`):**

    - **Usage Notes:** Similar to linear velocity, the engine derives an internal angular velocity.

- **`linear.mass`, `linear.damping`, `angular.inertia`, `angular.damping`:** These properties are **ignored** for kinematic bodies, as they are not driven by forces or simulation.

- **`position` (Array of Numbers `[x, y, z]`):** The initial world space position. This will be overwritten by external control during runtime.

- **`orientation` (Array of Numbers `[x, y, z, w]`):** The initial world space orientation. This will be overwritten by external control during runtime.

- **`sleeping` (Boolean):** Can be `true` (if the kinematic body is initially stationary and not moved) or `false` (if it's intended to move immediately).

- **`gravityScale` (Number):** Typically `0.0` as kinematic bodies are not influenced by gravity. If set to a non-zero value, it will still be ignored.

- **`collider` (Object):** Defines the collision shape. Kinematic bodies _must_ have a collider to interact with dynamic bodies.

___
### Object: `collider` (Within `rigidbody-component`)

**Description:** The `collider` object, embedded within a `rigidbody-component`, defines the geometric shape and physical material properties that enable an entity to detect collisions and interact with other objects in the physics simulation. Every rigid body intended to physically interact or detect events _must_ have a `collider`. The collider's position and orientation are defined relative to the rigid body's center of mass.

**Structure:**
```
{
    "collider":
    {
        "shape":
        {
            "type": "<collider_shape_type>",
            "<collider_shape_type>":
            {
                // ... shape-specific parameters
            }
        },

        "friction": 0.6,
        "restitution": 0.2,
        "is_trigger": false
    }
}
```
- **`shape` (Object):**

    - **Purpose:** Defines the specific geometric form of the collider. Different shapes are optimized for various collision detection scenarios.

    - **`type` (String):** Specifies the type of collider shape. Examples include `"collider-box"`, `"collider-sphere"`, `"collider-capsule"`, etc.

    - **`<collider_shape_type>` (Object):** A nested object whose structure and parameters depend on the `type` specified (e.g., `collider-box` for a box shape). Details for each specific shape type will be provided separately.

- **`offset` (Object):**

    - **Purpose:** Defines a local transformation (position and orientation) of the collider relative to the rigid body's center of mass. This allows for placing and rotating the collision shape independently of the rigid body's origin, which is useful for complex models or when the visual mesh origin doesn't align with the desired center of mass.

    - **`position` (Array of Numbers `[x, y, z]`):**

        - **Purpose:** The translational offset of the collider's origin from the rigid body's center of mass.

        - **Units:** Scene units.

    - **`orientation` (Array of Numbers `[x, y, z, w]`):**

        - **Purpose:** The rotational offset of the collider relative to the rigid body's local coordinate system, represented as a unit quaternion.

        - **Usage Notes:** This must be a normalized (unit) quaternion.

- **`friction` (Number):**

    - **Purpose:** Defines the combined static and dynamic friction coefficient for this collider. Friction determines the resistance to sliding motion between colliding surfaces.

    - **Range:** `0.0` (no friction) to higher values. Typical values are often between `0.0` and `1.0`, but higher values can represent very sticky surfaces.

    - **Usage Notes:** The physics engine will use this value in conjunction with the friction of the other colliding body to determine the overall friction applied during contact.

- **`restitution` (Number):**

    - **Purpose:** Defines the coefficient of restitution, often referred to as "bounciness." It determines how much kinetic energy is conserved during a collision.

    - **Range:** `0.0` (perfectly inelastic, no bounce) to `1.0` (perfectly elastic, full bounce).

    - **Usage Notes:** A value of `0.0` will result in objects sticking together or losing all kinetic energy upon impact (unless other forces act). A value of `1.0` means objects will rebound with the same relative speed as they had before impact.

- **`isTrigger` (Boolean):**

    - **Purpose:** If `true`, this collider acts as a "trigger" volume. Triggers detect overlaps with other colliders and generate event notifications (e.g., "onTriggerEnter", "onTriggerExit"), but they do _not_ apply physics forces or cause physical collisions that prevent movement.

    - **Usage Notes:** Ideal for detection zones, checkpoints, or areas that should cause a script action without physically impeding other objects. When `false`, the collider acts as a solid physical boundary.

___
### Collider Shape: `collider-box` (Oriented Bounding Box - OBB)

**Description:** The `collider-box` defines a cuboid collision shape, often referred to as an Oriented Bounding Box (OBB). Unlike an Axis-Aligned Bounding Box (AABB), an OBB can be arbitrarily rotated in local space, making it highly flexible for representing rectangular objects that are not aligned with the world or rigid body axes.

**Parameters:**
```
{
    "type": "collider-box",
    "collider-box": {
        "center": [0,0,0],
        "extents": [1,1,1],
        "axes":
        [
            [1,0,0],
            [0,1,0],
            [0,0,1]
        ]
    }
}
```
- **`type` (String):** Always `"collider-box"`.

- **`collider-box` (Object):** Contains the specific properties for this box shape.

    - **`center` (Array of Numbers `[x, y, z]`):**

        - **Purpose:** The local center point of the box, relative to the `collider.offset.position`.

        - **Units:** Scene units.

        - **Usage Notes:** Often `[0,0,0]` if the box is centered around the collider's offset origin.

    - **`extents` (Array of Numbers `[x, y, z]`):**

        - **Purpose:** The half-lengths of the box along its local X, Y, and Z axes, respectively. The full length of the box along an axis will be `2 * extent_value`.

        - **Units:** Scene units.

        - **Usage Notes:** All values must be positive. For example, `[1,1,1]` defines a cube with a side length of 2 units.

    - **`axes` (Array of Arrays of Numbers `[[x1,y1,z1], [x2,y2,z2], [x3,y3,z3]]`):**

        - **Purpose:** Defines the local orthonormal basis (orientation) of the box. These three vectors represent the local X, Y, and Z axes of the box.

        - **Usage Notes:**

            - Each inner array `[x,y,z]` must be a **normalized (unit) vector**.

            - The three vectors _must_ be **orthogonal** to each other (i.e., their dot products are zero).

            - Together, these vectors form a 3x3 rotation matrix for the box in its local space. The physics engine uses this to correctly orient the box collider.

            - A common default, representing an axis-aligned box relative to the `collider.offset.orientation`, would be `[[1,0,0], [0,1,0], [0,0,1]]`.

___
### Collider Shape: `collider-sphere`

**Description:** The `collider-sphere` defines a spherical collision shape. Spheres are computationally efficient for collision detection and are ideal for representing spherical objects or for use as simplified bounding volumes for more complex, irregularly shaped entities (e.g., characters, rolling objects). Due to its perfect radial symmetry, a sphere collider's orientation (defined by the `collider.offset.orientation` or the rigid body's `orientation`) does not affect its collision geometry, though it still inherits the position offset.

**Parameters:**
```
{
    "type": "collider-sphere",
    "collider-sphere": {
        "center": [0,0,0],
        "radius": 1.0
    }
}
```
- **`type` (String):** Always `"collider-sphere"`.

- **`collider-sphere` (Object):** Contains the specific properties for this sphere shape.

    - **`center` (Array of Numbers `[x, y, z]`):**

        - **Purpose:** The local center point of the sphere, relative to the `collider.offset.position`.

        - **Units:** Scene units.

        - **Usage Notes:** Often `[0,0,0]` if the sphere is centered around the collider's offset origin.

    - **`radius` (Number):**

        - **Purpose:** The radius of the sphere.

        - **Units:** Scene units.

        - **Usage Notes:** Must be a positive value. A larger radius will result in a larger collision volume.

___
### Collider Shape: `collider-cylinder`

**Description:** The `collider-cylinder` defines a cylindrical collision shape. Cylinders are useful for representing objects such as pillars, barrels, or simplified character bodies, offering a balance between performance and accuracy for certain geometries. The cylinder's orientation is determined by its `axes` property, allowing it to be aligned arbitrarily in local space.

**Parameters:**
```
{
    "type": "collider-cylinder",
    "collider-cylinder":
    {
        "center": [0,0,0],
        "axes": [ [1,0,0], [0,1,0] ], // Example, order matters
        "height": 2.0,
        "radius": 1.0
    }
}
```
- **`type` (String):** Always `"collider-cylinder"`.

- **`collider-cylinder` (Object):** Contains the specific properties for this cylinder shape.

    - **`center` (Array of Numbers `[x, y, z]`):**

        - **Purpose:** The local center point of the cylinder, relative to the `collider.offset.position`. This point is typically the midpoint along the cylinder's height axis.

        - **Units:** Scene units.

        - **Usage Notes:** Often `[0,0,0]` if the cylinder is centered around the collider's offset origin.

    - **`axes` (Array of Arrays of Numbers `[[x1,y1,z1], [x2,y2,z2]]`):**

        - **Purpose:** Defines the local orientation of the cylinder.

            - The **first vector** defines a radial axis, perpendicular to the cylinder's height axis. It helps establish the cylinder's rotational orientation around its height axis.

            - The **second vector** explicitly defines the direction of the cylinder's **height axis**.

        - **Usage Notes:**

            - Both vectors must be **normalized (unit vectors)**.

            - The two vectors _must_ be **orthogonal** to each other (i.e., their dot product is zero). The physics engine will likely construct the third orthogonal axis internally.

            - For example, `[[1,0,0], [0,1,0]]` would define a cylinder whose height axis is aligned with the local Y-axis and whose primary radial direction is along the local X-axis.

    - **`height` (Number):**

        - **Purpose:** The total length of the cylinder along its height axis.

        - **Units:** Scene units.

        - **Usage Notes:** Must be a positive value.

    - **`radius` (Number):**

        - **Purpose:** The radius of the cylinder.

        - **Units:** Scene units.

        - **Usage Notes:** Must be a positive value.

___
### Collider Shape: `collider-plane`

**Description:** The `collider-plane` defines an infinite flat plane collision shape. Planes are commonly used to represent large, static, and immovable surfaces such as ground, walls, or water surfaces. Because a plane is infinite, it is typically used for static rigid bodies that represent environmental boundaries.

**Important Note on Offsets:** For `collider-plane`, the `offset` properties (`collider.offset.position` and `collider.offset.orientation`) are **ignored**. The `normal` and `position` properties of the `collider-plane` are defined directly in the local coordinate system of the rigid body to which this collider is attached. If the rigid body itself has a `transform-component` (defining its position and orientation), the plane's final world-space definition will be relative to that rigid body's transform.

**Parameters:**
```
{
    "type": "collider-plane",
    "collider-plane":
    {
        "normal": [0.0, 1.0, 0.0], // Example: Y-up plane
        "position": [0.0, 0.0, 0.0]  // Example: Plane passing through the rigid body's origin
    }
}
```
- **`type` (String):** Always `"collider-plane"`.

- **`collider-plane` (Object):** Contains the specific properties for this plane shape.

    - **`normal` (Array of Numbers `[x, y, z]`):**

        - **Purpose:** Defines the direction perpendicular to the plane's surface, indicating which way the plane is "facing." Collision detection typically occurs on the side of the plane pointed to by the normal vector.

        - **Usage Notes:**

            - Must be a **normalized (unit) vector**.

            - This vector is interpreted directly in the local space of the rigid body. For example, `[0, 1, 0]` would define a horizontal plane (like a floor) if the rigid body's Y-axis is up.

    - **`position` (Array of Numbers `[x, y, z]`):**

        - **Purpose:** A single point that lies on the plane. Together with the `normal`, this point uniquely defines the plane's exact location in space.

        - **Units:** Scene units.

        - **Usage Notes:** This point is interpreted in the local space of the rigid body. If the rigid body is at world `[0,0,0]` with no rotation, then `[0,0,0]` for `position` means the plane passes through the world origin.

___
### Collider Shape: `collider-cone`

**Description:** The `collider-cone` defines a conical collision shape. Cones are useful for specific applications such as representing the field of view for certain sensors, or for simplified collision volumes of pointed objects.

**Important Note on Spatial Definition:** Unlike many component properties that are relative to an entity's transform, the `apex` and `direction` parameters of the `collider-cone` are defined directly in **absolute world space**. This means the cone's initial position and orientation are fixed in the world, independent of the parent rigid body's `position` and `orientation` specified in its `rigidbody-component`. The physics engine or game logic is responsible for associating this static world-space collider with a moving rigid body if dynamic interaction is desired. The `collider.offset` properties are not applicable and are ignored for this collider type.

**Parameters:**
```
{
    "type": "collider-cone",
    "collider-cone":
    {
        "apex": [0.0, 2.0, 0.0],    // Example: Tip at world (0,2,0)
        "direction": [0.0, -1.0, 0.0], // Example: Cone points downwards along Y-axis
        "height": 2.0,
        "radius": 1.0
    }
}
```
- **`type` (String):** Always `"collider-cone"`.

- **`collider-cone` (Object):** Contains the specific properties for this cone shape.

    - **`apex` (Array of Numbers `[x, y, z]`):**

        - **Purpose:** Defines the absolute world-space coordinates of the conical tip (the point opposite the circular base).

        - **Units:** World scene units.

    - **`direction` (Array of Numbers `[x, y, z]`):**

        - **Purpose:** Defines the absolute world-space direction vector along the cone's central axis, pointing away from the `apex` towards the center of the base.

        - **Usage Notes:**

            - Must be a **normalized (unit) vector**.

            - This vector establishes the cone's orientation in world space.

    - **`height` (Number):**

        - **Purpose:** The total perpendicular distance from the `apex` to the circular base, measured along the `direction` vector.

        - **Units:** World scene units.

        - **Usage Notes:** Must be a positive value.

    - **`radius` (Number):**

        - **Purpose:** The radius of the cone's circular base.

        - **Units:** World scene units.

        - **Usage Notes:** Must be a positive value.

___
### Collider Shape: `collider-circle`

**Description:** The `collider-circle` defines a circular disk collision shape with a defined thickness, effectively representing a "thick 2D disk" in 3D space. While conceptually a circle is 2D, this collider includes a `height` parameter to provide a finite thickness, ensuring robust collision detection, as no physical object in a simulation can have zero volume. It is useful for flat, circular objects like coins, wheels (if simplified), or circular pressure plates.

**Important Note on Spatial Definition:** Similar to `collider-plane` and `collider-cone`, the `center` and `normal` parameters of the `collider-circle` are defined directly in **absolute world space**. This means the disk's initial position and orientation are fixed in the world, independent of the parent rigid body's `position` and `orientation` specified in its `rigidbody-component`. The physics engine or game logic is responsible for associating this static world-space collider with a moving rigid body if dynamic interaction is desired. The `collider.offset` properties are not applicable and are ignored for this collider type.

**Parameters:**
```
{
    "type": "collider-circle",
    "collider-circle":
    {
        "center": [0.0, 2.0, 0.0],  // Example: Center of the disk at world (0,2,0)
        "normal": [0.0, 1.0, 0.0],  // Example: Disk lies on the XZ plane, facing up along Y
        "height": 0.1,              // Example: A thin disk with 0.1 units thickness
        "radius": 1.0
    }
}
```
- **`type` (String):** Always `"collider-circle"`.

- **`collider-circle` (Object):** Contains the specific properties for this disk shape.

    - **`center` (Array of Numbers `[x, y, z]`):**

        - **Purpose:** The absolute world-space coordinates of the exact center of the circular disk.

        - **Units:** World scene units.

    - **`normal` (Array of Numbers `[x, y, z]`):**

        - **Purpose:** Defines the absolute world-space direction vector perpendicular to the circular disk's surface. This indicates the "up" direction the disk is facing and defines the orientation of its circular plane. The disk's thickness extends along this normal.

        - **Usage Notes:** Must be a **normalized (unit) vector**.

    - **`height` (Number):**

        - **Purpose:** The thickness of the disk, measured along its `normal` vector. The disk extends `height / 2` units in both positive and negative directions along the `normal` from its `center`.

        - **Units:** World scene units.

        - **Usage Notes:** Must be a positive value. This provides the necessary volume for collision detection.

    - **`radius` (Number):**

        - **Purpose:** The radius of the circular disk.

        - **Units:** World scene units.

        - **Usage Notes:** Must be a positive value.

___
### Collider Shape: `collider-mesh`

**Description:** The `collider-mesh` defines a collision shape directly from a custom 3D mesh, allowing for highly detailed and complex collision geometry that precisely matches visual models. It is composed of an array of vertices and an array of indices that define its triangular faces.

**Important Note on Spatial Definition:** Similar to `collider-plane`, `collider-cone`, and `collider-circle`, the `vertices` of the `collider-mesh` are defined directly in **absolute world space**. This means the mesh collider's initial position and orientation are fixed in the world, independent of the parent rigid body's `position` and `orientation` specified in its `rigidbody-component`. The physics engine or game logic is responsible for associating this static world-space collider with a moving rigid body if dynamic interaction is desired. The `collider.offset` properties are not applicable and are ignored for this collider type.

**Flexibility & Performance Considerations:** The `collider-mesh` is highly flexible as it can represent both **convex hull** shapes and complex **concave** geometry. It can be used with `static`, `dynamic`, and `kinematic` rigid bodies. However, users should be aware of the significant performance implications associated with complex mesh colliders:

- **Concave Meshes:** While supported, collisions involving concave mesh colliders (especially with other dynamic or complex colliders) are significantly more computationally intensive than collisions with simpler primitive shapes (sphere, box, capsule) or convex hulls. For dynamic objects, using primitive colliders or a convex hull approximation of the mesh is generally recommended for better performance.

- **Dynamic Mesh Colliders:** Using `collider-mesh` on `dynamic` rigid bodies can be very demanding on the physics engine. It is typically most performant when applied to `static` environmental geometry (e.g., ground, buildings, detailed level geometry).

- **Vertex Count:** There is no hard-coded limit on the number of vertices a `collider-mesh` can have. The appropriate complexity (vertex count) is left to the developer to determine based on their target platform, performance budget, and the specific requirements of their scene. Higher vertex counts will naturally lead to increased computational cost for collision detection.


**Collision Processing:** The physics system will attempt to process collisions between `collider-mesh` and other colliders. If a specific collision function exists within the physics engine to handle the interaction between the given `collider-mesh` and the other collider type, the collision will be processed. If no such function is available for a particular combination, a `false` value (or equivalent indicator of no detected collision) will be returned, and no physical response will occur.

**Parameters:**
```
{
    "type": "collider-mesh",
    "collider-mesh":

        "triangles": [0,1,3, 0,3,2, ...], // Example for two triangles forming a quad
        "vertices": [[0.0,0.0,0.0], [1.0,0.0,0.0], [1.0,1.0,0.0], [0.0,1.0,0.0], ... ]
    }
}
```
- **`type` (String):** Always `"collider-mesh"`.

- **`collider-mesh` (Object):** Contains the specific properties for this mesh shape.

    - **`triangles` (Array of Numbers):**

        - **Purpose:** A flat array of vertex indices defining the triangles that make up the mesh. Each group of three consecutive indices forms a single triangle, referencing vertices from the `vertices` array.

        - **Usage Notes:** For example, `[0,1,2, 0,2,3]` would define two triangles. The winding order (clockwise or counter-clockwise) is important for determining the "outside" surface of the mesh for collision normal calculations.

    - **`vertices` (Array of Arrays of Numbers `[[x,y,z], ...]`):**

        - **Purpose:** An array of 3D points representing the absolute world-space coordinates of the mesh's vertices.

        - **Units:** World scene units.

        - **Usage Notes:** These points define the geometry of the collision mesh.

    - **Watertightness:** All meshes provided are **expected to be watertight** (i.e., fully enclosed, manifold geometry with no holes or gaps). This is crucial for reliable and predictable collision detection and response, especially when calculating internal/external points or complex contact manifolds.

___
### Collider Shape: `collider-capsule`

**Description:** The `collider-capsule` defines a capsule-shaped collision volume. A capsule is essentially a cylinder with hemispherical caps on both ends. This shape is widely used in physics simulations, especially for character controllers, projectiles, or any object where a rounded, elongated form is desirable for robust collision detection and smooth sliding along surfaces.

**Important Note on Spatial Definition:** Similar to `collider-plane`, `collider-cone`, `collider-circle`, and `collider-mesh`, the `center` and `axes` parameters of the `collider-capsule` are defined directly in **absolute world space**. This means the capsule's initial position and orientation are fixed in the world, independent of the parent rigid body's `position` and `orientation` specified in its `rigidbody-component`. The physics engine or game logic is responsible for associating this static world-space collider with a moving rigid body if dynamic interaction is desired. The `collider.offset` properties are not applicable and are ignored for this collider type.

**Parameters:**
```
{
    "type": "collider-capsule",
    "collider-capsule":
    {
        "center": [0.0, 1.0, 0.0],    // Example: Center of the capsule at world (0,1,0)
        "axes": [[1.0,0.0,0.0], [0.0,1.0,0.0]], // Example: Main axis along Y, radial along X
        "height": 2.0,                 // Length of the cylindrical part
        "radius": 0.5                  // Radius of the capsule
    }
}
```
- **`type` (String):** Always `"collider-capsule"`.

- **`collider-capsule` (Object):** Contains the specific properties for this capsule shape.

    - **`center` (Array of Numbers `[x, y, z]`):**

        - **Purpose:** The absolute world-space coordinates of the midpoint of the capsule's central axis (the midpoint of the cylindrical portion).

        - **Units:** World scene units.

    - **`axes` (Array of Arrays of Numbers `[[x1,y1,z1], [x2,y2,z2]]`):**

        - **Purpose:** Defines the absolute world-space orientation of the capsule.

            - The **first vector** defines a radial axis, perpendicular to the capsule's main axis. It helps establish the capsule's rotational orientation around its main axis.

            - The **second vector** explicitly defines the direction of the capsule's **main axis** (the axis along which its `height` is measured).

        - **Usage Notes:**

            - Both vectors must be **normalized (unit vectors)**.

            - The two vectors _must_ be **orthogonal** to each other (i.e., their dot product is zero). The physics engine will likely construct the third orthogonal axis internally.

            - For example, `[[1,0,0], [0,1,0]]` would define a capsule whose main axis is aligned with the world Y-axis and whose primary radial direction is along the world X-axis.

    - **`height` (Number):**

        - **Purpose:** The length of the cylindrical portion of the capsule _between_ its two hemispherical caps. The total length of the capsule (including caps) will be `height + (2 * radius)`.

        - **Units:** World scene units.

        - **Usage Notes:** Must be a positive value.

    - **`radius` (Number):**

        - **Purpose:** The radius of the cylindrical part of the capsule and also the radius of its hemispherical caps.

        - **Units:** World scene units.

        - **Usage Notes:** Must be a positive value.

___
### Collider Shape: `collider-aabb`

**Description:** The `collider-aabb` defines an Axis-Aligned Bounding Box (AABB) collision shape. An AABB is a rectangular prism that is always aligned with the global X, Y, and Z axes. This makes it one of the most computationally efficient collision shapes for broad-phase collision detection and simple interactions. It is defined by its minimum and maximum corner points in world space.

**Important Note on Spatial Definition:** Consistent with other recent collider types, the `min` and `max` parameters of the `collider-aabb` are defined directly in **absolute world space**. This means the AABB's initial position and dimensions are fixed in the world, independent of the parent rigid body's `position` and `orientation` specified in its `rigidbody-component`. The physics engine or game logic is responsible for associating this static world-space collider with a moving rigid body if dynamic interaction is desired. The `collider.offset` properties are not applicable and are ignored for this collider type.

**Parameters:**
```
{
    "type": "collider-aabb",
    "collider-aabb":
    {
        "min": [0.0, 0.0, 0.0],  // Example: Bottom-left-back corner at world origin
        "max": [1.0, 1.0, 1.0]   // Example: Top-right-front corner at world (1,1,1)
    }
}
```
- **`type` (String):** Always `"collider-aabb"`.

- **`collider-aabb` (Object):** Contains the specific properties for this AABB shape.

    - **`min` (Array of Numbers `[x, y, z]`):**

        - **Purpose:** The absolute world-space coordinates of the minimum corner of the AABB. This point defines the lowest X, Y, and Z extents of the box.

        - **Units:** World scene units.

        - **Usage Notes:** It is implicitly expected that `min.x <= max.x`, `min.y <= max.y`, and `min.z <= max.z`.

    - **`max` (Array of Numbers `[x, y, z]`):**

        - **Purpose:** The absolute world-space coordinates of the maximum corner of the AABB. This point defines the highest X, Y, and Z extents of the box.

        - **Units:** World scene units.

        - **Usage Notes:** It is implicitly expected that `max.x >= min.x`, `max.y >= min.y`, and `max.z >= min.z`.

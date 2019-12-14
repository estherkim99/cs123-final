#version 330 core
in vec2 texc;
out vec4 fragColor;

uniform sampler2D tex;
uniform int useTexture = 0;
uniform sampler2D bumpMap;
uniform int useBumpMapping = 0;
uniform vec2 repeatUV;

in vec4 position_cameraSpace;
in vec4 normal_cameraSpace;
in vec3 normal_ws;
in vec3 tangent_ws;
in vec3 pos_ws;

// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

// Light data
const int MAX_LIGHTS = 10;
uniform int lightTypes[MAX_LIGHTS];         // 0 for point, 1 for directional
uniform vec3 lightPositions[MAX_LIGHTS];    // For point lights
uniform vec3 lightDirections[MAX_LIGHTS];   // For directional lights
//uniform vec3 lightAttenuations[MAX_LIGHTS]; // Constant, linear, and quadratic term
uniform vec3 lightColors[MAX_LIGHTS];

// Material data
uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 specular_color;
uniform float shininess;

uniform bool useLighting;     // Whether to calculate lighting using lighting equation

vec3 calculateBumpNormal() {
    vec3 normal = normalize(normal_ws);
    vec3 tangent = normalize(tangent_ws);
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 bitangent = cross(tangent, normal);
    mat3 TBN = mat3(tangent, bitangent, normal);
    vec3 bump_ts = texture(bumpMap, texc).xyz;
    bump_ts = 2.0 * bump_ts - vec3(1.0, 1.0, 1.0);
    vec3 bump_ws = TBN * bump_ts;
    bump_ws = normalize(bump_ws);
    return bump_ws;
}

void main(){
    vec3 normal = normal_ws;
//     bump mapping
    if (useBumpMapping == 1) {
        normal = calculateBumpNormal();
    }

    // texture mapping
    vec3 texColor = texture(tex, texc).rgb;
    texColor = clamp(texColor + vec3(1-useTexture), vec3(0), vec3(1));

    vec3 color;
    if (useLighting) {
        color = ambient_color.xyz;

        for (int i = 0; i < MAX_LIGHTS; i++) {
            vec4 vertexToLight = vec4(0);
            // Point Light
            if (lightTypes[i] == 0) {
                vertexToLight = normalize(v * vec4(lightPositions[i], 1) - position_cameraSpace);
            } else if (lightTypes[i] == 1) {
                // Dir Light
                vertexToLight = normalize(v * vec4(-lightDirections[i], 0));
            }

            // diffuse component
            float diffuseIntensity = max(0.0, dot(vertexToLight, v * vec4(normal, 0)));
            color += max(vec3(0), lightColors[i] * diffuse_color * diffuseIntensity);

            // specular component
            vec4 lightReflection = normalize(-reflect(vertexToLight, normal_cameraSpace));
            vec4 eyeDirection = normalize(vec4(0,0,0,1) - position_cameraSpace);
            float specIntensity = pow(max(0.0, dot(eyeDirection, lightReflection)), shininess);
            color += max (vec3(0), lightColors[i] * specular_color * specIntensity);
        }
    } else {
        color = ambient_color + diffuse_color;
    }
    color = clamp(color, 0.0, 1.0);

    fragColor = vec4(color * texColor, 1.f);
}



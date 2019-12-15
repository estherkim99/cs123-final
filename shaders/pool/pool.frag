#version 330 core
in vec2 texc;
out vec4 fragColor;

uniform sampler2D tex;
uniform int useTexture = 0;
uniform sampler2D bumpMap;
uniform int useBumpMapping = 0;
uniform vec2 repeatUV;

in vec3 position_world;
in vec4 position_cameraSpace;
in vec4 normal_cameraSpace;
in vec3 anormal;
in vec3 atangent;
uniform float blend;

// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;
uniform vec3 ball_positions[16];

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

mat3 TBN;

vec3 calculateBumpNormal() {
    vec3 normal = mat3(v * m) * normalize(anormal);
    vec3 tangent = mat3(v * m) * normalize(atangent);
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 bitangent = cross(normal, tangent);
    TBN = transpose(mat3(tangent, bitangent, normal));
    vec3 bump_ts = texture(bumpMap, texc).xyz;
    bump_ts = 2.0 * bump_ts - vec3(1.0, 1.0, 1.0);
    return normalize(bump_ts);
}

void main(){
    vec3 normal = vec3(0);
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
//            float diffuseIntensity = max(0.0, dot(normal,  vec3(vertexToLight)));

            float diffuseIntensity = max(0.0, max(dot(vertexToLight, normal_cameraSpace), dot(normal, vec3(vertexToLight))));
            color += max(vec3(0), lightColors[i] * diffuse_color * diffuseIntensity);

            // specular component
            vec4 lightReflection = normalize(-reflect(vertexToLight, normal_cameraSpace));
            vec4 eyeDirection = normalize(vec4(0,0,0,1) - position_cameraSpace);
            float specIntensity = pow(max(0.0, dot(eyeDirection, lightReflection)), shininess);
            color += max (vec3(0), lightColors[i] * specular_color * specIntensity);

            // shadow
            if(abs(position_world.y) < 0.001f){
                vec2 pt = vec2(position_world.x, position_world.z);
                for(int i = 0; i <= 15; i++){
                    vec2 ball = vec2(ball_positions[i].x,ball_positions[i].z);
                    float x = distance(ball, pt);
                    if(x < 0.028575f){
                        color = color * (x*35);
                        break;
                    }
                }
            }
        }
    } else {
        color = ambient_color + diffuse_color;
    }
    color = clamp(color, 0.0, 1.0);

    if (useTexture == 1) {
        fragColor = vec4(color.xyz * (1 - blend) + texColor.xyz * blend, 1.f);
    } else {

        fragColor = vec4(color, 1.f);
    }
}



#version 330 core

in vec3 color;
in vec2 texc;
out vec4 fragColor;

uniform sampler2D tex;
uniform int useTexture = 0;
uniform sampler2D bumpMap;
uniform int useBumpMapping = 0;
uniform vec2 repeatUV;

in vec3 light_ts;
in vec3 eye_ts;
in mat3 TBN;

const int MAX_LIGHTS = 10;
uniform vec3 lightColors[MAX_LIGHTS];

uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 specular_color;
uniform float shininess;

void main(){
    vec3 texColor = texture(tex, texc*repeatUV).rgb;
    texColor = clamp(texColor + vec3(1-useTexture), vec3(0), vec3(1));

        vec3 normal_ts = texture(bumpMap, texc*repeatUV).rgb;
        normal_ts = normalize(normal_ts * 2.0 - 1.0);
//        normal_ts = normalize(normal_ts);
        float kd = clamp(dot(normal_ts, light_ts), 0.0, 1.0);
//        vec3 lightReflection = normalize(-reflect(light_ts, normal_ts));
//        float ks = pow(max(0.0, dot(eye_ts, lightReflection)), shininess);
        vec3 normalColor = vec3(0.f);
        normalColor +=  max(vec3(0), lightColors[1].xyz * texColor * kd);

        normalColor += ambient_color;

//        vec3 N = normalize((tangent * normal_ts.x) + (binormal * normal_ts.y) + (normal *normal_ts.z));
        if (useBumpMapping == 1) {
            fragColor = vec4(normalColor, 1.f);
        } else {
            fragColor = vec4(color * texColor, 1.f);
        }
}

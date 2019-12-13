//in vec3 light_ts;
//in vec3 eye_ts;

//uniform sampler2D bumpMap;
//uniform int useBumpMapping = 0;

//const int MAX_LIGHTS = 10;
//uniform vec3 lightColors[MAX_LIGHTS];

//uniform vec3 ambient_color;
//uniform vec3 diffuse_color;

//    if (useBumpMapping) {
//        vec3 normal_ts = normalize(texture(bumpMap, texc*repeatUV).rgb * 2.0 - 1.0);
//        float kd = min(max(dot(normal_ts, light_ts), 0.0), 1.0);
//        vec3 normColor = max(vec3(0), lightColors[0] * diffuse_color * kd) + ambient_color;
//        fragColor = vec4(normColor, 1.0);
//    }

#version 330 core

in vec3 color;
in vec2 texc;
out vec4 fragColor;

uniform sampler2D tex;
uniform int useTexture = 0;
uniform vec2 repeatUV;

void main(){
    vec3 texColor = texture(tex, texc*repeatUV).rgb;
    texColor = clamp(texColor + vec3(1-useTexture), vec3(0), vec3(1));
    fragColor = vec4(color * texColor, 1);
}

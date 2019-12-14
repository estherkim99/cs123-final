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

const int MAX_LIGHTS = 10;
uniform vec3 lightColors[MAX_LIGHTS];

uniform vec3 ambient_color;
uniform vec3 diffuse_color;

void main(){
    vec3 texColor = texture(tex, texc*repeatUV).rgb;
    texColor = clamp(texColor + vec3(1-useTexture), vec3(0), vec3(1));


    if (useBumpMapping == 0) {
        fragColor = vec4(color * texColor, 1);
    } else {

        vec3 normal_ts = texture(bumpMap, texc*repeatUV).rgb;
        normal_ts = normalize(normal_ts * 2.0 - 1.0);
        float kd = clamp(dot(normal_ts, -light_ts), 0.0, 1.0);

        fragColor = clamp(kd * vec4(diffuse_color, 1), vec4(0), vec4(1)) * 0.7 + 0.3 * vec4(color,1);
    }


}

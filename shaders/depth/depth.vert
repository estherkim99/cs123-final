#version 330 core

layout (location = 0) in vec3 position;

const int MAX_LIGHTS = 10;
uniform mat4 lightSpaceMatrix[MAX_LIGHTS]; // hard-coding number of lights to 10
uniform mat4 m;
uniform int lightIndex;

void main(void)
{
    gl_Position = lightSpaceMatrix[lightIndex] * m * vec4(position, 1.0);
}

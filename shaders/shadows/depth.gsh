#version 330 core
layout(location = 0) in vec3 position; // Position of the vertex
layout(location = 1) in vec3 normal;   // Normal of the vertex
layout(location = 5) in vec2 texCoord; // UV texture coordinates
layout(location = 10) in float arrowOffset; // Sideways offset for billboarded normal arrows

uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

  void main(void)
 {
      gl_Position = m * vec4(position, 1.0);
 }

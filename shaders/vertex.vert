#version 330 core

#ifdef GL_ES
precision mediump float;
#endif

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

out vec3 normal;
out vec3 position;

uniform mat4 u_MV;
uniform mat3 u_N;
uniform mat4 u_MVP;

void main() {
    normal = normalize(u_N * vertexNormal);
    position = vec3(u_MV * vec4(vertexPosition, 1.0));
    gl_Position = u_MVP * vec4(vertexPosition, 1.0);
}

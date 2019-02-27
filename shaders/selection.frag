#version 330 core

#ifdef GL_ES
precision mediump float;
#endif

uniform float u_time;
uniform vec3 u_selectionColor = vec3(0.0, 0.75, 1.0);

void main() {
    vec3 color = u_selectionColor * abs(sin(u_time));
    gl_FragColor = vec4(color, 1.0);
}

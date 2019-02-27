#version 330 core

#ifdef GL_ES
precision mediump float;
#endif

in vec3 normal;
in vec3 position;

layout (location = 0) out vec4 fragColor;

struct Light {
    vec4 position;
    vec3 intensity;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shine;
};

uniform Light u_light;
uniform Material u_material;

void main() {
    vec3 n = normalize(normal);
    vec3 s = normalize(u_light.position.xyz - position);
    vec3 v = normalize(-position.xyz);
    vec3 h = normalize(v + s);
    float sdn = dot(s, n);
    vec3 ambient  = u_material.ambient;
    vec3 diffuse  = u_material.diffuse * max(sdn, 0.0);
    vec3 specular = u_material.specular * mix(0.0, pow(dot(h, n), u_material.shine), step(0.0, sdn));
    fragColor = vec4(u_light.intensity * (ambient + diffuse + specular), 1);
}

#version 330 core

in vec3 normal;
in vec3 position;

layout (location = 0) out vec4 fragColor;

struct Light
{
    vec4 position;
    vec3 intensity;
};
uniform Light light;

struct Material {
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float shininess;
};
uniform Material material;

void main()
{
    vec3 n = normalize(normal);
    vec3 s = normalize(light.position.xyz - position);
    vec3 v = normalize(-position.xyz);
    vec3 h = normalize(v + s);
    float sdn = dot(s, n);
    vec3 ambient = material.Ka;
    vec3 diffuse = material.Kd * max(sdn, 0.0);
    vec3 specular = material.Ks * mix(0.0, pow(dot(h, n), material.shininess), step(0.0, sdn));
    fragColor = vec4(light.intensity * (ambient + diffuse + specular), 1);
}
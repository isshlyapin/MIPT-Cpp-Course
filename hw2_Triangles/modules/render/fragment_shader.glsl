#version 330 core

in vec3 Color;
in vec3 Normal;
in vec3 FragPos;

out vec4 fragColor;

uniform float ambient_strength;
uniform float specular_strength;

uniform vec3 view_pos;
uniform vec3 light_pos;
uniform vec3 light_color;

void main() {
    // Ambient lighting
    vec3 ambient = ambient_strength * light_color;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light_pos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light_color;

    // Specular lighting
    vec3 viewDir = normalize(view_pos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specular_strength * spec * light_color;

    // Combine results
    vec3 res_color = (ambient + diffuse + specular) * Color;

    fragColor = vec4(res_color, 1.0f);
}
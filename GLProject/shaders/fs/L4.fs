#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D sunTexture;

void main()
{
    vec3 color = texture(sunTexture, TexCoords).rgb;
    FragColor = vec4(color * 2.5, 1.0);
}

#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 Texture;

uniform sampler2D texture1;

void main()
{
    FragColor = textureProj(texture1, vec4(FragPos, 1.0));
}
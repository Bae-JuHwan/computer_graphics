#version 330 core

in vec3 fragColor;
in vec2 fragTexCoord;

out vec4 FragColor;

uniform sampler2D outTexture;

void main() {
    vec4 texColor = texture(outTexture, fragTexCoord);
    FragColor = texColor * vec4(fragColor, 1.0);
}
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main() {
    gl_Position = vec4(aPos, 1.0); // 변환 없이 NDC 좌표로 직접 출력
    TexCoord = aTexCoord;
}
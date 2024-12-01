#version 330 core

layout (location = 0) in vec3 vPos; // ���� ���� ��ǥ
layout (location = 1) in vec3 vNormal;  // ���� ���� ���� ����
layout (location = 2) in vec3 color;  // ���� ����

out vec3 FragPos;   // ��ȯ�� ������ ���� ���� ��ǥ
out vec3 Normal;    // ��ȯ�� ������ ����
out vec3 vColor;  // ���� ���� ����

uniform mat4 model; // �� ���. ���� ��ǥ�� ���� ��ǥ�� ��ȯ
uniform mat4 view;  // �� ���. ���� ��ǥ�� ī�޶� ��ǥ�� ��ȯ
uniform mat4 projection;    // ���� ���. ī�޶� ��ǥ�� Ŭ�� �������� ��ȯ

void main(void) 
{
    gl_Position = projection * view * model * vec4(vPos, 1.0);  // ���� ��ǥ�� ���� �������� Ŭ�� �������� ��ȯ

    FragPos = vec3(model * vec4(vPos, 1.0));    // ���� ���������� ���� ��ġ ���

    Normal = normalize(mat3(transpose(inverse(model))) * vNormal);  // �� ����� ������� ��ü����� ����� ��ȯ
    
    vColor = color;  // ���� ���� ����
}
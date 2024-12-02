#version 330 core

in vec3 FragPos;    // ���� ���������� ���� ��ǥ
in vec3 Normal; // ���� ���������� ���� ����
in vec3 vColor;  // ���� ����

out vec4 FragColor; // ���� ���� ����

uniform vec3 lightPos;   // ���� ��ġ
uniform vec3 viewPos;    // ī�޶� ��ġ
uniform vec3 lightColor; // ���� ����
uniform vec3 lightCubeColor; // ���� ������ü ����
uniform bool isLightCube; // ���� ������ü ����
uniform float lightIntensity;   // ������ ����

void main() 
{
    if (isLightCube) {
        // ���� ������ü�� ������ �������� ������
        FragColor = vec4(lightCubeColor, 1.0);
        return;
    }

    // Ambient ����
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor * lightIntensity;

    // Diffuse ����
    vec3 normalVector = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diffuseLight = max(dot(normalVector, lightDir), 0.0);
    vec3 diffuse = diffuseLight * lightColor * lightIntensity * vColor;

    // Specular ����
    float shininess = 128.0;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normalVector);
    float specularLight = max(dot(viewDir, reflectDir), 0.0);
    specularLight = pow(specularLight, shininess);
    vec3 specular = specularLight * lightColor * lightIntensity;;

    // ���� ���� ��� (���� ���� ���ϱ�)
    vec3 result = (ambient + diffuse + specular) * vColor;
    FragColor = vec4(result, 1.0);
}

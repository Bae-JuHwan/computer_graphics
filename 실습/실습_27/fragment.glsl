#version 330 core

in vec3 FragPos;    // 월드 공간에서의 정점 좌표
in vec3 Normal; // 월드 공간에서의 법선 벡터
in vec3 vColor;  // 정점 색상

out vec4 FragColor; // 계산된 최종 색상

uniform vec3 lightPos;   // 조명 위치
uniform vec3 viewPos;    // 카메라 위치
uniform vec3 lightColor; // 조명 색상
uniform vec3 lightCubeColor; // 조명 정육면체 색상
uniform bool isLightCube; // 조명 정육면체 여부
uniform float lightIntensity;   // 조명의 세기

void main() 
{
    if (isLightCube) {
        // 조명 정육면체는 지정된 색상으로 렌더링
        FragColor = vec4(lightCubeColor, 1.0);
        return;
    }

    // Ambient 조명
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor * lightIntensity;

    // Diffuse 조명
    vec3 normalVector = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diffuseLight = max(dot(normalVector, lightDir), 0.0);
    vec3 diffuse = diffuseLight * lightColor * lightIntensity * vColor;

    // Specular 조명
    float shininess = 128.0;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normalVector);
    float specularLight = max(dot(viewDir, reflectDir), 0.0);
    specularLight = pow(specularLight, shininess);
    vec3 specular = specularLight * lightColor * lightIntensity;;

    // 최종 색상 계산 (정점 색상 곱하기)
    vec3 result = (ambient + diffuse + specular) * vColor;
    FragColor = vec4(result, 1.0);
}

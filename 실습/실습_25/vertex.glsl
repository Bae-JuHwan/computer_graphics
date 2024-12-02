#version 330 core

layout (location = 0) in vec3 vPos; // 모델의 정점 좌표
layout (location = 1) in vec3 vNormal;  // 모델의 정점 법선 벡터
layout (location = 2) in vec3 color;  // 정점 색상

out vec3 FragPos;   // 변환된 정점의 월드 공간 좌표
out vec3 Normal;    // 변환된 정점의 법선
out vec3 vColor;  // 정점 색상 전달

uniform mat4 model; // 모델 행렬. 로컬 좌표를 월드 좌표로 변환
uniform mat4 view;  // 뷰 행렬. 월드 좌표를 카메라 좌표로 변환
uniform mat4 projection;    // 투영 행렬. 카메라 좌표를 클립 공간으로 변환

void main(void) 
{
    gl_Position = projection * view * model * vec4(vPos, 1.0);  // 정점 좌표를 로컬 공간에서 클립 공간으로 변환

    FragPos = vec3(model * vec4(vPos, 1.0));    // 월드 공간에서의 정점 위치 계산

    Normal = normalize(mat3(transpose(inverse(model))) * vNormal);  // 모델 행렬의 역행렬의 전체행렬을 사용해 변환
    
    vColor = color;  // 정점 색상 전달
}
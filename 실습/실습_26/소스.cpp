#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include "헤더.h"

GLuint vaoCube, vaoBottom, vaoMiddle, vaoLeftArm, vaoRightArm, vaoLeftBarrel, vaoRightBarrel, vaoLightCube;
GLuint vboCube[2], vboBottom[2], vboMiddle[2], vboLeftArm[2], vboRightArm[2], vboLeftBarrel[2], vboRightBarrel[2], vboLightCube[2];
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
Model modelCube, modelBottom, modelMiddle, modelLeftArm, modelRightArm, modelLeftBarrel, modelRightBarrel, modelLightCube;

GLfloat cameraX = 0.0f;
GLfloat cameraZ = 3.0f;
GLfloat cameraYrevolve = 0.0f;    // 공전
GLfloat lightAngle = 0.0f;

bool isCameraXmove = false;
bool isCameraXmoveReverse = false;
bool isCameraZmove = false;
bool isCameraZmoveReverse = false;
bool isCameraYrevolve = false;
bool isCameraYrevolveReverse = false;
bool isLightOn = false;
bool rotatePositiveY = false;
bool rotateNegativeY = false;

int currentLightColorIndex = 0; // 현재 조명 색상 상태를 나타내는 변수
glm::vec3 lightColors[] = {
    glm::vec3(1.0f, 1.0f, 1.0f), // 흰색
    glm::vec3(0.0f, 0.0f, 1.0f), // 파란색
    glm::vec3(1.0f, 0.0f, 0.0f)  // 빨간색
};

char* filetobuf(const char* file) {
    FILE* fptr;
    long length;
    char* buf;

    fptr = fopen(file, "rb");
    if (!fptr)
        return NULL;
    fseek(fptr, 0, SEEK_END);
    length = ftell(fptr);
    buf = (char*)malloc(length + 1);
    fseek(fptr, 0, SEEK_SET);
    fread(buf, length, 1, fptr);
    fclose(fptr);
    buf[length] = 0;
    return buf;
}

void make_vertexShaders();
void make_fragmentShaders();
void InitBuffer();
void InitCube();
void InitBottom();
void InitMiddle();
void InitLeftArm();
void InitRightArm();
void InitLeftBarrel();
void InitRightBarrel();
void InitLightCube();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer(int value);

int window_Width = 800;
int window_Height = 600;

void InitBuffer(GLuint& vao, GLuint* vbo, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(2, vbo);

    // 정점버퍼 (위치와 색상)
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // 버퍼 요소들 (인덱스)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // 위치
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);  // Position attribute
    glEnableVertexAttribArray(0);

    // 노멀
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // 색상
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);  // VAO 바인드 해제
}

void InitCube() {
    read_obj_file("cube.obj", modelCube); // OBJ 파일 읽기

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    glm::vec3 color = glm::vec3(0.0f, 0.0f, 1.0f);

    for (size_t i = 0; i < modelCube.faces.size(); ++i) {
        Vertex v1 = modelCube.vertices[modelCube.faces[i].v1];
        Vertex v2 = modelCube.vertices[modelCube.faces[i].v2];
        Vertex v3 = modelCube.vertices[modelCube.faces[i].v3];

        // 각 정점에 색상 추가
        v1.color = color;
        v2.color = color;
        v3.color = color;

        // 삼각형의 법선 계산
        glm::vec3 edge1 = glm::vec3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
        glm::vec3 edge2 = glm::vec3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        // 정점에 법선 추가
        v1.normal = normal;
        v2.normal = normal;
        v3.normal = normal;

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoCube, vboCube, expandedVertices, indices); // InitBuffer 호출
}

void InitBottom() {
    read_obj_file("bottom.obj", modelBottom);

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);

    for (size_t i = 0; i < modelBottom.faces.size(); ++i) {
        Vertex v1 = modelBottom.vertices[modelBottom.faces[i].v1];
        Vertex v2 = modelBottom.vertices[modelBottom.faces[i].v2];
        Vertex v3 = modelBottom.vertices[modelBottom.faces[i].v3];

        // 각 정점에 색상 추가
        v1.color = color;
        v2.color = color;
        v3.color = color;

        // 삼각형의 법선 계산
        glm::vec3 edge1 = glm::vec3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
        glm::vec3 edge2 = glm::vec3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        // 정점에 법선 추가
        v1.normal = normal;
        v2.normal = normal;
        v3.normal = normal;

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoBottom, vboBottom, expandedVertices, indices);
}

void InitMiddle() {
    read_obj_file("middle.obj", modelMiddle);

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f);

    for (size_t i = 0; i < modelMiddle.faces.size(); ++i) {
        Vertex v1 = modelMiddle.vertices[modelMiddle.faces[i].v1];
        Vertex v2 = modelMiddle.vertices[modelMiddle.faces[i].v2];
        Vertex v3 = modelMiddle.vertices[modelMiddle.faces[i].v3];

        // 각 정점에 색상 추가
        v1.color = color;
        v2.color = color;
        v3.color = color;

        // 삼각형의 법선 계산
        glm::vec3 edge1 = glm::vec3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
        glm::vec3 edge2 = glm::vec3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        // 정점에 법선 추가
        v1.normal = normal;
        v2.normal = normal;
        v3.normal = normal;

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoMiddle, vboMiddle, expandedVertices, indices);
}

void InitLeftArm() {
    read_obj_file("leftArm.obj", modelLeftArm);

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    glm::vec3 color = glm::vec3(1.0f, 1.0f, 0.0f);

    for (size_t i = 0; i < modelLeftArm.faces.size(); ++i) {
        Vertex v1 = modelLeftArm.vertices[modelLeftArm.faces[i].v1];
        Vertex v2 = modelLeftArm.vertices[modelLeftArm.faces[i].v2];
        Vertex v3 = modelLeftArm.vertices[modelLeftArm.faces[i].v3];

        // 각 정점에 색상 추가
        v1.color = color;
        v2.color = color;
        v3.color = color;

        // 삼각형의 법선 계산
        glm::vec3 edge1 = glm::vec3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
        glm::vec3 edge2 = glm::vec3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        // 정점에 법선 추가
        v1.normal = normal;
        v2.normal = normal;
        v3.normal = normal;

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoLeftArm, vboLeftArm, expandedVertices, indices);
}

void InitRightArm() {
    read_obj_file("rightArm.obj", modelRightArm);

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);

    for (size_t i = 0; i < modelRightArm.faces.size(); ++i) {
        Vertex v1 = modelRightArm.vertices[modelRightArm.faces[i].v1];
        Vertex v2 = modelRightArm.vertices[modelRightArm.faces[i].v2];
        Vertex v3 = modelRightArm.vertices[modelRightArm.faces[i].v3];

        // 각 정점에 색상 추가
        v1.color = color;
        v2.color = color;
        v3.color = color;

        // 삼각형의 법선 계산
        glm::vec3 edge1 = glm::vec3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
        glm::vec3 edge2 = glm::vec3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        // 정점에 법선 추가
        v1.normal = normal;
        v2.normal = normal;
        v3.normal = normal;

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoRightArm, vboRightArm, expandedVertices, indices);
}

void InitLeftBarrel() {
    read_obj_file("leftBarrel.obj", modelLeftBarrel);

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    glm::vec3 color = glm::vec3(1.0f, 0.0f, 1.0f);

    for (size_t i = 0; i < modelLeftBarrel.faces.size(); ++i) {
        Vertex v1 = modelLeftBarrel.vertices[modelLeftBarrel.faces[i].v1];
        Vertex v2 = modelLeftBarrel.vertices[modelLeftBarrel.faces[i].v2];
        Vertex v3 = modelLeftBarrel.vertices[modelLeftBarrel.faces[i].v3];

        // 각 정점에 색상 추가
        v1.color = color;
        v2.color = color;
        v3.color = color;

        // 삼각형의 법선 계산
        glm::vec3 edge1 = glm::vec3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
        glm::vec3 edge2 = glm::vec3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        // 정점에 법선 추가
        v1.normal = normal;
        v2.normal = normal;
        v3.normal = normal;

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoLeftBarrel, vboLeftBarrel, expandedVertices, indices);
}

void InitRightBarrel() {
    read_obj_file("rightBarrel.obj", modelRightBarrel);

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    glm::vec3 color = glm::vec3(1.0f, 0.0f, 1.0f);

    for (size_t i = 0; i < modelRightBarrel.faces.size(); ++i) {
        Vertex v1 = modelRightBarrel.vertices[modelRightBarrel.faces[i].v1];
        Vertex v2 = modelRightBarrel.vertices[modelRightBarrel.faces[i].v2];
        Vertex v3 = modelRightBarrel.vertices[modelRightBarrel.faces[i].v3];

        // 각 정점에 색상 추가
        v1.color = color;
        v2.color = color;
        v3.color = color;

        // 삼각형의 법선 계산
        glm::vec3 edge1 = glm::vec3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
        glm::vec3 edge2 = glm::vec3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        // 정점에 법선 추가
        v1.normal = normal;
        v2.normal = normal;
        v3.normal = normal;

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoRightBarrel, vboRightBarrel, expandedVertices, indices);
}

void InitLightCube() {
    read_obj_file("lightCube.obj", modelLightCube); // cube.obj 읽기

    glGenVertexArrays(1, &vaoLightCube);
    glBindVertexArray(vaoLightCube);

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> vertex_colors = {
        glm::vec3(1.0f, 0.0f, 0.0f),    // 빨간색
        glm::vec3(0.0f, 1.0f, 0.0f),    // 초록색
        glm::vec3(0.0f, 0.0f, 1.0f),    // 파란색
        glm::vec3(1.0f, 1.0f, 0.0f),    // 노란색
        glm::vec3(1.0f, 0.0f, 1.0f),    // 자홍색
        glm::vec3(0.0f, 1.0f, 1.0f),    // 청록색
        glm::vec3(1.0f, 0.5f, 0.0f),    // 주황색
        glm::vec3(0.5f, 0.0f, 0.5f)     // 보라색
    };

    for (size_t i = 0; i < modelLightCube.faces.size(); ++i) {
        Vertex v1 = modelLightCube.vertices[modelLightCube.faces[i].v1];
        Vertex v2 = modelLightCube.vertices[modelLightCube.faces[i].v2];
        Vertex v3 = modelLightCube.vertices[modelLightCube.faces[i].v3];

        v1.color = vertex_colors[modelLightCube.faces[i].v1];
        v2.color = vertex_colors[modelLightCube.faces[i].v2];
        v3.color = vertex_colors[modelLightCube.faces[i].v3];

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    glGenBuffers(2, vboLightCube);
    glBindBuffer(GL_ARRAY_BUFFER, vboLightCube[0]);
    glBufferData(GL_ARRAY_BUFFER, expandedVertices.size() * sizeof(Vertex), expandedVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboLightCube[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(window_Width, window_Height);
    glutCreateWindow("실습26");

    glewExperimental = GL_TRUE;
    glewInit();

    make_shaderProgram();
    InitCube();
    InitBottom();
    InitMiddle();
    InitLeftArm();
    InitRightArm();
    InitLeftBarrel();
    InitRightBarrel();
    InitLightCube();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutTimerFunc(16, Timer, 0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);

    glutMainLoop();
}

void make_vertexShaders() {
    GLchar* vertexSource;

    vertexSource = filetobuf("vertex.glsl");

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
    glCompileShader(vertexShader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }
}

void make_fragmentShaders() {
    GLchar* fragmentSource;

    fragmentSource = filetobuf("fragment.glsl");

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
    glCompileShader(fragmentShader);

    GLint result;
    GLchar errorLog[512];

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }
}

GLuint make_shaderProgram() {
    make_vertexShaders();
    make_fragmentShaders();

    shaderProgramID = glCreateProgram();

    glAttachShader(shaderProgramID, vertexShader);
    glAttachShader(shaderProgramID, fragmentShader);
    glLinkProgram(shaderProgramID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgramID);
    return shaderProgramID;
}

GLvoid drawScene() {
    glClearColor(1.0, 1.0, 1.0, 1.0f);  // 배경을 흰색으로 설정
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    // 공전 각도 계산
    float revolveAngle = glm::radians(cameraYrevolve);
    glm::mat4 revolveMatrix = glm::rotate(glm::mat4(1.0f), revolveAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 initialCameraPos(cameraX, 0.3f, cameraZ);
    glm::vec3 finalCameraPos = glm::vec3(revolveMatrix * glm::vec4(initialCameraPos, 1.0f));

    // 카메라가 보는 지점을 회전
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 direction = target - finalCameraPos;

    // 뷰 행렬 생성
    glm::mat4 viewMatrix = glm::lookAt(
        finalCameraPos,  // 카메라 위치
       target,   // 회전된 시점 (카메라가 바라보는 지점)
        glm::vec3(0.0f, 1.0f, 0.0f)  // 위쪽 방향
    );

    // 뷰 행렬을 셰이더에 전달
    GLint viewMatrixLocation = glGetUniformLocation(shaderProgramID, "view");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // 투영 변환
    glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(45.0f),
        (float)window_Width / (float)window_Height,
        0.1f,
        100.0f
    );

    GLint projMatrixLocation = glGetUniformLocation(shaderProgramID, "projection");
    glUniformMatrix4fv(projMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // 바닥
    glm::mat4 cubeModelMatrix = glm::mat4(1.0f);
    GLint modelMatrixLocation = glGetUniformLocation(shaderProgramID, "model");
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(cubeModelMatrix));

    glBindVertexArray(vaoCube);
    glDrawElements(GL_TRIANGLES, modelCube.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 탱크 바닥
    glm::mat4 bottomModelMatrix = glm::mat4(1.0f);
    bottomModelMatrix = glm::translate(bottomModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    bottomModelMatrix = glm::translate(bottomModelMatrix, glm::vec3(0.0f, 0.0625f, 0.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(bottomModelMatrix));

    glBindVertexArray(vaoBottom);
    glDrawElements(GL_TRIANGLES, modelBottom.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 탱크 중간
    glm::mat4 middleModelMatrix = glm::mat4(1.0f);
    middleModelMatrix = glm::translate(middleModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    middleModelMatrix = glm::translate(middleModelMatrix, glm::vec3(0.0f, 0.1425f, 0.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(middleModelMatrix));

    glBindVertexArray(vaoMiddle);
    glDrawElements(GL_TRIANGLES, modelMiddle.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 왼쪽 팔
    glm::mat4 leftArmModelMatrix = glm::mat4(1.0f);
    leftArmModelMatrix = glm::translate(leftArmModelMatrix, glm::vec3(-0.06f, 0.1f, 0.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(leftArmModelMatrix));

    glBindVertexArray(vaoLeftArm);
    glDrawElements(GL_TRIANGLES, modelLeftArm.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 오른쪽 팔
    glm::mat4 rightArmModelMatrix = glm::mat4(1.0f);
    rightArmModelMatrix = glm::translate(rightArmModelMatrix, glm::vec3(0.06f, 0.1f, 0.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(rightArmModelMatrix));

    glBindVertexArray(vaoRightArm);
    glDrawElements(GL_TRIANGLES, modelRightArm.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 왼쪽 포신
    glm::mat4 leftBarrelModelMatrix = glm::mat4(1.0f);
    leftBarrelModelMatrix = glm::translate(leftBarrelModelMatrix, glm::vec3(-0.06f, 0.1025f, 0.26f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(leftBarrelModelMatrix));

    glBindVertexArray(vaoLeftBarrel);
    glDrawElements(GL_TRIANGLES, modelLeftBarrel.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 오른쪽 포신
    glm::mat4 rightBarrelModelMatrix = glm::mat4(1.0f);
    rightBarrelModelMatrix = glm::translate(rightBarrelModelMatrix, glm::vec3(0.06f, 0.1025f, 0.26f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(rightBarrelModelMatrix));

    glBindVertexArray(vaoRightBarrel);
    glDrawElements(GL_TRIANGLES, modelRightBarrel.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 조명 공전 변환
    glm::mat4 orbitTransform = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 lightPos = glm::vec3(orbitTransform * glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)); // 공전 위치
    glm::vec3 lightColor = isLightOn ? lightColors[currentLightColorIndex] : glm::vec3(0.0f, 0.0f, 0.0f);  // 조명 색상
    glm::vec3 viewPos(0.0f, 0.0f, 10.0f);    // 카메라 위치

    GLuint lightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");
    GLuint viewPosLoc = glGetUniformLocation(shaderProgramID, "viewPos");

    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));

    // 최종 조명 위치를 유니폼에 전달
    GLuint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

    // 조명 도형 그리기
    glUniform1i(glGetUniformLocation(shaderProgramID, "isLightCube"), true);

    glm::vec3 lightCubeColor(0.5f, 0.5f, 0.5f);
    GLuint lightCubeColorLoc = glGetUniformLocation(shaderProgramID, "lightCubeColor");
    glUniform3fv(lightCubeColorLoc, 1, glm::value_ptr(lightCubeColor));

    glm::mat4 lightModelMatrix = glm::mat4(1.0f);
    lightModelMatrix = glm::translate(lightModelMatrix, lightPos);
    lightModelMatrix = glm::scale(lightModelMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightModelMatrix));

    glBindVertexArray(vaoLightCube);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glUniform1i(glGetUniformLocation(shaderProgramID, "isLightCube"), false);

    glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'q':
        glutLeaveMainLoop();
        break;
    case 'x':
        isCameraXmove = !isCameraXmove;
        break;
    case 'X':
        isCameraXmoveReverse = !isCameraXmoveReverse;
        break;
    case 'z':
        isCameraZmove = !isCameraZmove;
        break;
    case 'Z':
        isCameraZmoveReverse = !isCameraZmoveReverse;
        break;
    case 'r':
        isCameraYrevolve = !isCameraYrevolve;
        break;
    case 'R':
        isCameraYrevolveReverse = !isCameraYrevolveReverse;
        break;
    case 'm':
        isLightOn = !isLightOn;
        break;
    case 'c':
        currentLightColorIndex = (currentLightColorIndex + 1) % 3; // 0, 1, 2 순환
        glutPostRedisplay(); // 화면 갱신
        break;
    case 'y':
        rotatePositiveY = !rotatePositiveY;
        break;
    case 'Y':
        rotateNegativeY = !rotateNegativeY;
        break;
    case 's':
        isCameraXmove = false;
        isCameraXmoveReverse = false;
        isCameraZmove = false;
        isCameraZmoveReverse = false;
        isCameraYrevolve = false;
        isCameraYrevolveReverse = false;
        rotatePositiveY = false;
        rotateNegativeY = false;
        break;
    }
    glutPostRedisplay();
}

GLvoid Timer(int value) {
    if (isCameraXmove) {
        if (isCameraXmoveReverse) {
            cameraX -= 0.01f;
        }
        else {
            cameraX += 0.01f;
        }
    }

    if (isCameraZmove) {
        if (isCameraZmoveReverse) {
            cameraZ -= 0.01f;
        }
        else {
            cameraZ += 0.01f;
        }
    }

    if (isCameraYrevolve) {
        if (isCameraYrevolveReverse) {
            cameraYrevolve -= 1.5f;
        }
        else {
            cameraYrevolve += 1.5f;
        }
    }

    if (rotatePositiveY) {
        if (rotateNegativeY) {
            lightAngle -= 1.0f;
        }
        else {
            lightAngle += 1.0f;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, Timer, 0);
}
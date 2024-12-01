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

GLuint vaoAxis, vaoCube, vaoPyramid, vaoLightCube;
GLuint vboAxis, vboCube[2], vboPyramid[2], vboLightCube[2];
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
Model modelCube, modelPyramid, modelLightCube;

GLfloat lightAngle = 0.0f;
GLfloat lightZmove = 3.0f;

bool drawCube = false;
bool cullingEnabled = false;
bool wireframeMode = false;
GLfloat yRotationAngle = 0.0f;
bool rotatePositiveY = false;
bool rotateNegativeY = false;
bool isLightOn = false; 
bool orbitPositiveY = false;
bool orbitNegativeY = false;
bool isLightZmove = false;
bool isLightZmoveReverse = false;

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
void InitCube();
void InitPyramid();
void InitLightCube();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid SpecialKeys(int key, int x, int y);
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

    std::vector<glm::vec3> face_colors = {
        glm::vec3(1.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    };

    for (size_t i = 0; i < modelCube.faces.size(); ++i) {
        Vertex v1 = modelCube.vertices[modelCube.faces[i].v1];
        Vertex v2 = modelCube.vertices[modelCube.faces[i].v2];
        Vertex v3 = modelCube.vertices[modelCube.faces[i].v3];

        // 각 정점에 색상 추가
        v1.color = face_colors[i / 2];
        v2.color = face_colors[i / 2];
        v3.color = face_colors[i / 2];

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

void InitPyramid() {
    read_obj_file("pyramid.obj", modelPyramid); // pyramid.obj 읽기

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> vertex_colors = {
        glm::vec3(1.0f, 0.0f, 0.0f),    // 빨간색
        glm::vec3(0.0f, 1.0f, 0.0f),    // 초록색
        glm::vec3(0.0f, 0.0f, 1.0f),    // 파란색
        glm::vec3(1.0f, 1.0f, 0.0f),    // 노란색
        glm::vec3(0.0f, 1.0f, 1.0f),    // 청록색
    };

    for (size_t i = 0; i < modelPyramid.faces.size(); ++i) {
        Vertex v1 = modelPyramid.vertices[modelPyramid.faces[i].v1];
        Vertex v2 = modelPyramid.vertices[modelPyramid.faces[i].v2];
        Vertex v3 = modelPyramid.vertices[modelPyramid.faces[i].v3];

        // 각 정점의 색상 설정
        v1.color = vertex_colors[i % vertex_colors.size()];
        v2.color = vertex_colors[i % vertex_colors.size()];
        v3.color = vertex_colors[i % vertex_colors.size()];

        // 삼각형의 법선 계산
        glm::vec3 edge1 = glm::vec3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
        glm::vec3 edge2 = glm::vec3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        // 각 정점에 동일한 법선 설정
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

    InitBuffer(vaoPyramid, vboPyramid, expandedVertices, indices); // InitBuffer 호출
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
    glutCreateWindow("Example24");

    glewExperimental = GL_TRUE;
    glewInit();

    make_shaderProgram();
    InitCube();
    InitPyramid();
    InitLightCube();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutTimerFunc(16, Timer, 0);
    glEnable(GL_DEPTH_TEST);

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
    glClearColor(0.0, 0.0, 0.0, 1.0f);  // 배경을 검은색으로 설정
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    // 뷰 변환
    glm::mat4 viewMatrix = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 10.0f),  // 카메라 위치
        glm::vec3(0.0f, 0.0f, 0.0f),   // 카메라가 바라보는 지점
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

    // 정육면체 및 피라미드의 초기 모델 행렬 (기본 X, Y축 회전 포함)
    glm::mat4 baseModelMatrix = glm::mat4(1.0f);
    baseModelMatrix = glm::rotate(baseModelMatrix, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Y축 기본 회전
    baseModelMatrix = glm::rotate(baseModelMatrix, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // X축 기본 회전

    // 정육면체 및 피라미드의 추가 Y축 회전
    glm::mat4 cubeModelMatrix = glm::rotate(baseModelMatrix, glm::radians(yRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 pyramidModelMatrix = glm::rotate(baseModelMatrix, glm::radians(yRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));

    unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "model");

    // 정육면체 렌더링
    if (drawCube) {
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cubeModelMatrix));
        glBindVertexArray(vaoCube);
        glDrawElements(GL_TRIANGLES, modelCube.faces.size() * 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    // 피라미드 렌더링
    else {
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(pyramidModelMatrix));
        glBindVertexArray(vaoPyramid);
        glDrawElements(GL_TRIANGLES, modelPyramid.faces.size() * 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    // 조명 공전 변환
    glm::mat4 orbitTransform = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 lightPos = glm::vec3(orbitTransform * glm::vec4(0.0f, 0.0f, lightZmove, 1.0f)); // 공전 위치
    glm::vec3 lightColor = isLightOn ? glm::vec3(1.0f, 1.0f, 1.0f) : glm::vec3(0.0f, 0.0f, 0.0f);  // 조명 색상 (흰색)
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

    glm::mat4 lightModelMatrix = glm::translate(baseModelMatrix, lightPos);
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(lightModelMatrix));

    glBindVertexArray(vaoLightCube);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glUniform1i(glGetUniformLocation(shaderProgramID, "isLightCube"), false);

    glutSwapBuffers();  // 더블 버퍼링으로 화면 갱신
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'q':
        glutLeaveMainLoop();
        break;
    case 'n':
        drawCube = !drawCube;
        break;
    case 'h':
        cullingEnabled = !cullingEnabled;
        if (cullingEnabled) {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        }
        else {
            glDisable(GL_CULL_FACE);
        }
        break;
    case 'w':
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case 'W':
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case 'y':
        rotatePositiveY = !rotatePositiveY;
        break;
    case 'Y':
        rotateNegativeY = !rotateNegativeY;
        break;
    case 's':
        yRotationAngle = 0.0f;
        rotatePositiveY = false;
        rotateNegativeY = false;
        break;
    case 'm':
        isLightOn = !isLightOn;
        break;
    case 'r':
        orbitPositiveY = !orbitPositiveY;
        orbitNegativeY = false;
        break;
    case 'R':
        orbitNegativeY = !orbitNegativeY;
        orbitPositiveY = false;
        break;
    case 'z':
        isLightZmove = !isLightZmove;
        break;
    case 'Z':
        isLightZmoveReverse = !isLightZmoveReverse;
        break;
    }
    glutPostRedisplay();
}

GLvoid Timer(int value) {
    if (rotatePositiveY) {
        yRotationAngle += 1.0f;
    }
    if (rotateNegativeY) {
        yRotationAngle -= 1.0f;
    }
    if (orbitPositiveY) {
        lightAngle += 1.0f;
    }
    if (orbitNegativeY) {
        lightAngle -= 1.0f;
    }
    if (yRotationAngle > 360.0f) yRotationAngle -= 360.0f;
    if (yRotationAngle < 360.0f) yRotationAngle += 360.0f;

    if (lightAngle > 360.0f) lightAngle -= 360.0f;
    if (lightAngle < 0.0f) lightAngle += 360.0f;

    if (isLightZmove) {
        lightZmove += 0.01f;
    }
    if (isLightZmoveReverse) {
        lightZmove -= 0.01f;
    }

    glutPostRedisplay();

    glutTimerFunc(16, Timer, 0);
}
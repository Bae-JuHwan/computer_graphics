#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <set>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include "헤더.h"

struct AABB {
    glm::vec3 min; // 충돌박스의 최소 좌표 (x, y, z)
    glm::vec3 max; // 충돌박스의 최대 좌표 (x, y, z)
};

AABB block1Box, block2Box, block3Box, robotBox, cubeFaceBoxes[6];

GLuint vaoBottom, vaoArrowAndPillar, vaoEndPoint, vaoPoint;
GLuint vboBottom[2], vboArrowAndPillar[2], vboEndPoint[2], vboPoint[2];
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
Model modelBottom, modelArrowAndPillar, modelEndPoint, modelPoint;

GLfloat cameraX = 0.0f;
GLfloat cameraY = 0.0f;
GLfloat cameraZ = 30.0f;

bool isCameraXmove = false;
bool isCameraXmoveReverse = false;
bool isCameraYmove = false;
bool isCameraYmoveReverse = false;
bool isCameraZmove = false;
bool isCameraZmoveReverse = false;

bool checkCollision(const AABB& box1, const AABB& box2) {
    return (box1.max.x > box2.min.x && box1.min.x < box2.max.x &&
        box1.max.y > box2.min.y && box1.min.y < box2.max.y &&
        box1.max.z > box2.min.z && box1.min.z < box2.max.z);
}

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
void InitBottom();
void InitArrowAndPillar();
void InitEndPoint();
void InitPoint();

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

    // 정점을 사용하게 함
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);  // Position attribute
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));  // Color attribute
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);  // VAO 바인드 해제
}

void InitBottom() {
    read_obj_file("arrowAndPillar.obj", modelArrowAndPillar);  // OBJ 파일 읽기

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    glm::vec3 color = glm::vec3(1.0f, 1.0f, 0.0f);

    for (size_t i = 0; i < modelArrowAndPillar.faces.size(); ++i) {
        Vertex v1 = { modelArrowAndPillar.vertices[modelArrowAndPillar.faces[i].v1].x,
                     modelArrowAndPillar.vertices[modelArrowAndPillar.faces[i].v1].y,
                     modelArrowAndPillar.vertices[modelArrowAndPillar.faces[i].v1].z,
                     color };

        Vertex v2 = { modelArrowAndPillar.vertices[modelArrowAndPillar.faces[i].v2].x,
                     modelArrowAndPillar.vertices[modelArrowAndPillar.faces[i].v2].y,
                     modelArrowAndPillar.vertices[modelArrowAndPillar.faces[i].v2].z,
                     color };

        Vertex v3 = { modelArrowAndPillar.vertices[modelArrowAndPillar.faces[i].v3].x,
                     modelArrowAndPillar.vertices[modelArrowAndPillar.faces[i].v3].y,
                     modelArrowAndPillar.vertices[modelArrowAndPillar.faces[i].v3].z,
                     color };

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoArrowAndPillar, vboArrowAndPillar, expandedVertices, indices);  // InitBuffer 호출
}

void InitArrowAndPillar() {
    read_obj_file("bottom.obj", modelBottom);  // OBJ 파일 읽기

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    glm::vec3 color = glm::vec3(0.0f, 0.0f, 1.0f);

    for (size_t i = 0; i < modelBottom.faces.size(); ++i) {
        Vertex v1 = { modelBottom.vertices[modelBottom.faces[i].v1].x,
                     modelBottom.vertices[modelBottom.faces[i].v1].y,
                     modelBottom.vertices[modelBottom.faces[i].v1].z,
                     color };

        Vertex v2 = { modelBottom.vertices[modelBottom.faces[i].v2].x,
                     modelBottom.vertices[modelBottom.faces[i].v2].y,
                     modelBottom.vertices[modelBottom.faces[i].v2].z,
                     color };

        Vertex v3 = { modelBottom.vertices[modelBottom.faces[i].v3].x,
                     modelBottom.vertices[modelBottom.faces[i].v3].y,
                     modelBottom.vertices[modelBottom.faces[i].v3].z,
                     color };

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoBottom, vboBottom, expandedVertices, indices);  // InitBuffer 호출
}

void InitEndPoint() {
    read_obj_file("endPoint.obj", modelEndPoint);  // OBJ 파일 읽기

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    glm::vec3 color = glm::vec3(1.0f, 0.0f, 1.0f);

    for (size_t i = 0; i < modelEndPoint.faces.size(); ++i) {
        Vertex v1 = { modelEndPoint.vertices[modelEndPoint.faces[i].v1].x,
                     modelEndPoint.vertices[modelEndPoint.faces[i].v1].y,
                     modelEndPoint.vertices[modelEndPoint.faces[i].v1].z,
                     color };

        Vertex v2 = { modelEndPoint.vertices[modelEndPoint.faces[i].v2].x,
                     modelEndPoint.vertices[modelEndPoint.faces[i].v2].y,
                     modelEndPoint.vertices[modelEndPoint.faces[i].v2].z,
                     color };

        Vertex v3 = { modelEndPoint.vertices[modelEndPoint.faces[i].v3].x,
                     modelEndPoint.vertices[modelEndPoint.faces[i].v3].y,
                     modelEndPoint.vertices[modelEndPoint.faces[i].v3].z,
                     color };

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoEndPoint, vboEndPoint, expandedVertices, indices);  // InitBuffer 호출
}

void InitPoint() {
    read_obj_file("point.obj", modelPoint);  // OBJ 파일 읽기

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);

    for (size_t i = 0; i < modelPoint.faces.size(); ++i) {
        Vertex v1 = { modelPoint.vertices[modelPoint.faces[i].v1].x,
                     modelPoint.vertices[modelPoint.faces[i].v1].y,
                     modelPoint.vertices[modelPoint.faces[i].v1].z,
                     color };

        Vertex v2 = { modelPoint.vertices[modelPoint.faces[i].v2].x,
                     modelPoint.vertices[modelPoint.faces[i].v2].y,
                     modelPoint.vertices[modelPoint.faces[i].v2].z,
                     color };

        Vertex v3 = { modelPoint.vertices[modelPoint.faces[i].v3].x,
                     modelPoint.vertices[modelPoint.faces[i].v3].y,
                     modelPoint.vertices[modelPoint.faces[i].v3].z,
                     color };

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoPoint, vboPoint, expandedVertices, indices);  // InitBuffer 호출
}

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(window_Width, window_Height);
    glutCreateWindow("team_project");

    glewExperimental = GL_TRUE;
    glewInit();

    make_shaderProgram();
    InitBottom();
    InitArrowAndPillar();
    InitEndPoint();
    InitPoint();

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

    // 뷰 변환
    glm::mat4 viewMatrix = glm::lookAt(
        glm::vec3(cameraX, cameraY, cameraZ),  // 카메라 위치
        glm::vec3(0.0f, 0.0f, 0.0f),   // 카메라가 바라보는 지점
        glm::vec3(0.0f, 1.0f, 0.0f)  // 위쪽 방향
    );

    // 뷰 행렬을 셰이더에 전달
    GLint viewMatrixLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // 투영 변환
    glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(45.0f),
        (float)window_Width / (float)window_Height,
        0.1f,
        100.0f
    );

    GLint projMatrixLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
    glUniformMatrix4fv(projMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // 바닥
    glm::mat4 bottomModelMatrix = glm::mat4(1.0f);
    GLint modelMatrixLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(bottomModelMatrix));

    glBindVertexArray(vaoBottom);
    glDrawElements(GL_TRIANGLES, modelBottom.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 화살표와 기둥
    glm::mat4 arrowAndPillarModelMatrix = glm::mat4(1.0f);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(arrowAndPillarModelMatrix));

    glBindVertexArray(vaoArrowAndPillar);
    glDrawElements(GL_TRIANGLES, modelArrowAndPillar.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 결승점
    glm::mat4 endPointModelMatrix = glm::mat4(1.0f);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(endPointModelMatrix));

    glBindVertexArray(vaoEndPoint);
    glDrawElements(GL_TRIANGLES, modelEndPoint.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // 포인트
    glm::mat4 pointModelMatrix = glm::mat4(1.0f);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(pointModelMatrix));

    glBindVertexArray(vaoPoint);
    glDrawElements(GL_TRIANGLES, modelPoint.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

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
    case 'z':
        isCameraZmove = !isCameraZmove;
        break;
    case 'Z':
        isCameraZmoveReverse = !isCameraZmoveReverse;
        break;
    case 'x':
        isCameraXmove = !isCameraXmove;
        break;
    case 'X':
        isCameraXmoveReverse = !isCameraXmoveReverse;
        break;
    case 'y':
        isCameraYmove = !isCameraYmove;
        break;
    case 'Y':
        isCameraYmoveReverse = !isCameraYmoveReverse;
        break;
    }

    glutPostRedisplay();
}

GLvoid Timer(int value) {
    if (isCameraXmove) {
        if (isCameraXmoveReverse) {
            cameraX -= 0.1f;
        }
        else {
            cameraX += 0.1f;
        }
    }

    if (isCameraYmove) {
        if (isCameraYmoveReverse) {
            cameraY -= 0.1f;
        }
        else {
            cameraY += 0.1f;
        }
    }

    if (isCameraZmove) {
        if (isCameraZmoveReverse) {
            cameraZ -= 0.1f;
        }
        else {
            cameraZ += 0.1f;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, Timer, 0);
}
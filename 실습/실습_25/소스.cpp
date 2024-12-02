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
#include "���.h"

GLuint vaoEarth, vaoVenus, vaoMercury, vaoLightCube;
GLuint vboEarth[2], vboVenus[2], vboMercury[2], vboLightCube[2];
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
Model modelEarth, modelVenus, modelMercury, modelLightCube;

GLfloat lightAngle = 0.0f;

bool orbitPositiveY = false;
bool orbitNegativeY = false;

int currentLightColorIndex = 0; // ���� ���� ���� ���¸� ��Ÿ���� ����
glm::vec3 lightColors[] = {
    glm::vec3(1.0f, 1.0f, 1.0f), // ���
    glm::vec3(0.0f, 0.0f, 1.0f), // �Ķ���
    glm::vec3(1.0f, 0.0f, 0.0f)  // ������
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
void InitEarth();
void InitVenus();
void InitMercury();
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

    // �������� (��ġ�� ����)
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // ���� ��ҵ� (�ε���)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // ��ġ
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);  // Position attribute
    glEnableVertexAttribArray(0);
    
    // ���
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // ����
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);  // VAO ���ε� ����
}

void InitEarth() {
    read_obj_file("earth.obj", modelEarth); // OBJ ���� �б�

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);

    for (size_t i = 0; i < modelEarth.faces.size(); ++i) {
        Vertex v1 = modelEarth.vertices[modelEarth.faces[i].v1];
        Vertex v2 = modelEarth.vertices[modelEarth.faces[i].v2];
        Vertex v3 = modelEarth.vertices[modelEarth.faces[i].v3];

        // �� ������ ���� �߰�
        v1.color = color;
        v2.color = color;
        v3.color = color;

        // �ﰢ���� ���� ���
        glm::vec3 edge1 = glm::vec3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
        glm::vec3 edge2 = glm::vec3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        // ������ ���� �߰�
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

    InitBuffer(vaoEarth, vboEarth, expandedVertices, indices); // InitBuffer ȣ��
}

void InitVenus() {
    read_obj_file("venus.obj", modelVenus); // OBJ ���� �б�

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f);

    for (size_t i = 0; i < modelVenus.faces.size(); ++i) {
        Vertex v1 = modelVenus.vertices[modelVenus.faces[i].v1];
        Vertex v2 = modelVenus.vertices[modelVenus.faces[i].v2];
        Vertex v3 = modelVenus.vertices[modelVenus.faces[i].v3];
        
        // �� ������ ���� �߰�
        v1.color = color;
        v2.color = color;
        v3.color = color;

        // �ﰢ���� ���� ���
        glm::vec3 edge1 = glm::vec3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
        glm::vec3 edge2 = glm::vec3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        // ������ ���� �߰�
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

    InitBuffer(vaoVenus, vboVenus, expandedVertices, indices); // InitBuffer ȣ��
}

void InitMercury() {
    read_obj_file("mercury.obj", modelMercury); // OBJ ���� �б�

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    glm::vec3 color = glm::vec3(0.0f, 0.0f, 1.0f);

    for (size_t i = 0; i < modelMercury.faces.size(); ++i) {
        Vertex v1 = modelMercury.vertices[modelMercury.faces[i].v1];
        Vertex v2 = modelMercury.vertices[modelMercury.faces[i].v2];
        Vertex v3 = modelMercury.vertices[modelMercury.faces[i].v3];

        // �� ������ ���� �߰�
        v1.color = color;
        v2.color = color;
        v3.color = color;

        // �ﰢ���� ���� ���
        glm::vec3 edge1 = glm::vec3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
        glm::vec3 edge2 = glm::vec3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        // ������ ���� �߰�
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

    InitBuffer(vaoMercury, vboMercury, expandedVertices, indices); // InitBuffer ȣ��
}

void InitLightCube() {
    read_obj_file("lightCube.obj", modelLightCube); // cube.obj �б�

    glGenVertexArrays(1, &vaoLightCube);
    glBindVertexArray(vaoLightCube);

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> vertex_colors = {
        glm::vec3(1.0f, 0.0f, 0.0f),    // ������
        glm::vec3(0.0f, 1.0f, 0.0f),    // �ʷϻ�
        glm::vec3(0.0f, 0.0f, 1.0f),    // �Ķ���
        glm::vec3(1.0f, 1.0f, 0.0f),    // �����
        glm::vec3(1.0f, 0.0f, 1.0f),    // ��ȫ��
        glm::vec3(0.0f, 1.0f, 1.0f),    // û�ϻ�
        glm::vec3(1.0f, 0.5f, 0.0f),    // ��Ȳ��
        glm::vec3(0.5f, 0.0f, 0.5f)     // �����
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
    glutCreateWindow("�ǽ�25");

    glewExperimental = GL_TRUE;
    glewInit();

    make_shaderProgram();
    InitEarth();
    InitVenus();
    InitMercury();
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
        std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
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
        std::cerr << "ERROR: frag_shader ������ ����\n" << errorLog << std::endl;
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
    glClearColor(0.0, 0.0, 0.0, 1.0f);  // ����� ���������� ����
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    // �� ��ȯ
    glm::mat4 viewMatrix = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 20.0f),  // ī�޶� ��ġ
        glm::vec3(0.0f, 0.0f, 0.0f),   // ī�޶� �ٶ󺸴� ����
        glm::vec3(0.0f, 1.0f, 0.0f)  // ���� ����
    );

    // �� ����� ���̴��� ����
    GLint viewMatrixLocation = glGetUniformLocation(shaderProgramID, "view");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // ���� ��ȯ
    glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(45.0f),
        (float)window_Width / (float)window_Height,
        0.1f,
        100.0f
    );

    GLint projMatrixLocation = glGetUniformLocation(shaderProgramID, "projection");
    glUniformMatrix4fv(projMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // ����
    glm::mat4 earthModelMatrix = glm::mat4(1.0f);
    GLint modelMatrixLocation = glGetUniformLocation(shaderProgramID, "model");
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(earthModelMatrix));
    glBindVertexArray(vaoEarth);
    glDrawElements(GL_TRIANGLES, modelEarth.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // �ݼ�
    glm::mat4 venusModelMatrix = glm::mat4(1.0f);
    venusModelMatrix = glm::translate(venusModelMatrix, glm::vec3(-6.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(venusModelMatrix));
    glBindVertexArray(vaoVenus);
    glDrawElements(GL_TRIANGLES, modelVenus.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // ����
    glm::mat4 mercuryModelMatrix = glm::mat4(1.0f);
    mercuryModelMatrix = glm::translate(mercuryModelMatrix, glm::vec3(-10.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(mercuryModelMatrix));
    glBindVertexArray(vaoMercury);
    glDrawElements(GL_TRIANGLES, modelMercury.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // ���� ���� ��ȯ
    glm::mat4 orbitTransform = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 lightPos = glm::vec3(orbitTransform * glm::vec4(0.0f, 0.0f, 7.0f, 1.0f)); // ���� ��ġ
    glm::vec3 lightColor = lightColors[currentLightColorIndex];  // ���� ����
    glm::vec3 viewPos(0.0f, 0.0f, 10.0f);    // ī�޶� ��ġ

    GLuint lightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");
    GLuint viewPosLoc = glGetUniformLocation(shaderProgramID, "viewPos");

    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));

    // ���� ���� ��ġ�� �������� ����
    GLuint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

    // ���� ���� �׸���
    glUniform1i(glGetUniformLocation(shaderProgramID, "isLightCube"), true);

    glm::vec3 lightCubeColor(0.5f, 0.5f, 0.5f);
    GLuint lightCubeColorLoc = glGetUniformLocation(shaderProgramID, "lightCubeColor");
    glUniform3fv(lightCubeColorLoc, 1, glm::value_ptr(lightCubeColor));

    glm::mat4 lightModelMatrix = glm::translate(glm::mat4(1.0f), lightPos);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightModelMatrix));

    glBindVertexArray(vaoLightCube);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glUniform1i(glGetUniformLocation(shaderProgramID, "isLightCube"), false);

    glutSwapBuffers();  // ���� ���۸����� ȭ�� ����
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'q':
        glutLeaveMainLoop();
        break;
    case 'r':
        orbitPositiveY = !orbitPositiveY;
        orbitNegativeY = false;
        break;
    case 'R':
        orbitNegativeY = !orbitNegativeY;
        orbitPositiveY = false;
        break;
    case 'c':
        currentLightColorIndex = (currentLightColorIndex + 1) % 3; // 0, 1, 2 ��ȯ
        glutPostRedisplay(); // ȭ�� ����
        break;
    }
    glutPostRedisplay();
}

GLvoid Timer(int value) {
    if (orbitPositiveY) {
        lightAngle += 1.0f;
    }
    if (orbitNegativeY) {
        lightAngle -= 1.0f;
    }

    if (lightAngle > 360.0f) lightAngle -= 360.0f;
    if (lightAngle < 0.0f) lightAngle += 360.0f;

    glutPostRedisplay();

    glutTimerFunc(16, Timer, 0);
}
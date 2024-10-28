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

GLuint vaoAxis, vaoCube, vaoPyramid;
GLuint vboAxis, vboCube[2], vboPyramid[2];
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
Model modelCube, modelPyramid;

bool drawCube = false;
bool drawPyramid = false;
bool cullingEnabled = false;
bool wireframeMode = false;

GLfloat xRotationAngle = 0.0f;
GLfloat yRotationAngle = 0.0f;
bool rotatePositiveX = false;
bool rotateNegativeX = false;
bool rotatePositiveY = false;
bool rotateNegativeY = false;
GLfloat objectPositionX = 0.0f;
GLfloat objectPositionY = 0.0f;

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
void InitAxis();
void DrawAxis();
void InitCube();
void InitPyramid();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid SpecialKeys(int key, int x, int y);
GLvoid Timer(int value);

int window_Width = 800;
int window_Height = 600;

void InitAxis() {
    GLfloat axisVertices[] = {
        // X�� ��ǥ�� ���� (������)
        -1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // ��ǥ, ������
         1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // ��ǥ, ������

         // Y�� ��ǥ�� ���� (�ʷϻ�)
          0.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // ��ǥ, �ʷϻ�
          0.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f   // ��ǥ, �ʷϻ�
    };

    glGenVertexArrays(1, &vaoAxis);
    glBindVertexArray(vaoAxis);

    glGenBuffers(1, &vboAxis);
    glBindBuffer(GL_ARRAY_BUFFER, vboAxis);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

    // ��ġ �Ӽ� (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // ���� �Ӽ� (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);  // VAO ����ε�
}

void DrawAxis() {
    glBindVertexArray(vaoAxis);  // ��ǥ �� VAO ���ε�

    // ���� ��� ����Ͽ� ��ǥ �࿡�� ��ȯ ���� X
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(identityMatrix));

    // X��� Y�� �׸���
    glDrawArrays(GL_LINES, 0, 2);  // X��
    glDrawArrays(GL_LINES, 2, 2);  // Y��

    glBindVertexArray(0);  // VAO ����ε�
}

void InitCube() {
    read_obj_file("cube.obj", modelCube); // cube.obj �б�

    glGenVertexArrays(1, &vaoCube);
    glBindVertexArray(vaoCube);

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

    for (size_t i = 0; i < modelCube.faces.size(); ++i) {
        Vertex v1 = modelCube.vertices[modelCube.faces[i].v1];
        Vertex v2 = modelCube.vertices[modelCube.faces[i].v2];
        Vertex v3 = modelCube.vertices[modelCube.faces[i].v3];

        v1.color = vertex_colors[modelCube.faces[i].v1];
        v2.color = vertex_colors[modelCube.faces[i].v2];
        v3.color = vertex_colors[modelCube.faces[i].v3];

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    glGenBuffers(2, vboCube);
    glBindBuffer(GL_ARRAY_BUFFER, vboCube[0]);
    glBufferData(GL_ARRAY_BUFFER, expandedVertices.size() * sizeof(Vertex), expandedVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboCube[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void InitPyramid() {
    read_obj_file("pyramid.obj", modelPyramid); // pyramid.obj �б�

    glGenVertexArrays(1, &vaoPyramid);
    glBindVertexArray(vaoPyramid);

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> vertex_colors = {
        glm::vec3(1.0f, 0.0f, 0.0f),    // ������
        glm::vec3(0.0f, 1.0f, 0.0f),    // �ʷϻ�
        glm::vec3(0.0f, 0.0f, 1.0f),    // �Ķ���
        glm::vec3(1.0f, 1.0f, 0.0f),    // �����
        glm::vec3(0.0f, 1.0f, 1.0f),    // û�ϻ�
    };

    for (size_t i = 0; i < modelPyramid.faces.size(); ++i) {
        Vertex v1 = modelPyramid.vertices[modelPyramid.faces[i].v1];
        Vertex v2 = modelPyramid.vertices[modelPyramid.faces[i].v2];
        Vertex v3 = modelPyramid.vertices[modelPyramid.faces[i].v3];

        v1.color = vertex_colors[modelPyramid.faces[i].v1];
        v2.color = vertex_colors[modelPyramid.faces[i].v2];
        v3.color = vertex_colors[modelPyramid.faces[i].v3];

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    glGenBuffers(2, vboPyramid);
    glBindBuffer(GL_ARRAY_BUFFER, vboPyramid[0]);
    glBufferData(GL_ARRAY_BUFFER, expandedVertices.size() * sizeof(Vertex), expandedVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboPyramid[1]);
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
    glutCreateWindow("Example13");

    glewExperimental = GL_TRUE;
    glewInit();

    make_shaderProgram();
    InitCube();
    InitPyramid();
    InitAxis();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(SpecialKeys);
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
    glClearColor(1.0, 1.0, 1.0, 1.0f);  // ����� ������� ����
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    DrawAxis();

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(xRotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));  // x�� ȸ��
    modelMatrix = glm::rotate(modelMatrix, glm::radians(yRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));  // y�� ȸ��
    modelMatrix = glm::translate(modelMatrix, glm::vec3(objectPositionX, objectPositionY, 0.0f));  // ��ġ ��ȯ

    unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");

    if (drawCube) {
        // ������ü ȸ�� ��� ����
        glm::mat4 cubeMatrix = glm::rotate(modelMatrix, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // Y�� ȸ��
        cubeMatrix = glm::rotate(cubeMatrix, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));  // X�� ȸ��

        // ������ü�� ������ �� ��ȯ ��� ����
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(cubeMatrix));

        // ������ü VAO �ٽ� ���ε�
        glBindVertexArray(vaoCube);

        // ������ü �׸���
        glDrawElements(GL_TRIANGLES, modelCube.faces.size() * 3, GL_UNSIGNED_INT, 0);

        // VAO ����ε�
        glBindVertexArray(0);
    }

    else if (drawPyramid) {
        glm::mat4 pyramidMatrix = glm::rotate(modelMatrix, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // Y�� ȸ��
        pyramidMatrix = glm::rotate(pyramidMatrix, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));  // X�� ȸ��

        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(pyramidMatrix));

        glBindVertexArray(vaoPyramid);

        glDrawElements(GL_TRIANGLES, modelPyramid.faces.size() * 3, GL_UNSIGNED_INT, 0);

        // VAO ����ε�
        glBindVertexArray(0);
    }

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
    case 'c':
        drawCube = true;
        break;
    case 'p':
        drawPyramid = true;
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
    case 'x':
        rotatePositiveX = true;
        rotateNegativeX = false;
        break;
    case 'X':
        rotatePositiveX = false;
        rotateNegativeX = true;
        break;
    case 'y':
        rotatePositiveY = true;
        rotateNegativeY = false;
        break;
    case 'Y':
        rotatePositiveY = false;
        rotateNegativeY = true;
        break;
    case 's':
        objectPositionX = 0.0f;
        objectPositionY = 0.0f;
        xRotationAngle = 0.0f;
        yRotationAngle = 0.0f;
        rotatePositiveX = false;
        rotatePositiveY = false;
        rotateNegativeX = false;
        rotateNegativeY = false;
        break;
    }
    glutPostRedisplay();
}

GLvoid SpecialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        objectPositionY += 0.1f;
        break;
    case GLUT_KEY_DOWN:
        objectPositionY -= 0.1f;
        break;
    case GLUT_KEY_LEFT:
        objectPositionX -= 0.1f;
        break;
    case GLUT_KEY_RIGHT:
        objectPositionX += 0.1f;
        break;
    }
    glutPostRedisplay();
}

GLvoid Timer(int value) {
    if (rotatePositiveX) {
        xRotationAngle += 1.0f;
    }
    if (rotateNegativeX) {
        xRotationAngle -= 1.0f;
    }
    if (xRotationAngle > 360.0f) xRotationAngle -= 360.0f;
    if (xRotationAngle < 360.0f) xRotationAngle += 360.0f;

    if (rotatePositiveY) {
        yRotationAngle += 1.0f;
    }
    if (rotateNegativeY) {
        yRotationAngle -= 1.0f;
    }
    if (yRotationAngle > 360.0f) yRotationAngle -= 360.0f;
    if (yRotationAngle < 360.0f) yRotationAngle += 360.0f;

    glutPostRedisplay();

    glutTimerFunc(16, Timer, 0);
}
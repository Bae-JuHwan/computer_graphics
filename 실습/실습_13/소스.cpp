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

GLuint vaoCube, vaoTetrahedron;
GLuint vboCube[2], vboTetrahedron[2];
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
GLuint axisVBO, axisVAO;
Model modelCube, modelTetrahedron;

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
void InitTetrahedron();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);

int window_Width = 800;
int window_Height = 600;
int selectedFace1 = 0;
int selectedFace2 = 0;

void InitAxis() {
    GLfloat axisVertices[] = {
        // X�� ��ǥ�� ���� (������)
        -1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // ��ǥ, ������
         1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // ��ǥ, ������

         // Y�� ��ǥ�� ���� (�ʷϻ�)
          0.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // ��ǥ, �ʷϻ�
          0.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f   // ��ǥ, �ʷϻ�
    };

    glGenVertexArrays(1, &axisVAO);
    glBindVertexArray(axisVAO);

    glGenBuffers(1, &axisVBO);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
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
    glBindVertexArray(axisVAO);  // ��ǥ �� VAO ���ε�

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

    std::vector<glm::vec3> face_colors = {
        glm::vec3(1.0f, 0.0f, 0.0f),    // ������
        glm::vec3(0.0f, 1.0f, 0.0f),    // �ʷϻ�
        glm::vec3(0.0f, 0.0f, 1.0f),    // �Ķ���
        glm::vec3(1.0f, 1.0f, 0.0f),    // �����
        glm::vec3(0.0f, 1.0f, 1.0f),    // û�ϻ�
        glm::vec3(1.0f, 0.0f, 1.0f)     // ��ȫ��
    };

    for (size_t i = 0; i < modelCube.faces.size(); ++i) {
        Vertex v1 = modelCube.vertices[modelCube.faces[i].v1];
        Vertex v2 = modelCube.vertices[modelCube.faces[i].v2];
        Vertex v3 = modelCube.vertices[modelCube.faces[i].v3];

        glm::vec3 face_color = face_colors[i / 2];

        v1.color = face_color;
        v2.color = face_color;
        v3.color = face_color;

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

void InitTetrahedron() {
    read_obj_file("tetrahedron.obj", modelTetrahedron); // tetrahedron.obj �б�

    glGenVertexArrays(1, &vaoTetrahedron);
    glBindVertexArray(vaoTetrahedron);

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> face_colors = {
        glm::vec3(1.0f, 0.0f, 0.0f),    // ������
        glm::vec3(0.0f, 1.0f, 0.0f),    // �ʷϻ�
        glm::vec3(0.0f, 0.0f, 1.0f),    // �Ķ���
        glm::vec3(1.0f, 1.0f, 0.0f)     // �����
    };

    for (size_t i = 0; i < modelTetrahedron.faces.size(); ++i) {
        glm::vec3 face_color = face_colors[i];

        Vertex v1 = modelTetrahedron.vertices[modelTetrahedron.faces[i].v1];
        Vertex v2 = modelTetrahedron.vertices[modelTetrahedron.faces[i].v2];
        Vertex v3 = modelTetrahedron.vertices[modelTetrahedron.faces[i].v3];

        v1.color = face_color;
        v2.color = face_color;
        v3.color = face_color;

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    glGenBuffers(2, vboTetrahedron);
    glBindBuffer(GL_ARRAY_BUFFER, vboTetrahedron[0]);
    glBufferData(GL_ARRAY_BUFFER, expandedVertices.size() * sizeof(Vertex), expandedVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboTetrahedron[1]);
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
    InitTetrahedron();
    InitAxis();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
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

    // ������ü ȸ�� ��� ����
    glm::mat4 cubeMatrix = glm::mat4(1.0f);
    cubeMatrix = glm::rotate(cubeMatrix, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // Y�� ȸ��
    cubeMatrix = glm::rotate(cubeMatrix, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));  // X�� ȸ��

    // ������ü�� ������ �� ��ȯ ��� ����
    unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(cubeMatrix));

    // ������ü VAO �ٽ� ���ε�
    glBindVertexArray(vaoCube);  // ������ü VAO �ٽ� ���ε�

    if (selectedFace1 == 0 && selectedFace2 == 0) {
        // ��ü ������ü�� �׸���
        glDrawElements(GL_TRIANGLES, modelCube.faces.size() * 3, GL_UNSIGNED_INT, 0);
    }
    else {
        // ���õ� �鸸 �׸���
        if (selectedFace1 != 0) {
            switch (selectedFace1) {
            case 1: glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(GLuint))); break;  // �ո�
            case 2: glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint))); break;  // �޸�
            case 3: glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(GLuint))); break; // ������
            case 4: glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(GLuint))); break; // ����
            case 5: glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(24 * sizeof(GLuint))); break; // ����
            case 6: glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(30 * sizeof(GLuint))); break; // �ظ�
            }
        }
        if (selectedFace2 != 0) {
            switch (selectedFace2) {
            case 1: glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(GLuint))); break;  // �ո�
            case 2: glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint))); break;  // �޸�
            case 3: glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(GLuint))); break; // ������
            case 4: glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(GLuint))); break; // ����
            case 5: glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(24 * sizeof(GLuint))); break; // ����
            case 6: glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(30 * sizeof(GLuint))); break; // �ظ�
            }
        }
    }
    // VAO ����ε�
    glBindVertexArray(0);

    glm::mat4 tetraMatrix = glm::mat4(1.0f);
    tetraMatrix = glm::rotate(tetraMatrix, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // Y�� ȸ��
    tetraMatrix = glm::rotate(tetraMatrix, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));  // X�� ȸ��
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(tetraMatrix));

    glBindVertexArray(vaoTetrahedron);

    if (selectedFace1 == 0 && selectedFace2 == 0) {
        // ��ü �����ü �׸���
        glDrawElements(GL_TRIANGLES, modelTetrahedron.faces.size() * 3, GL_UNSIGNED_INT, 0);
    }
    else if (selectedFace1 >= 7 && selectedFace1 <= 10) {
        // �����ü�� ���õ� �� �׸���
        switch (selectedFace1) {
        case 7: glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(0 * sizeof(GLuint))); break;  // ù ��° ��
        case 8: glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(3 * sizeof(GLuint))); break;  // �� ��° ��
        case 9: glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint))); break;  // �� ��° ��
        case 10: glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(9 * sizeof(GLuint))); break; // �� ��° ��
        }
        // �� ��° ���� ���õ� ��� �׸���
        if (selectedFace2 != 0) {
            switch (selectedFace2) {
            case 7: glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(0 * sizeof(GLuint))); break;  // ù ��° ��
            case 8: glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(3 * sizeof(GLuint))); break;  // �� ��° ��
            case 9: glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint))); break;  // �� ��° ��
            case 10: glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(9 * sizeof(GLuint))); break; // �� ��° ��
            }
        }
    }
    glBindVertexArray(0);

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
    case 'a':
        selectedFace1 = 0;
        selectedFace2 = 0;
        break;
    case '1':
        selectedFace1 = 1;
        selectedFace2 = 0;
        break;
    case '2':
        selectedFace1 = 2;
        selectedFace2 = 0;
        break;
    case '3':
        selectedFace1 = 3;
        selectedFace2 = 0;
        break;
    case '4':
        selectedFace1 = 4;
        selectedFace2 = 0;
        break;
    case '5':
        selectedFace1 = 5;
        selectedFace2 = 0;
        break;
    case '6':
        selectedFace1 = 6;
        selectedFace2 = 0;
        break;
    case '7':
        selectedFace1 = 7;
        selectedFace2 = 0;
        break;
    case '8':
        selectedFace1 = 8;
        selectedFace2 = 0;
        break;
    case '9':
        selectedFace1 = 9;
        selectedFace2 = 0;
        break;
    case '0':
        selectedFace1 = 10;
        selectedFace2 = 0;
        break;
    case 'c':
        srand(static_cast<unsigned int>(time(0)));
        selectedFace1 = rand() % 6 + 1;
        do {
            selectedFace2 = rand() % 6 + 1;
        } while (selectedFace1 == selectedFace2);
        break;
    case 't':
        srand(static_cast<unsigned int>(time(0)));
        selectedFace1 = rand() % 4 + 7;
        do {
            selectedFace2 = rand() % 4 + 7;
        } while (selectedFace1 == selectedFace2);
        break;
    }
    glutPostRedisplay();
}
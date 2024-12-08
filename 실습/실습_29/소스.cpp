#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
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
#include "stb_image.h"

GLuint vaoAxis, vaoCube, vaoPyramid;
GLuint vboAxis, vboCube[2], vboPyramid[2];
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
GLuint vao[2], vbo[2];
GLuint textures[6];
const char* textureFiles[6] = {
    "textureA.png", "textureB.png", "textureC.png",
    "textureD.png", "textureE.png", "textureF.png"
};
Model modelCube, modelPyramid;
GLfloat yRotationAngle = 0.0f;
GLfloat xRotationAngle = 0.0f;

bool drawCube = false;
bool drawPyramid = false;
bool rotatePositiveY = false;
bool rotateNegativeY = false;
bool rotatePositiveX = false;
bool rotateNegativeX = false;

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
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid SpecialKeys(int key, int x, int y);
GLvoid Timer(int value);

int window_Width = 800;
int window_Height = 600;

float vertexData[] = {
    // Front face
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,

    // Left face
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,

    // Right face
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  1.0f, 0.0f,

     // Top face
     -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  0.0f, 1.0f,
      0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
     -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f, 0.0f,
     -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  0.0f, 1.0f,

     // Bottom face
     -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  1.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f, 1.0f,
      0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f,
      0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f,
     -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  1.0f, 0.0f,
     -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  1.0f, 1.0f
};

float vertexData2[] = {
    // Side 1 (�ո�)
    -0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, // Bottom-left
    0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, // Bottom-right
    0.0f, 0.5f,  0.0f,   1.0f, 0.0f, 0.0f,   0.5f, 1.0f, // Apex

    // Side 2 (������ ��)
    0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Bottom-left
    0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom-right
    0.0f, 0.5f,  0.0f,   0.0f, 1.0f, 0.0f,   0.5f, 1.0f, // Apex

    // Side 3 (�޸�)
    0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom-left
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f, // Bottom-right
    0.0f, 0.5f,  0.0f,   0.0f, 0.0f, 1.0f,   0.5f, 1.0f, // Apex

    // Side 4 (���� ��)
    -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Bottom-left
    -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom-right
    0.0f, 0.5f,  0.0f,   1.0f, 1.0f, 0.0f,   0.5f, 1.0f,  // Apex

    // Base (�簢�� �ٴ�)
    -0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f, // Bottom-left
    0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f, // Bottom-right
    0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Top-right
    0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Top-right
    -0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f, // Top-left
    -0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f // Bottom-left
};

void InitBuffer() {
    glGenVertexArrays(2, vao);
    glGenBuffers(2, vbo);

    // ù ��° ���ؽ� ������
    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // �� ��° ���ؽ� ������
    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData2), vertexData2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void InitTexture() {
    glGenTextures(6, textures);

    for (int i = 0; i < 6; ++i) {
        glBindTexture(GL_TEXTURE_2D, textures[i]);

        // �ؽ�ó �Ķ���� ����
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // �ؽ�ó ������ �ε�
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(textureFiles[i], &width, &height, &channels, 0);
        if (data) {
            GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cerr << "Failed to load texture: " << textureFiles[i] << std::endl;
        }
        stbi_image_free(data);
    }
}

//void InitCube() {
//    read_obj_file("cube.obj", modelCube); // OBJ ���� �б�
//
//    std::vector<Vertex> expandedVertices;
//    std::vector<unsigned int> indices;
//
//    for (size_t i = 0; i < modelCube.faces.size(); ++i) {
//        Vertex v1 = modelCube.vertices[modelCube.faces[i].v1];
//        Vertex v2 = modelCube.vertices[modelCube.faces[i].v2];
//        Vertex v3 = modelCube.vertices[modelCube.faces[i].v3];
//
//        int faceIndex = i / 2; // �� ��ȣ (0 ~ 5)
//
//        // �ؽ�ó ��ǥ ����
//        if (i % 2 == 0) { // ù ��° �ﰢ��
//            v1.texCoord = glm::vec2(0.0f, 0.0f);
//            v2.texCoord = glm::vec2(1.0f, 0.0f);
//            v3.texCoord = glm::vec2(1.0f, 1.0f);
//        }
//        else { // �� ��° �ﰢ��
//            v1.texCoord = glm::vec2(0.0f, 0.0f);
//            v2.texCoord = glm::vec2(1.0f, 1.0f);
//            v3.texCoord = glm::vec2(0.0f, 1.0f);
//        }
//
//        expandedVertices.push_back(v1);
//        expandedVertices.push_back(v2);
//        expandedVertices.push_back(v3);
//
//        indices.push_back(expandedVertices.size() - 3);
//        indices.push_back(expandedVertices.size() - 2);
//        indices.push_back(expandedVertices.size() - 1);
//    }
//
//    InitBuffer(vaoCube, vboCube, expandedVertices, indices); // InitBuffer ȣ��
//}

//void InitPyramid() {
//    read_obj_file("pyramid.obj", modelPyramid); // pyramid.obj �б�
//
//    std::vector<Vertex> expandedVertices;
//    std::vector<unsigned int> indices;
//
//    for (size_t i = 0; i < modelPyramid.faces.size(); ++i) {
//        Vertex v1 = modelPyramid.vertices[modelPyramid.faces[i].v1];
//        Vertex v2 = modelPyramid.vertices[modelPyramid.faces[i].v2];
//        Vertex v3 = modelPyramid.vertices[modelPyramid.faces[i].v3];
//
//        if (i == 0) { // ù ��° �鿡�� �ؽ�ó ����
//            v1.texCoord = glm::vec2(0.0f, 0.0f);
//            v2.texCoord = glm::vec2(1.0f, 0.0f);
//            v3.texCoord = glm::vec2(0.5f, 1.0f);
//        }
//        else if (i == 1) { // ù ��° �鿡�� �ؽ�ó ����
//            v1.texCoord = glm::vec2(0.0f, 0.0f);
//            v2.texCoord = glm::vec2(1.0f, 0.0f);
//            v3.texCoord = glm::vec2(0.5f, 1.0f);
//        }
//        else if (i == 2) { // ù ��° �鿡�� �ؽ�ó ����
//            v1.texCoord = glm::vec2(0.0f, 0.0f);
//            v2.texCoord = glm::vec2(1.0f, 0.0f);
//            v3.texCoord = glm::vec2(0.5f, 1.0f);
//        }
//        else if (i == 3) { // ù ��° �鿡�� �ؽ�ó ����
//            v1.texCoord = glm::vec2(0.0f, 0.0f);
//            v2.texCoord = glm::vec2(1.0f, 0.0f);
//            v3.texCoord = glm::vec2(0.5f, 1.0f);
//        }
//        else if (i == 4 || i == 5) {
//            if (i == 4) {
//                v1.texCoord = glm::vec2(0.0f, 0.0f);  // ù ��° �ﰢ��
//                v2.texCoord = glm::vec2(1.0f, 0.0f);
//                v3.texCoord = glm::vec2(1.0f, 1.0f);
//            }
//            else if (i == 5) {
//                v1.texCoord = glm::vec2(0.0f, 0.0f);  // �� ��° �ﰢ��
//                v2.texCoord = glm::vec2(1.0f, 1.0f);
//                v3.texCoord = glm::vec2(0.0f, 1.0f);
//            }
//        }
//
//        // �ؽ�ó ��ǥ�� OBJ ���Ͽ��� ���� �� ���
//        expandedVertices.push_back(v1);
//        expandedVertices.push_back(v2);
//        expandedVertices.push_back(v3);
//
//        indices.push_back(expandedVertices.size() - 3);
//        indices.push_back(expandedVertices.size() - 2);
//        indices.push_back(expandedVertices.size() - 1);
//    }
//
//    InitBuffer(vaoPyramid, vboPyramid, expandedVertices, indices); // InitBuffer ȣ��
//}

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(window_Width, window_Height);
    glutCreateWindow("�ǽ�29");

    glewExperimental = GL_TRUE;
    glewInit();

    make_shaderProgram();
    InitTexture();
    InitBuffer();

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
    glClearColor(1.0, 1.0, 1.0, 1.0f);           // ��� ���� ����
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ȭ�� �ʱ�ȭ
    glUseProgram(shaderProgramID);              // ���̴� ���α׷� Ȱ��ȭ

    // ��� ���� ��� ����
    glm::mat4 viewMatrix = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 5.0f),  // ī�޶� ��ġ
        glm::vec3(0.0f, 0.0f, 0.0f),   // ī�޶� �ٶ󺸴� ����
        glm::vec3(0.0f, 1.0f, 0.0f)    // ���� ����
    );
    glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(45.0f),
        (float)window_Width / (float)window_Height,
        0.1f,
        100.0f
    );

    GLint viewMatrixLocation = glGetUniformLocation(shaderProgramID, "view");
    GLint projMatrixLocation = glGetUniformLocation(shaderProgramID, "projection");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(projMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // �� ���
    glm::mat4 baseModelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    baseModelMatrix = glm::rotate(baseModelMatrix, glm::radians(yRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    baseModelMatrix = glm::rotate(baseModelMatrix, glm::radians(xRotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    GLint modelLocation = glGetUniformLocation(shaderProgramID, "model");

    if (drawCube) {
        glBindVertexArray(vao[0]); // VAO ���ε� (������ü ������)

        // �� �� ������
        for (int i = 0; i < 6; ++i) {
            // �ؽ�ó Ȱ��ȭ �� ���ε�
            glActiveTexture(GL_TEXTURE0);                              // �ؽ�ó ���� Ȱ��ȭ
            glBindTexture(GL_TEXTURE_2D, textures[i]);                 // �ؽ�ó ���ε�
            glUniform1i(glGetUniformLocation(shaderProgramID, "outTexture"), 0); // ���̴��� �ؽ�ó ����

            // �� ��� ����
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(baseModelMatrix));

            // ���� ������ ������
            glDrawArrays(GL_TRIANGLES, i * 6, 6); // �� �鿡 ���� 6���� ���� ������
        }
        glBindVertexArray(0); // VAO ���ε� ����
    }

    if (drawPyramid) {
        glBindVertexArray(vao[1]); // �Ƕ�̵� VAO ���ε�

        // ���� ������ (�ﰢ�� 4��)
        for (int i = 0; i < 4; ++i) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures[i]); // ���� �ؽ�ó (0~3�� �ؽ�ó)
            glUniform1i(glGetUniformLocation(shaderProgramID, "outTexture"), 0);

            // �� ��� ����
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(baseModelMatrix));

            // �� ������ ������
            glDrawArrays(GL_TRIANGLES, i * 3, 3); // �� �鿡 ���� 3���� ���� ������
        }

        // �ظ� ������ (�ﰢ�� 2��)
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[4]); // �ظ� �ؽ�ó (4�� �ؽ�ó)
        glUniform1i(glGetUniformLocation(shaderProgramID, "outTexture"), 0);

        // �ظ� ù ��° �ﰢ��
        glDrawArrays(GL_TRIANGLES, 12, 3);

        // �ظ� �� ��° �ﰢ��
        glDrawArrays(GL_TRIANGLES, 15, 3);

        glBindVertexArray(0); // VAO ���ε� ����
    }

    glutSwapBuffers();    // ���� ���۸����� ȭ�� ����
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
        drawPyramid = false;
        break;
    case 'p':
        drawCube = false;
        drawPyramid = true;
        break;
    case 'y':
        rotatePositiveY = !rotatePositiveY;
        break;
    case 'Y':
        rotateNegativeY = !rotateNegativeY;
        break;
    case 'x':
        rotatePositiveX = !rotatePositiveX;
        break;
    case 'X':
        rotateNegativeX = !rotateNegativeX;
        break;
    case 's':
        yRotationAngle = 0.0f;
        xRotationAngle = 0.0f;
        rotatePositiveY = false;
        rotateNegativeY = false;
        rotatePositiveX = false;
        rotateNegativeX = false;
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

    if (yRotationAngle > 360.0f) yRotationAngle -= 360.0f;
    if (yRotationAngle < 360.0f) yRotationAngle += 360.0f;

    if (rotatePositiveX) {
        xRotationAngle += 1.0f;
    }
    if (rotateNegativeX) {
        xRotationAngle -= 1.0f;
    }

    if (xRotationAngle > 360.0f) xRotationAngle -= 360.0f;
    if (xRotationAngle < 360.0f) xRotationAngle += 360.0f;

    glutPostRedisplay();
    glutTimerFunc(16, Timer, 0);
}
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
#include "���.h"

struct AABB {
    glm::vec3 min; // �浹�ڽ��� �ּ� ��ǥ (x, y, z)
    glm::vec3 max; // �浹�ڽ��� �ִ� ��ǥ (x, y, z)
};

AABB block1Box, block2Box, block3Box, robotBox, cubeFaceBoxes[6];

GLuint vaoCube, vaoBody, vaoLeftEye, vaoRightEye, vaoLeftArm, vaoRightArm, vaoLeftLeg, vaoRightLeg, vaoBlock;
GLuint vboCube[2], vboBody[2], vboLeftEye[2], vboRightEye[2], vboLeftArm[2], vboRightArm[2], vboLeftLeg[2], vboRightLeg[2], vboBlock[2];
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
Model modelCube, modelBody, modelLeftEye, modelRightEye, modelLeftArm, modelRightArm, modelLeftLeg, modelRightLeg, modelBlock;

GLfloat cameraX = 0.0f;
GLfloat cameraY = 0.0f;
GLfloat cameraZ = 30.0f;
GLfloat cubeFrontX = 0.0f;
GLfloat moveSpeed = 0.05f;
GLfloat robotRotationAngle = 0.0f;
GLfloat armSwingAngle = 0.0f;
GLfloat legSwingAngle = 0.0f;
GLfloat maxSwingAngle = 30.0f;
GLfloat jumpVelocity = 0.2f;
GLfloat gravity = -0.01f;
GLfloat maxJumpHeight = 2.0f;
GLfloat initialRotationAngle = 0.0f;

glm::mat4 robotModelMatrix = glm::mat4(1.0f);
glm::vec3 robotDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 robotPosition = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 initialPosition = glm::vec3(0.0f, 0.0f, 0.0f);

bool isCameraXmove = false;
bool isCameraXmoveReverse = false;
bool isCameraYmove = false;
bool isCameraYmoveReverse = false;
bool isCameraZmove = false;
bool isCameraZmoveReverse = false;
bool isCubeFrontXmove = false;
bool isCubeFrontXmoveReverse = false;
bool isSwingingForward = true;
bool isJumping = false;
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
void InitCube();
void InitBody();
void InitLeftEye();
void InitRightEye();
void InitLeftArm();
void InitRightArm();
void InitLeftLeg();
void InitRightLeg();
void InitBlock();
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

    // �������� (��ġ�� ����)
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // ���� ��ҵ� (�ε���)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // ������ ����ϰ� ��
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);  // Position attribute
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));  // Color attribute
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);  // VAO ���ε� ����
}

void InitCube() {
    read_obj_file("cube.obj", modelCube);  // OBJ ���� �б�

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
        Vertex v1 = { modelCube.vertices[modelCube.faces[i].v1].x,
                     modelCube.vertices[modelCube.faces[i].v1].y,
                     modelCube.vertices[modelCube.faces[i].v1].z,
                     face_colors[i / 2] };

        Vertex v2 = { modelCube.vertices[modelCube.faces[i].v2].x,
                     modelCube.vertices[modelCube.faces[i].v2].y,
                     modelCube.vertices[modelCube.faces[i].v2].z,
                     face_colors[i / 2] };

        Vertex v3 = { modelCube.vertices[modelCube.faces[i].v3].x,
                     modelCube.vertices[modelCube.faces[i].v3].y,
                     modelCube.vertices[modelCube.faces[i].v3].z,
                     face_colors[i / 2] };

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    // ������ü�� 6�鿡 ���� �浹�ڽ� ����
    cubeFaceBoxes[0] = { glm::vec3(-10.0f, -10.0f, 10.0f), glm::vec3(10.0f, 10.0f, 10.1f) }; // �ո�
    cubeFaceBoxes[1] = { glm::vec3(-10.0f, -10.0f, -10.1f), glm::vec3(10.0f, 10.0f, -10.0f) }; // �޸�
    cubeFaceBoxes[2] = { glm::vec3(-10.0f, 10.0f, -10.0f), glm::vec3(10.0f, 10.1f, 10.0f) }; // ����
    cubeFaceBoxes[3] = { glm::vec3(-10.0f, -10.1f, -10.0f), glm::vec3(10.0f, -10.0f, 10.0f) }; // �Ʒ���
    cubeFaceBoxes[4] = { glm::vec3(-10.1f, -10.0f, -10.0f), glm::vec3(-10.0f, 10.0f, 10.0f) }; // ���ʸ�
    cubeFaceBoxes[5] = { glm::vec3(10.0f, -10.0f, -10.0f), glm::vec3(10.1f, 10.0f, 10.0f) }; // �����ʸ�

    InitBuffer(vaoCube, vboCube, expandedVertices, indices);  // InitBuffer ȣ��
}

void InitBody() {
    read_obj_file("body.obj", modelBody);

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    glm::vec3 blueColor = glm::vec3(0.0f, 0.0f, 1.0f); // �Ķ���

    for (const auto& face : modelBody.faces) {
        if (face.v1 >= modelBody.vertices.size() ||
            face.v2 >= modelBody.vertices.size() ||
            face.v3 >= modelBody.vertices.size()) {
            std::cerr << "Error: Face index out of range. Face: ("
                << face.v1 << ", " << face.v2 << ", " << face.v3
                << "), Vertices Size: " << modelBody.vertices.size() << std::endl;
            continue;
        }

        // ���� ����
        Vertex v1 = { modelBody.vertices[face.v1].x, modelBody.vertices[face.v1].y, modelBody.vertices[face.v1].z, blueColor };
        Vertex v2 = { modelBody.vertices[face.v2].x, modelBody.vertices[face.v2].y, modelBody.vertices[face.v2].z, blueColor };
        Vertex v3 = { modelBody.vertices[face.v3].x, modelBody.vertices[face.v3].y, modelBody.vertices[face.v3].z, blueColor };

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    if (expandedVertices.empty() || indices.empty()) {
        std::cerr << "Error: No valid vertices or indices generated for body." << std::endl;
        return;
    }

    InitBuffer(vaoBody, vboBody, expandedVertices, indices);
}

void InitLeftEye() {
    read_obj_file("leftEye.obj", modelLeftEye);  // OBJ ���� �б�

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    for (size_t i = 0; i < modelLeftEye.faces.size(); ++i) {
        glm::vec3 black_color = glm::vec3(0.0f, 0.0f, 0.0f);

        Vertex v1 = { modelLeftEye.vertices[modelLeftEye.faces[i].v1].x,
                     modelLeftEye.vertices[modelLeftEye.faces[i].v1].y,
                     modelLeftEye.vertices[modelLeftEye.faces[i].v1].z,
                     black_color };

        Vertex v2 = { modelLeftEye.vertices[modelLeftEye.faces[i].v2].x,
                     modelLeftEye.vertices[modelLeftEye.faces[i].v2].y,
                     modelLeftEye.vertices[modelLeftEye.faces[i].v2].z,
                     black_color };

        Vertex v3 = { modelLeftEye.vertices[modelLeftEye.faces[i].v3].x,
                     modelLeftEye.vertices[modelLeftEye.faces[i].v3].y,
                     modelLeftEye.vertices[modelLeftEye.faces[i].v3].z,
                     black_color };

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoLeftEye, vboLeftEye, expandedVertices, indices);  // InitBuffer ȣ��
}

void InitRightEye() {
    read_obj_file("rightEye.obj", modelRightEye);  // OBJ ���� �б�

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    for (size_t i = 0; i < modelRightEye.faces.size(); ++i) {
        glm::vec3 black_color = glm::vec3(0.0f, 0.0f, 0.0f);

        Vertex v1 = { modelRightEye.vertices[modelRightEye.faces[i].v1].x,
                     modelRightEye.vertices[modelRightEye.faces[i].v1].y,
                     modelRightEye.vertices[modelRightEye.faces[i].v1].z,
                     black_color };

        Vertex v2 = { modelRightEye.vertices[modelRightEye.faces[i].v2].x,
                     modelRightEye.vertices[modelRightEye.faces[i].v2].y,
                     modelRightEye.vertices[modelRightEye.faces[i].v2].z,
                     black_color };

        Vertex v3 = { modelRightEye.vertices[modelRightEye.faces[i].v3].x,
                     modelRightEye.vertices[modelRightEye.faces[i].v3].y,
                     modelRightEye.vertices[modelRightEye.faces[i].v3].z,
                     black_color };

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoRightEye, vboRightEye, expandedVertices, indices);  // InitBuffer ȣ��
}

void InitLeftArm() {
    read_obj_file("leftArm.obj", modelLeftArm);  // OBJ ���� �б�

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    for (size_t i = 0; i < modelLeftArm.faces.size(); ++i) {
        glm::vec3 blueGreen_color = glm::vec3(0.0f, 1.0f, 1.0f);

        Vertex v1 = { modelLeftArm.vertices[modelLeftArm.faces[i].v1].x,
                     modelLeftArm.vertices[modelLeftArm.faces[i].v1].y,
                     modelLeftArm.vertices[modelLeftArm.faces[i].v1].z,
                     blueGreen_color };

        Vertex v2 = { modelLeftArm.vertices[modelLeftArm.faces[i].v2].x,
                     modelLeftArm.vertices[modelLeftArm.faces[i].v2].y,
                     modelLeftArm.vertices[modelLeftArm.faces[i].v2].z,
                     blueGreen_color };

        Vertex v3 = { modelLeftArm.vertices[modelLeftArm.faces[i].v3].x,
                     modelLeftArm.vertices[modelLeftArm.faces[i].v3].y,
                     modelLeftArm.vertices[modelLeftArm.faces[i].v3].z,
                     blueGreen_color };

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoLeftArm, vboLeftArm, expandedVertices, indices);  // InitBuffer ȣ��
}

void InitRightArm() {
    read_obj_file("rightArm.obj", modelRightArm);  // OBJ ���� �б�

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    for (size_t i = 0; i < modelRightArm.faces.size(); ++i) {
        glm::vec3 redBlue_color = glm::vec3(1.0f, 0.0f, 1.0f);

        Vertex v1 = { modelRightArm.vertices[modelRightArm.faces[i].v1].x,
                     modelRightArm.vertices[modelRightArm.faces[i].v1].y,
                     modelRightArm.vertices[modelRightArm.faces[i].v1].z,
                     redBlue_color };

        Vertex v2 = { modelRightArm.vertices[modelRightArm.faces[i].v2].x,
                     modelRightArm.vertices[modelRightArm.faces[i].v2].y,
                     modelRightArm.vertices[modelRightArm.faces[i].v2].z,
                     redBlue_color };

        Vertex v3 = { modelRightArm.vertices[modelRightArm.faces[i].v3].x,
                     modelRightArm.vertices[modelRightArm.faces[i].v3].y,
                     modelRightArm.vertices[modelRightArm.faces[i].v3].z,
                     redBlue_color };

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoRightArm, vboRightArm, expandedVertices, indices);  // InitBuffer ȣ��
}

void InitLeftLeg() {
    read_obj_file("leftLeg.obj", modelLeftLeg);  // OBJ ���� �б�

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    for (size_t i = 0; i < modelLeftLeg.faces.size(); ++i) {
        glm::vec3 yellow_color = glm::vec3(1.0f, 1.0f, 0.0f);

        Vertex v1 = { modelLeftLeg.vertices[modelLeftLeg.faces[i].v1].x,
                     modelLeftLeg.vertices[modelLeftLeg.faces[i].v1].y,
                     modelLeftLeg.vertices[modelLeftLeg.faces[i].v1].z,
                     yellow_color };

        Vertex v2 = { modelLeftLeg.vertices[modelLeftLeg.faces[i].v2].x,
                     modelLeftLeg.vertices[modelLeftLeg.faces[i].v2].y,
                     modelLeftLeg.vertices[modelLeftLeg.faces[i].v2].z,
                     yellow_color };

        Vertex v3 = { modelLeftLeg.vertices[modelLeftLeg.faces[i].v3].x,
                     modelLeftLeg.vertices[modelLeftLeg.faces[i].v3].y,
                     modelLeftLeg.vertices[modelLeftLeg.faces[i].v3].z,
                     yellow_color };

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoLeftLeg, vboLeftLeg, expandedVertices, indices);  // InitBuffer ȣ��
}

void InitRightLeg() {
    read_obj_file("rightLeg.obj", modelRightLeg);  // OBJ ���� �б�

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    for (size_t i = 0; i < modelRightLeg.faces.size(); ++i) {
        glm::vec3 yellow_color = glm::vec3(1.0f, 1.0f, 0.0f);

        Vertex v1 = { modelRightLeg.vertices[modelRightLeg.faces[i].v1].x,
                     modelRightLeg.vertices[modelRightLeg.faces[i].v1].y,
                     modelRightLeg.vertices[modelRightLeg.faces[i].v1].z,
                     yellow_color };

        Vertex v2 = { modelRightLeg.vertices[modelRightLeg.faces[i].v2].x,
                     modelRightLeg.vertices[modelRightLeg.faces[i].v2].y,
                     modelRightLeg.vertices[modelRightLeg.faces[i].v2].z,
                     yellow_color };

        Vertex v3 = { modelRightLeg.vertices[modelRightLeg.faces[i].v3].x,
                     modelRightLeg.vertices[modelRightLeg.faces[i].v3].y,
                     modelRightLeg.vertices[modelRightLeg.faces[i].v3].z,
                     yellow_color };

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    InitBuffer(vaoRightLeg, vboRightLeg, expandedVertices, indices);  // InitBuffer ȣ��
}

void InitBlock() {
    read_obj_file("block.obj", modelBlock);  // OBJ ���� �б�

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> face_colors = {
        glm::vec3(0.5f, 0.5, 0.5f),
        glm::vec3(0.5f, 0.5, 0.5f),
        glm::vec3(0.5f, 0.5, 0.5f),
        glm::vec3(0.5f, 0.5, 0.5f),
        glm::vec3(0.5f, 0.5, 0.5f),
        glm::vec3(0.5f, 0.5, 0.5f)
    };

    for (size_t i = 0; i < modelBlock.faces.size(); ++i) {
        Vertex v1 = { modelBlock.vertices[modelBlock.faces[i].v1].x,
                     modelBlock.vertices[modelBlock.faces[i].v1].y,
                     modelBlock.vertices[modelBlock.faces[i].v1].z,
                     face_colors[i / 2] };

        Vertex v2 = { modelBlock.vertices[modelBlock.faces[i].v2].x,
                     modelBlock.vertices[modelBlock.faces[i].v2].y,
                     modelBlock.vertices[modelBlock.faces[i].v2].z,
                     face_colors[i / 2] };

        Vertex v3 = { modelBlock.vertices[modelBlock.faces[i].v3].x,
                     modelBlock.vertices[modelBlock.faces[i].v3].y,
                     modelBlock.vertices[modelBlock.faces[i].v3].z,
                     face_colors[i / 2] };

        expandedVertices.push_back(v1);
        expandedVertices.push_back(v2);
        expandedVertices.push_back(v3);

        indices.push_back(expandedVertices.size() - 3);
        indices.push_back(expandedVertices.size() - 2);
        indices.push_back(expandedVertices.size() - 1);
    }

    // Block 1
    block1Box = { glm::vec3(-2.5f, -10.2f, 4.5f), glm::vec3(-1.5f, -9.2f, 5.5f) };

    // Block 2
    block2Box = { glm::vec3(5.5f, -10.2f, -6.5f), glm::vec3(6.5f, -9.2f, -5.5f) };

    // Block 3
    block3Box = { glm::vec3(7.5f, -10.2f, 6.5f), glm::vec3(8.5f, -9.2f, 7.5f) };

    InitBuffer(vaoBlock, vboBlock, expandedVertices, indices);  // InitBuffer ȣ��
}

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(window_Width, window_Height);
    glutCreateWindow("�ǽ�21");

    glewExperimental = GL_TRUE;
    glewInit();

    make_shaderProgram();
    InitCube();
    InitBody();
    InitLeftEye();
    InitRightEye();
    InitLeftArm();
    InitRightArm();
    InitLeftLeg();
    InitRightLeg();
    InitBlock();

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

    // �� ��ȯ
    glm::mat4 viewMatrix = glm::lookAt(
        glm::vec3(cameraX, cameraY, cameraZ),  // ī�޶� ��ġ
        glm::vec3(0.0f, 0.0f, 0.0f),   // ī�޶� �ٶ󺸴� ����
        glm::vec3(0.0f, 1.0f, 0.0f)  // ���� ����
    );

    // �� ����� ���̴��� ����
    GLint viewMatrixLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // ���� ��ȯ
    glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(45.0f),
        (float)window_Width / (float)window_Height,
        0.1f,
        100.0f
    );

    GLint projMatrixLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
    glUniformMatrix4fv(projMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));


    GLint modelMatrixLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
    // ������ü ������
    glm::mat4 cubeModelMatrix = glm::mat4(1.0f);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(cubeModelMatrix));
    glBindVertexArray(vaoCube);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(GLuint)));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(GLuint)));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(GLuint)));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(24 * sizeof(GLuint)));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(30 * sizeof(GLuint)));
    glm::mat4 cubeFrontMatrix = cubeModelMatrix;
    cubeFrontMatrix = glm::translate(cubeFrontMatrix, glm::vec3(cubeFrontX, 0.0f, 0.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(cubeFrontMatrix));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint)));
    glBindVertexArray(0);

    // ����
    glm::mat4 bodyModelMatrix = robotModelMatrix;
    bodyModelMatrix = glm::translate(bodyModelMatrix, glm::vec3(0.0f, -6.5f, 0.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(bodyModelMatrix));
    glBindVertexArray(vaoBody);
    glDrawElements(GL_TRIANGLES, modelBody.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // ���� ��
    glm::mat4 leftEyeModelMatrix = robotModelMatrix;
    leftEyeModelMatrix = glm::translate(leftEyeModelMatrix, glm::vec3(0.3f, -4.3f, 0.8f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(leftEyeModelMatrix));
    glBindVertexArray(vaoLeftEye);
    glDrawElements(GL_TRIANGLES, modelLeftEye.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // ������ ��
    glm::mat4 rightEyeModelMatrix = robotModelMatrix;
    rightEyeModelMatrix = glm::translate(rightEyeModelMatrix, glm::vec3(-0.3f, -4.3f, 0.8f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(rightEyeModelMatrix));
    glBindVertexArray(vaoRightEye);
    glDrawElements(GL_TRIANGLES, modelRightEye.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // ���� ��
    glm::mat4 leftArmModelMatrix = robotModelMatrix;
    leftArmModelMatrix = glm::translate(leftArmModelMatrix, glm::vec3(1.3f, -6.2f, 0.0f));
    leftArmModelMatrix = glm::translate(leftArmModelMatrix, glm::vec3(0.0f, 1.0f, 0.0f));
    leftArmModelMatrix = glm::rotate(leftArmModelMatrix, glm::radians(armSwingAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    leftArmModelMatrix = glm::translate(leftArmModelMatrix, glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(leftArmModelMatrix));
    glBindVertexArray(vaoLeftArm);
    glDrawElements(GL_TRIANGLES, modelLeftArm.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // ������ ��
    glm::mat4 rightArmModelMatrix = robotModelMatrix;
    rightArmModelMatrix = glm::translate(rightArmModelMatrix, glm::vec3(-1.3f, -6.2f, 0.0f));
    rightArmModelMatrix = glm::translate(rightArmModelMatrix, glm::vec3(0.0f, 1.0f, 0.0f));
    rightArmModelMatrix = glm::rotate(rightArmModelMatrix, glm::radians(-armSwingAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    rightArmModelMatrix = glm::translate(rightArmModelMatrix, glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(rightArmModelMatrix));
    glBindVertexArray(vaoRightArm);
    glDrawElements(GL_TRIANGLES, modelRightArm.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // ���� �ٸ�
    glm::mat4 leftLegModelMatrix = robotModelMatrix;
    leftLegModelMatrix = glm::translate(leftLegModelMatrix, glm::vec3(0.4f, -9.0f, 0.0f));
    leftLegModelMatrix = glm::translate(leftLegModelMatrix, glm::vec3(0.0f, 1.0f, 0.0f));
    leftLegModelMatrix = glm::rotate(leftLegModelMatrix, glm::radians(legSwingAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    leftLegModelMatrix = glm::translate(leftLegModelMatrix, glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(leftLegModelMatrix));
    glBindVertexArray(vaoLeftLeg);
    glDrawElements(GL_TRIANGLES, modelLeftLeg.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // ������ �ٸ�
    glm::mat4 rightLegModelMatrix = robotModelMatrix;
    rightLegModelMatrix = glm::translate(rightLegModelMatrix, glm::vec3(-0.4f, -9.0f, 0.0f));
    rightLegModelMatrix = glm::translate(rightLegModelMatrix, glm::vec3(0.0f, 1.0f, 0.0f));
    rightLegModelMatrix = glm::rotate(rightLegModelMatrix, glm::radians(-legSwingAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    rightLegModelMatrix = glm::translate(rightLegModelMatrix, glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(rightLegModelMatrix));
    glBindVertexArray(vaoRightLeg);
    glDrawElements(GL_TRIANGLES, modelRightLeg.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // ��ֹ�1
    glm::mat4 blockModelMatrix1 = glm::mat4(1.0f);
    blockModelMatrix1 = glm::translate(blockModelMatrix1, glm::vec3(-2.0f, -9.7f, 5.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(blockModelMatrix1));
    glBindVertexArray(vaoBlock);
    glDrawElements(GL_TRIANGLES, modelBlock.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // ��ֹ�2
    glm::mat4 blockModelMatrix2 = glm::mat4(1.0f);
    blockModelMatrix2 = glm::translate(blockModelMatrix2, glm::vec3(6.0f, -9.7f, -6.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(blockModelMatrix2));
    glBindVertexArray(vaoBlock);
    glDrawElements(GL_TRIANGLES, modelBlock.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // ��ֹ�3
    glm::mat4 blockModelMatrix3 = glm::mat4(1.0f);
    blockModelMatrix3 = glm::translate(blockModelMatrix3, glm::vec3(8.0f, -9.7f, 7.0f));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(blockModelMatrix3));
    glBindVertexArray(vaoBlock);
    glDrawElements(GL_TRIANGLES, modelBlock.faces.size() * 3, GL_UNSIGNED_INT, 0);
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
    case 'o':
        isCubeFrontXmove = !isCubeFrontXmove;
        break;
    case 'O':
        isCubeFrontXmoveReverse = !isCubeFrontXmoveReverse;
        break;
    case 'w':
        robotDirection = glm::vec3(0.0f, 0.0f, -moveSpeed);
        robotRotationAngle = 180.0f;
        break;
    case 'a':
        robotDirection = glm::vec3(-moveSpeed, 0.0f, 0.0f);
        robotRotationAngle = -90.0f;
        break;
    case 's':
        robotDirection = glm::vec3(0.0f, 0.0f, moveSpeed);
        robotRotationAngle = 0.0f;
        break;
    case 'd':
        robotDirection = glm::vec3(moveSpeed, 0.0f, 0.0f);
        robotRotationAngle = 90.0f;
        break;
    case '+':
        maxSwingAngle += 5.0f;
        if (maxSwingAngle > 60.0f) {
            maxSwingAngle = 60.0f;
        }
        break;
    case '-':
        maxSwingAngle -= 5.0f;
        if (maxSwingAngle < 10.0f) {
            maxSwingAngle = 10.0f;
        }
        break;
    case 'j':
        if (!isJumping) {
            isJumping = true;
            jumpVelocity = 0.2f;
        }
        break;
    case 'i':
        robotPosition = initialPosition;
        robotDirection = glm::vec3(0.0f);
        robotRotationAngle = initialRotationAngle;

        armSwingAngle = 0.0f;
        legSwingAngle = 0.0f;

        isJumping = false;
        jumpVelocity = 0.0f;

        isSwingingForward = true;

        robotModelMatrix = glm::mat4(1.0f);
        break;
    }

    robotModelMatrix = glm::translate(glm::mat4(1.0f), robotPosition);
    robotModelMatrix = glm::rotate(robotModelMatrix, glm::radians(robotRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));

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

    if (isCubeFrontXmove) {
        if (isCubeFrontXmoveReverse) {
            cubeFrontX -= 0.1f;
            if (cubeFrontX <= -20.0f) {
                cubeFrontX = -20.0f;
            }
        }
        else {
            cubeFrontX += 0.1f;
            if (cubeFrontX >= 20.0f) {
                cubeFrontX = 20.0f;
            }
        }
    }

    if (isSwingingForward) {
        armSwingAngle += 2.0f;
        legSwingAngle -= 2.0f;
        if (armSwingAngle >= maxSwingAngle) {
            isSwingingForward = false;
        }
    }
    else {
        armSwingAngle -= 2.0f;
        legSwingAngle += 2.0f;
        if (armSwingAngle <= -maxSwingAngle) {
            isSwingingForward = true;
        }
    }

    if (isJumping) {
        robotPosition.y += jumpVelocity;
        jumpVelocity += gravity;

        if (robotPosition.y >= maxJumpHeight) {
            jumpVelocity = gravity;
        }

        // ���� �����ϸ� ���� ����
        if (robotPosition.y <= 0.0f) {
            robotPosition.y = 0.0f;
            isJumping = false;
            jumpVelocity = 0.0f;
        }
    }

    // �κ� �̵� ó��
    robotPosition += robotDirection; // ���� �������� ��ġ ������Ʈ

    robotBox.min = robotPosition + glm::vec3(-0.8f, -10.0f, -0.8f);
    robotBox.max = robotPosition + glm::vec3(0.8f, -5.3f, 0.8f);

    if (checkCollision(robotBox, block1Box) ||
        checkCollision(robotBox, block2Box) ||
        checkCollision(robotBox, block3Box)) {
        robotPosition -= robotDirection; // �浹 �� �̵� ���
    }

    for (int i = 0; i < 6; ++i) {
        if (checkCollision(robotBox, cubeFaceBoxes[i])) {
            robotDirection = -robotDirection;
            robotRotationAngle += 180.0f;
        }
    }

    robotModelMatrix = glm::translate(glm::mat4(1.0f), robotPosition); // �̵� ����
    robotModelMatrix = glm::rotate(robotModelMatrix, glm::radians(robotRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f)); // ������ ȸ�� ����

    glutPostRedisplay();
    glutTimerFunc(16, Timer, 0);
}
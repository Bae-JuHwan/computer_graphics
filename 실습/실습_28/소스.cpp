#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <random>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include "���.h"

GLuint vaoBottom, vaoPyramid, vaoLightCube, vaoEarth, vaoVenus, vaoMercury, vaoSnow;
GLuint vboBottom[2], vboPyramid[2], vboLightCube[2], vboEarth[2], vboVenus[2], vboMercury[2], vboSnow[2];
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
Model modelBottom, modelPyramid, modelLightCube, modelEarth, modelVenus, modelMercury, modelSnow;

GLfloat lightAngle = 0.0f;      // ���� ����
GLfloat lightZmove = 3.0f;      // ���� z�� �̵�
GLfloat planetOrbit = 0.0f;
float lightIntensity = 1.0f;
int sierpinskiDepth = 0;

bool orbitPositiveY = false;    // ���� ����
bool orbitNegativeY = false;    // ���� ����
bool isLightZmove = false;      // ���� z�� �̵�
bool isLightZmoveReverse = false;   // ���� z�� �̵�
bool isSnowFalling = false;
bool isLightOn = false;
bool isLightPositionChange = false;

// �� ���� ������ ������ ����ü ����
struct Snowflake {
    glm::vec3 position; // ��ġ
    float speed;        // �ӵ�
};

// �� ���� 30�� ����
std::vector<Snowflake> snowflakes;

// ���� ������ �ʱ�ȭ
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> randomX(-5.0f, 5.0f);  // x�� ����
std::uniform_real_distribution<float> randomY(-1.0f, 7.0f);   // y�� �ʱ� ����
std::uniform_real_distribution<float> randomSpeed(0.01f, 0.05f); // �ӵ� ����

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
void InitBottom();
void InitPyramid();
void InitLightCube();
void InitEarth();
void InitVenus();
void InitMercury();
void InitSnow();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid SpecialKeys(int key, int x, int y);
GLvoid Timer(int value);

int window_Width = 800;
int window_Height = 600;

int currentLightColorIndex = 0; // ���� ���� ���� ���¸� ��Ÿ���� ����
glm::vec3 lightColors[] = {
    glm::vec3(1.0f, 1.0f, 1.0f), // ���
    glm::vec3(0.0f, 0.0f, 1.0f), // �Ķ���
    glm::vec3(1.0f, 0.0f, 0.0f)  // ������
};

int currentLightPosition = 0;
glm::vec3 lightPositions[] = {
    glm::vec3(0.0f, 3.0f, 3.0f),
    glm::vec3(3.0f, 0.0f, 0.0f),
    glm::vec3(-3.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, -3.0f)
};

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

void InitBottom() {
    read_obj_file("bottom.obj", modelBottom); // pyramid.obj �б�

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f);

    for (size_t i = 0; i < modelBottom.faces.size(); ++i) {
        Vertex v1 = modelBottom.vertices[modelBottom.faces[i].v1];
        Vertex v2 = modelBottom.vertices[modelBottom.faces[i].v2];
        Vertex v3 = modelBottom.vertices[modelBottom.faces[i].v3];

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

    InitBuffer(vaoBottom, vboBottom, expandedVertices, indices); // InitBuffer ȣ��
}

void InitPyramid() {
    read_obj_file("pyramid.obj", modelPyramid); // pyramid.obj �б�

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    glm::vec3 color = glm::vec3(0.5f, 0.8f, 1.0f);

    for (size_t i = 0; i < modelPyramid.faces.size(); ++i) {
        Vertex v1 = modelPyramid.vertices[modelPyramid.faces[i].v1];
        Vertex v2 = modelPyramid.vertices[modelPyramid.faces[i].v2];
        Vertex v3 = modelPyramid.vertices[modelPyramid.faces[i].v3];

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

    InitBuffer(vaoPyramid, vboPyramid, expandedVertices, indices); // InitBuffer ȣ��
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

void InitSnow() {
    read_obj_file("snow.obj", modelSnow); // OBJ ���� �б�

    std::vector<Vertex> expandedVertices;
    std::vector<unsigned int> indices;

    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

    for (size_t i = 0; i < modelSnow.faces.size(); ++i) {
        Vertex v1 = modelSnow.vertices[modelSnow.faces[i].v1];
        Vertex v2 = modelSnow.vertices[modelSnow.faces[i].v2];
        Vertex v3 = modelSnow.vertices[modelSnow.faces[i].v3];

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

    InitBuffer(vaoSnow, vboSnow, expandedVertices, indices); // InitBuffer ȣ��
}

void InitSnowflakes() {
    for (int i = 0; i < 30; ++i) {
        Snowflake snowflake;
        snowflake.position = glm::vec3(randomX(gen), randomY(gen), randomX(gen)); // ���� ��ġ
        snowflake.speed = randomSpeed(gen);                                      // ���� �ӵ�
        snowflakes.push_back(snowflake);
    }
}

// �� ������ ��ġ ������Ʈ
void UpdateSnowflakes() {
    if (!isSnowFalling) return;

    for (auto& snowflake : snowflakes) {
        snowflake.position.y -= snowflake.speed; // �Ʒ��� �̵�

        // �ٴڿ� �����ϸ� �ٽ� �ʱ�ȭ
        if (snowflake.position.y < -1.0f) {
            snowflake.position.y = randomY(gen);
            snowflake.position.x = randomX(gen);
            snowflake.position.z = randomX(gen);
            snowflake.speed = randomSpeed(gen);
        }
    }
}

// �� ������
void DrawSnowflakes() {
    glBindVertexArray(vaoSnow);
    for (const auto& snowflake : snowflakes) {
        glm::mat4 snowModelMatrix = glm::mat4(1.0f);
        snowModelMatrix = glm::translate(snowModelMatrix, snowflake.position);

        GLint modelLocation = glGetUniformLocation(shaderProgramID, "model");
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(snowModelMatrix));

        glDrawElements(GL_TRIANGLES, modelSnow.faces.size() * 3, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

// �þ��ɽ�Ű ����Ż �ﰢ�� �׸���
void drawSierpinskiTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, int depth) {
    if (depth == 0) {
        // ���̰� 0�̸� �ﰢ���� �׸��ϴ�.
        glBegin(GL_TRIANGLES);
        glVertex3f(p1.x, p1.y, p1.z);
        glVertex3f(p2.x, p2.y, p2.z);
        glVertex3f(p3.x, p3.y, p3.z);
        glEnd();
        return;
    }

    // �ﰢ���� �� ���� ������ ����մϴ�.
    glm::vec3 mid1 = (p1 + p2) / 2.0f;
    glm::vec3 mid2 = (p2 + p3) / 2.0f;
    glm::vec3 mid3 = (p3 + p1) / 2.0f;

    // ��������� 3���� ���� �ﰢ���� �׸��ϴ�.
    drawSierpinskiTriangle(p1, mid1, mid3, depth - 1);
    drawSierpinskiTriangle(mid1, p2, mid2, depth - 1);
    drawSierpinskiTriangle(mid3, mid2, p3, depth - 1);
}

// �� �鸶�� �׸���
void drawPyramidWithSierpinski(int depth) {
    glm::mat4 sierpinskiModelMatrix = glm::mat4(1.0f); // �⺻ �� ���
    unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(sierpinskiModelMatrix));

    glm::vec3 apex = { 0.0f, 1.1f, 0.0f }; // �Ƕ�̵� �����
    glm::vec3 base1 = { 1.1f, -1.1f, -1.1f };
    glm::vec3 base2 = { 1.1f, -1.1f, 1.1f };
    glm::vec3 base3 = { -1.1f, -1.1f, -1.1f };
    glm::vec3 base4 = { -1.1f, -1.1f, 1.1f };

    // �Ƕ�̵��� �� �ﰢ�� �鿡 ���� �þ��ɽ�Ű �ﰢ���� �׸��ϴ�.
    drawSierpinskiTriangle(apex, base2, base1, depth);
    drawSierpinskiTriangle(apex, base1, base3, depth); 
    drawSierpinskiTriangle(apex, base3, base4, depth);
    drawSierpinskiTriangle(apex, base4, base2, depth);
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
    InitBottom();
    InitPyramid();
    InitLightCube();
    InitEarth();
    InitVenus();
    InitMercury();
    InitSnow();
    InitSnowflakes();

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
        glm::vec3(0.0f, 5.0f, 10.0f),  // ī�޶� ��ġ
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

    // �ٴ�
    glm::mat4 bottomModelMatrix = glm::mat4(1.0f);
    unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(bottomModelMatrix));
    glBindVertexArray(vaoBottom);
    glDrawElements(GL_TRIANGLES, modelBottom.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // �Ƕ�̵�
    glm::mat4 pyramidModelMatrix = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(pyramidModelMatrix));
    glBindVertexArray(vaoPyramid);
    glDrawElements(GL_TRIANGLES, modelPyramid.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // ����
    glm::mat4 earthModelMatrix = glm::mat4(1.0f);
    earthModelMatrix = glm::rotate(earthModelMatrix, glm::radians(planetOrbit), glm::vec3(0.0f, 1.0f, 0.0f));
    earthModelMatrix = glm::translate(earthModelMatrix, glm::vec3(-3.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(earthModelMatrix));
    glBindVertexArray(vaoEarth);
    glDrawElements(GL_TRIANGLES, modelEarth.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // �ݼ�
    glm::mat4 venusModelMatrix = glm::mat4(1.0f);
    venusModelMatrix = glm::rotate(venusModelMatrix, glm::radians(planetOrbit), glm::vec3(0.0f, 1.0f, 0.0f));
    venusModelMatrix = glm::translate(venusModelMatrix, glm::vec3(5.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(venusModelMatrix));
    glBindVertexArray(vaoVenus);
    glDrawElements(GL_TRIANGLES, modelVenus.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // ����
    glm::mat4 mercuryModelMatrix = glm::mat4(1.0f);
    mercuryModelMatrix = glm::rotate(mercuryModelMatrix, glm::radians(planetOrbit), glm::vec3(0.0f, 1.0f, 0.0f));
    mercuryModelMatrix = glm::translate(mercuryModelMatrix, glm::vec3(0.0f, 0.0f, -7.0f));
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(mercuryModelMatrix));
    glBindVertexArray(vaoMercury);
    glDrawElements(GL_TRIANGLES, modelMercury.faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // �� ������
    DrawSnowflakes();

    // �þ��ɽ�Ű �ﰢ�� ������ - ��� ���� �ʱ�ȭ
    drawPyramidWithSierpinski(sierpinskiDepth);

    // ���� ���� ��ȯ
    glm::mat4 orbitTransform = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 lightPos = glm::vec3(orbitTransform * glm::vec4(lightPositions[currentLightPosition], 1.0f)); // ���� ��ġ
    glm::vec3 lightColor = isLightOn ? lightColors[currentLightColorIndex] : glm::vec3(0.0f, 0.0f, 0.0f);  // ���� ����
    glm::vec3 viewPos(0.0f, 5.0f, 10.0f);    // ī�޶� ��ġ

    GLuint lightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");
    GLuint viewPosLoc = glGetUniformLocation(shaderProgramID, "viewPos");
    // ������ ���� ����
    GLuint lightIntensityLoc = glGetUniformLocation(shaderProgramID, "lightIntensity");
    glUniform1f(lightIntensityLoc, lightIntensity);

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

    glm::mat4 lightModelMatrix = glm::mat4(1.0f);
    lightModelMatrix = glm::translate(lightModelMatrix, lightPos);
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(lightModelMatrix));

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
    case 'f':
        isLightZmove = !isLightZmove;
        break;
    case 'n':
        isLightZmoveReverse = !isLightZmoveReverse;
        break;
    case '+':
        lightIntensity += 0.1f;
        if (lightIntensity > 2.0f)
            lightIntensity = 2.0f;
        break;
    case '-':
        lightIntensity -= 0.1f;
        if (lightIntensity < 0.0f)
            lightIntensity = 0.0f;
        break;
    case 's':
        isSnowFalling = !isSnowFalling;
        break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
        sierpinskiDepth = key - '0'; // ���� Ű���� ���̸� ����
        break;
    case 'm':
        isLightOn = !isLightOn;
        break;
    case 'c':
        currentLightColorIndex = (currentLightColorIndex + 1) % 3; // 0, 1, 2 ��ȯ
        glutPostRedisplay(); // ȭ�� ����
        break;
    case 'p':
        currentLightPosition = (currentLightPosition + 1) % 5;  // 0,1,2,3,4,5 ��ȯ
        glutPostRedisplay();
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

    if (isLightZmove) {
        lightZmove += 0.01f;
    }
    if (isLightZmoveReverse) {
        lightZmove -= 0.01f;
    }

    planetOrbit += 1.0f;

    // �� ���� ������Ʈ
    UpdateSnowflakes();

    glutPostRedisplay();

    glutTimerFunc(16, Timer, 0);
}
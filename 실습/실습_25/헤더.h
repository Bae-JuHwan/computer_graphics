#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

struct Vertex {
    GLfloat x, y, z;
    glm::vec3 normal;
    glm::vec3 color;
};

struct Face {
    unsigned int v1, v2, v3;
};

struct Model {
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
};

void read_obj_file(const std::string& filename, Model& model) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error opening file: " + filename);
    }

    std::string line;
    std::vector<glm::vec3> normals; // 법선 데이터를 저장할 벡터

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue; // 빈 줄과 주석 무시

        std::istringstream iss(line);

        if (line.rfind("v ", 0) == 0) { // 정점 데이터
            Vertex vertex;
            iss.ignore(2); // "v " 건너뜀
            if (!(iss >> vertex.x >> vertex.y >> vertex.z)) {
                throw std::runtime_error("Failed to parse vertex data in file: " + filename);
            }
            model.vertices.push_back(vertex);
        }
        else if (line.rfind("vn ", 0) == 0) { // 법선 데이터
            glm::vec3 normal;
            iss.ignore(3); // "vn " 건너뜀
            if (!(iss >> normal.x >> normal.y >> normal.z)) {
                throw std::runtime_error("Failed to parse normal data in file: " + filename);
            }
            normals.push_back(normal);
        }
        else if (line.rfind("f ", 0) == 0) { // 면 데이터
            Face face;
            std::string v1_str, v2_str, v3_str;

            iss.ignore(2); // "f " 건너뜀
            if (!(iss >> v1_str >> v2_str >> v3_str)) {
                throw std::runtime_error("Failed to parse face data in file: " + filename);
            }

            auto parse_vertex_index = [](const std::string& token, int& normalIndex) -> unsigned int {
                size_t first_slash = token.find('/');
                size_t second_slash = token.find('/', first_slash + 1);
                normalIndex = (second_slash != std::string::npos) ? std::stoi(token.substr(second_slash + 1)) - 1 : -1;
                return std::stoi(token.substr(0, first_slash)) - 1;
                };

            try {
                int normalIndex1, normalIndex2, normalIndex3;
                face.v1 = parse_vertex_index(v1_str, normalIndex1);
                face.v2 = parse_vertex_index(v2_str, normalIndex2);
                face.v3 = parse_vertex_index(v3_str, normalIndex3);

                model.faces.push_back(face);

                // 법선을 정점과 연결
                model.vertices[face.v1].normal = normals[normalIndex1];
                model.vertices[face.v2].normal = normals[normalIndex2];
                model.vertices[face.v3].normal = normals[normalIndex3];
            }
            catch (const std::exception& e) {
                std::cerr << "Error parsing face line: " << line
                    << ", exception: " << e.what() << std::endl;
            }
        }
    }
}
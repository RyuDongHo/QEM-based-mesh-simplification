#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Vertex{
public:
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec4 color;
    std::vector<int> adjacentVertices; // 인접한 정점들의 인덱스
    glm::mat4 quadric; // QEM에서 사용하는 쿼드릭 행렬

    Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& uv, const glm::vec4& col)
        : position(pos), normal(norm), texCoord(uv), color(col), quadric(glm::mat4(0.0f)) {}
        
};
/**
 * Face.h
 * 
 * 삼각형 면 (Triangle Face)
 * - 3개의 vertex 인덱스
 * - 면 법선 (normal)
 * - 평면 방정식 (plane equation): ax + by + cz + d = 0
 */

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Face
{
public:
  int v1, v2, v3;              // Vertex indices (정점 인덱스)
  glm::vec3 normal;            // Face normal (면의 법선)
  glm::vec4 planeEquation;     // [a, b, c, d] for ax + by + cz + d = 0

  // Default constructor
  Face() : v1(0), v2(0), v3(0), normal(glm::vec3(0.0f)), planeEquation(glm::vec4(0.0f)) {}

  /**
   * Constructor with vertex positions
   * 
   * 정점 위치로부터 면 법선과 평면 방정식 계산
   * 
   * @param idx1, idx2, idx3 정점 인덱스
   * @param pos1, pos2, pos3 정점 위치 (3D coordinates)
   */
  Face(int idx1, int idx2, int idx3,
       const glm::vec3 &pos1, const glm::vec3 &pos2, const glm::vec3 &pos3)
      : v1(idx1), v2(idx2), v3(idx3)
  {
    // Compute face normal using cross product
    // normal = normalize( (pos2 - pos1) × (pos3 - pos1) )
    glm::vec3 edge1 = pos2 - pos1;  // Edge vector 1
    glm::vec3 edge2 = pos3 - pos1;  // Edge vector 2
    normal = glm::normalize(glm::cross(edge1, edge2));

    // Compute plane equation: ax + by + cz + d = 0
    // where (a,b,c) = normal, d = -dot(normal, point_on_plane)
    float d = -glm::dot(normal, pos1);

    // Store as vec4: [a, b, c, d]
    planeEquation = glm::vec4(normal.x, normal.y, normal.z, d);
  }
};
  }
};
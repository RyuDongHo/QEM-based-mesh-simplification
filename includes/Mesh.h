/**
 * Mesh.h
 * 
 * Mesh 데이터 구조
 * - Vertices: 메시의 모든 정점 (position, normal, UV, quadric)
 * - Edges: 메시의 모든 간선 (중복 제거)
 * - Faces: 메시의 모든 삼각형 (plane equation 포함)
 */

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Vertex.h"
#include "Edge.h"
#include "Face.h"
#include <unordered_map>
#include <map>

class Mesh
{
public:
  std::vector<Vertex> vertices;  // 메시의 모든 정점
  std::vector<Edge> edges;       // 메시의 모든 간선 (unique)
  std::vector<Face> faces;       // 메시의 모든 면 (triangles)

  /**
   * Build mesh from OBJ data
   * 
   * OBJ 데이터로부터 Mesh 구조 생성:
   * 1. Vertices 생성 (position, normal, UV)
   * 2. Faces 생성 (triangulated, plane equation 계산)
   * 3. Edges 추출 (Face로부터, 중복 제거)
   * 
   * @param numVertices OBJ의 vertex 개수
   * @param vertices 정점 위치 배열
   * @param uvs 텍스처 좌표 배열
   * @param normals 법선 벡터 배열
   */

  void buildMesh(int numVertices, const std::vector<glm::vec3> &vertices,
                 const std::vector<glm::vec2> &uvs, const std::vector<glm::vec3> &normals)
  {
    // -----------------------------------------------------------------------
    // Step 1: Build vertices from OBJ data
    // -----------------------------------------------------------------------
    for (int i = 0; i < numVertices; ++i)
    {
      // Create vertex with position, normal, UV, and default white color
      this->vertices.push_back(Vertex(vertices[i], glm::vec3(0.0f), glm::vec2(0.0f), glm::vec4(1.0f)));
      this->vertices[i].normal = normals[i];
      this->vertices[i].texCoord = uvs[i];
    }

    // -----------------------------------------------------------------------
    // Step 2: Build faces (assume OBJ is already triangulated)
    // -----------------------------------------------------------------------
    for (int i = 0; i < numVertices; i += 3)
    {
      if (i + 2 < numVertices)
      {
        // Create face with 3 vertex indices and compute plane equation
        Face face(i, i + 1, i + 2,
                  this->vertices[i].position,
                  this->vertices[i + 1].position,
                  this->vertices[i + 2].position);
        faces.push_back(face);
      }
    }

    // -----------------------------------------------------------------------
    // Step 3: Extract unique edges from faces
    // -----------------------------------------------------------------------
    // Use map to filter duplicate edges
    // Edge (v1, v2) === Edge (v2, v1), so normalize with min/max
    std::map<std::pair<int, int>, bool> edgeSet;
    
    for (const auto &face : faces)
    {
      // Each face has 3 edges: (v1,v2), (v2,v3), (v3,v1)
      auto e1 = std::make_pair(std::min(face.v1, face.v2), std::max(face.v1, face.v2));
      auto e2 = std::make_pair(std::min(face.v2, face.v3), std::max(face.v2, face.v3));
      auto e3 = std::make_pair(std::min(face.v3, face.v1), std::max(face.v3, face.v1));

      // Add edge only if not already in set
      if (!edgeSet[e1])
      {
        edges.push_back(Edge(e1.first, e1.second));
        edgeSet[e1] = true;
      }
      if (!edgeSet[e2])
      {
        edges.push_back(Edge(e2.first, e2.second));
        edgeSet[e2] = true;
      }
      if (!edgeSet[e3])
      {
        edges.push_back(Edge(e3.first, e3.second));
        edgeSet[e3] = true;
      }
    }
  }
};
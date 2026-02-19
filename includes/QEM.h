/**
 * QEM.h
 * 
 * Quadric Error Metric (QEM) 알고리즘 구현
 * 
 * 참고 논문:
 * Garland, M., & Heckbert, P. S. (1997). 
 * "Surface simplification using quadric error metrics." SIGGRAPH 97.
 */

#include "Mesh.h"

/**
 * Compute collapse cost for an edge
 * 
 * QEM 공식을 사용하여 edge collapse 비용 계산:
 * 1. Q_edge = Q_v1 + Q_v2 (quadric 합산)
 * 2. 최적 위치 v* = argmin(v^T · Q · v)
 * 3. Cost = v*^T · Q · v*
 * 
 * @param edge 계산할 edge (cost와 optimalPosition이 업데이트됨)
 * @param vertices 메시의 모든 vertex (각 vertex는 quadric을 가짐)
 * @param faces 메시의 모든 face (unused in this function)
 */

void computeCost(Edge &edge, const std::vector<Vertex> &vertices, const std::vector<Face> &faces)
{
  // Combine quadrics from both vertices
  // Q_edge = Q_v1 + Q_v2
  glm::mat4 Q = vertices[edge.v1].quadric + vertices[edge.v2].quadric;

  // Compute optimal collapse position
  // Modify last row to enforce w=1 constraint (homogeneous coordinates)
  glm::mat4 Q_bar = Q;
  Q_bar[3] = glm::vec4(0, 0, 0, 1);  // [0,0,0,1] for w=1 constraint

  glm::vec4 optimalPos;
  if (glm::determinant(Q_bar) != 0)
  {
    // Solve linear system: Q_bar · v = [0,0,0,1]^T
    // Result: optimal position with w=1
    optimalPos = glm::inverse(Q_bar) * glm::vec4(0, 0, 0, 1);
  }
  else
  {
    // Singular matrix: fallback to midpoint
    // TODO: Try v1, v2, midpoint and pick minimum cost
    optimalPos = glm::vec4((vertices[edge.v1].position + vertices[edge.v2].position) * 0.5f, 1.0f);
  }

  // Store optimal position (vec4 → vec3)
  edge.optimalPosition = glm::vec3(optimalPos);

  // Compute collapse cost: error = v^T · Q · v
  // Note: Use original Q (not Q_bar) for accurate error measurement
  edge.cost = glm::dot(optimalPos, Q * optimalPos);
}

/**
 * Compute quadric matrix for a vertex
 * 
 * Vertex의 quadric = 인접한 모든 face들의 fundamental quadric 합
 * Q_vertex = Σ Kp (for all adjacent faces)
 * 
 * Kp = p · p^T (outer product)
 * where p = [a, b, c, d]^T = plane equation
 * 
 * @param vertex 계산할 vertex (quadric이 업데이트됨)
 * @param faces 메시의 모든 face
 * @param vertices 모든 vertex (인접 face 찾기용)
 * 
 * TODO: 성능 개선 - position 비교 대신 vertex index 비교 사용
 */

void computeQuadric(Vertex &vertex, const std::vector<Face> &faces, const std::vector<Vertex> &vertices)
{
  // Initialize quadric to zero matrix
  vertex.quadric = glm::mat4(0.0f);
  
  // Sum quadrics from all adjacent faces
  for (const Face &face : faces)
  {
    // Check if this vertex belongs to the face
    // TODO: Use vertex index instead of position comparison for performance
    if (vertices[face.v1].position == vertex.position 
      || vertices[face.v2].position == vertex.position 
      || vertices[face.v3].position == vertex.position)
    {
      // Get plane equation: p = [a, b, c, d]^T
      glm::vec4 p = face.planeEquation;
      
      // Compute fundamental quadric: Kp = p · p^T (outer product)
      // Result is 4x4 symmetric matrix
      glm::mat4 Kp = glm::outerProduct(p, p);
      
      // Add to vertex quadric
      vertex.quadric += Kp;
    }
  }
}

/**
 * Edge collapse operation
 * 
 * Edge를 collapse하여 vertex를 병합:
 * 1. 새로운 vertex 위치 = edge.optimalPosition
 * 2. 영향받는 face들 업데이트
 * 3. Degenerate face 제거 (area=0 면)
 * 4. 인접 edge들의 cost 재계산
 * 
 * @param mesh 메시 데이터 (vertices, faces가 수정됨)
 * @param edge collapse할 edge
 * 
 * TODO: 현재는 미완성 (placeholder)
 */
void edgeCollapse(Mesh &mesh, const Edge &edge)
{
  // TODO: Implement edge collapse logic
  // 1. Update vertex position to optimal
  // 2. Update faces referencing removed vertex
  // 3. Remove degenerate faces
  // 4. Update adjacent edges' costs
}
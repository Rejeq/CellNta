#include "Cellnta/Renderer/Drawer/Scene.h"

#include <type_traits>

using namespace Cellnta;

SceneDrawer::SceneDrawer() {
  CELLNTA_PROFILE;

  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);

  static_assert(std::is_same<HypercubeStorage::Point, float>::value,
                "glVertexAttribute has another type");
  glBindBuffer(GL_ARRAY_BUFFER, m_cubeDrawer.GetPointsBuffer());
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                        3 * sizeof(HypercubeStorage::Point), nullptr);
  glEnableVertexAttribArray(0);

  static_assert(std::is_same<NCellStorage::Point, float>::value,
                "glVertexAttribute has another type");
  glBindBuffer(GL_ARRAY_BUFFER, m_cellsDrawer.GetBuffer());
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                        3 * sizeof(NCellStorage::Point), nullptr);
  glVertexAttribDivisor(1, 1);
  glEnableVertexAttribArray(1);
}

SceneDrawer::~SceneDrawer() {
  CELLNTA_PROFILE;

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glDeleteVertexArrays(1, &m_vao);
}

bool SceneDrawer::CreateShaders(const std::string& gridPath,
                                const std::string& cellPath) {
  CELLNTA_PROFILE;

  if (m_cellShader.GetID() != 0)
    m_cellShader.Delete();

  if (m_gridShader.GetID() != 0)
    m_gridShader.Delete();

  if (m_gridShader.Create(gridPath)) {
    CELLNTA_LOG_ERROR("Unable to load grid shader");
    return true;
  }

  if (m_cellShader.Create(cellPath)) {
    CELLNTA_LOG_ERROR("Unable to load cell shader");
    return true;
  }

  const Eigen::Vector4f gridColor(0.2f, 0.2f, 0.2f, 0.8f);
  const Eigen::Vector4f xAxisColor(0.1f, 0.1f, 1.0f, 1.0f);
  const Eigen::Vector4f zAxisColor(1.0f, 0.1f, 0.1f, 1.0f);

  m_gridShader.Use();
  m_gridShader.Set("u_gridColor", gridColor);
  m_gridShader.Set("u_xAxisColor", xAxisColor);
  m_gridShader.Set("u_zAxisColor", zAxisColor);

  m_cellShader.Use();
  m_cellShader.Set("u_facesColorTex", 0);

  UpdateDistance(0);
  return false;
}

void SceneDrawer::DrawGrid() {
  CELLNTA_PROFILE;

  m_gridShader.Use();
  glDrawArrays(GL_TRIANGLES, 0, 12);
}

void SceneDrawer::Draw() {
  CELLNTA_PROFILE;

  if (m_cellsDrawer.GetSize() == 0)
    return;

  m_cellShader.Use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_BUFFER, m_cubeDrawer.GetColor().GetTexture());

  static_assert(std::is_same<HypercubeStorage::Ind, uint16_t>::value,
                "Indices in glDrawElement has another type");
  glDrawElementsInstanced(m_cubeDrawer.GetMode(), m_cubeDrawer.GetIndicesSize(),
                          GL_UNSIGNED_SHORT, nullptr, m_cellsDrawer.GetSize());
}

void SceneDrawer::UpdateDistance(float distance) {
  CELLNTA_PROFILE;

  constexpr float FarPlane = 6.25f;

  m_gridShader.Use();
  m_gridShader.Set("u_near", 0.01f);
  m_gridShader.Set("u_far", (float)distance * FarPlane);
}

void SceneDrawer::UpdateCamera(const Eigen::Matrix4f& projView) {
  CELLNTA_PROFILE;

  m_cellShader.Use();
  m_cellShader.Set("u_projView", projView);

  m_gridShader.Use();
  m_gridShader.Set("u_projView", projView);
}

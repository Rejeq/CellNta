#pragma once

#include <Eigen/Dense>

namespace Cellnta {

static float radians(float degrees) {
  return degrees * static_cast<float>(0.01745329251994329576923690768489);
}

class CameraBase {
 public:
  CameraBase() = default;
  CameraBase(int dim) { SetDimensions(dim); }
  virtual ~CameraBase() {}

  bool Update() {
    bool out = p_updateMatrix;
    p_updateMatrix = false;

    if (p_updateView) {
      UpdateViewMatrix();
      p_updateView = false;
      out = true;
    }
    if (p_updateProj) {
      UpdateProjMatrix();
      p_updateProj = false;
      out = true;
    }

    return out;
  }

  virtual void SetDimensions(int dim) { m_dim = dim; }
  int GetDimensions() const { return m_dim; }
  virtual void SetUsePerspective(bool newState) { p_usePerspective = newState; }
  bool GetUsePerspective() const { return p_usePerspective; }

  virtual void SetViewMatrix(const Eigen::MatrixXf& view) = 0;
  virtual void SetProjectionMatrix(const Eigen::MatrixXf& proj) = 0;
  virtual void SetPosition(const Eigen::VectorXf& pos) = 0;
  virtual void SetFront(const Eigen::VectorXf& front) = 0;
  virtual void SetWorldUp(const Eigen::MatrixXf& worldUp) = 0;
  virtual void SetFovDegrees(float fov) { SetFov(radians(fov)); }
  virtual void SetFov(float fov) = 0;

  virtual Eigen::MatrixXf GetViewMatrix() const { return Eigen::MatrixXf(); }
  virtual Eigen::MatrixXf GetProjectionMatrix() const {
    return Eigen::MatrixXf();
  }
  virtual Eigen::MatrixXf GetUps() const { return Eigen::MatrixXf(); }
  virtual Eigen::VectorXf GetPosition() const { return Eigen::VectorXf(); }
  virtual Eigen::VectorXf GetFront() const { return Eigen::VectorXf(); }

 protected:
  virtual void UpdateViewMatrix() = 0;
  virtual void UpdateProjMatrix() = 0;

  uint32_t GetVecSize() const { return m_dim + 1; }

  bool p_updateView = true;
  bool p_updateProj = true;
  bool p_updateMatrix = true;

  bool p_usePerspective = true;
  float p_fov = 45.0f;  // radians(45.0f);
 private:
  int m_dim = 0;
};

}  // namespace Cellnta

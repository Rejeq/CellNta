#pragma once

namespace Cellnta {

inline float radians(float degrees) {
  return degrees * static_cast<float>(0.01745329251994329576923690768489);
}

class CameraController {
 public:
  CameraController() = default;
  virtual ~CameraController() = default;

  bool Update();

  void SetUsePerspective(bool newState);
  bool GetUsePerspective() const { return p_usePerspective; }

  void ForceUpdateView() { p_updateView = true; }
  void ForceUpdateProjection() { p_updateProj = true; }
  void ForceUpdateMatrix() { p_updateMatrix = true; }

 protected:
  virtual void UpdateViewMatrix() = 0;
  virtual void UpdateProjMatrix() = 0;

  bool p_updateView = true;
  bool p_updateProj = true;
  bool p_updateMatrix = true;
  bool p_usePerspective = true;
};

}  // namespace Cellnta

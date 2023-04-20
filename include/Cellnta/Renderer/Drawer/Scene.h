#pragma once

#include <Eigen/Core>

#include "Cellnta/Renderer/Drawer/Hypercube.h"
#include "Cellnta/Renderer/Drawer/NCell.h"

namespace Cellnta {

class SceneDrawer {
 public:
  virtual ~SceneDrawer() = default;

  virtual bool CreateShaders(const std::string& gridPath,
                             const std::string& cellPath) = 0;

  virtual void DrawGrid() = 0;
  virtual void Draw() = 0;

  virtual void UpdateDistance(float distance) = 0;
  virtual void UpdateCamera(const Eigen::Matrix4f& projView) = 0;

  virtual HypercubeDrawer& GetCube() = 0;
  virtual const HypercubeDrawer& GetCube() const = 0;

  virtual NCellDrawer& GetCell() = 0;
  virtual const NCellDrawer& GetCell() const = 0;
};

}  // namespace Cellnta

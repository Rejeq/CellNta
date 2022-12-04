#pragma once

#include <Eigen/Dense>

#include "Cellnta/Renderer/HypercubeStorage.h"
#include "Cellnta/Renderer/NCellStorage.h"

namespace Cellnta {

Eigen::MatrixXf LookAt(const int N, const Eigen::VectorXf& from,
                       const Eigen::VectorXf& to, const Eigen::MatrixXf& ups);
Eigen::MatrixXf Perspective(const int N, double eyeAngle, double nearPlane,
                            double farPlane, double aspect);

void NProject(NCellStorage& cells, const int cameraDim,
              const Eigen::MatrixXf& viewProj, bool perspective);
void NProject(HypercubeStorage& cube, const int cameraDim,
              const Eigen::MatrixXf& viewProj, bool perspective);

Eigen::MatrixXf NRotate(int N, int axis1, int axis2, float angle);

}  // namespace Cellnta

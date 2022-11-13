#pragma once

#include <Eigen/Dense>

#include "Cellnta/Renderer/HypercubeStorage.h"
#include "Cellnta/Renderer/NCellStorage.h"

namespace Cellnta {

Eigen::MatrixXf LookAt(const size_t N, const Eigen::VectorXf& from, const Eigen::VectorXf& to, const Eigen::MatrixXf& ups);
Eigen::MatrixXf Perspective(const size_t N, double eyeAngle, double nearPlane, double farPlane, double aspect);

void NProject(NCellStorage& cells, const size_t cameraDim, const Eigen::MatrixXf& viewProj, bool perspective);
void NProject(HypercubeStorage& cube, const size_t cameraDim, const Eigen::MatrixXf& viewProj, bool perspective);

Eigen::MatrixXf NRotate(size_t N, size_t axis1, size_t axis2, float angle);

} //namespace Cellnta

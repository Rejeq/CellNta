#pragma once

#include <Eigen/LU>

namespace Cellnta {

class HypercubeStorage;
class NCellStorage;

Eigen::MatrixXf LookAt(int N, const Eigen::VectorXf& from,
                       const Eigen::VectorXf& to, const Eigen::MatrixXf& ups);
Eigen::MatrixXf Perspective(int N, double eyeAngle, double nearPlane,
                            double farPlane, double aspect);

void NProject(NCellStorage& cells, int cameraDim,
              const Eigen::MatrixXf& viewProj, bool perspective);
void NProject(HypercubeStorage& cube, int cameraDim,
              const Eigen::MatrixXf& viewProj, bool perspective);

Eigen::MatrixXf NRotate(int N, int axis1, int axis2, float angle);

}  // namespace Cellnta

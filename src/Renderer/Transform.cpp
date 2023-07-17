#include "Cellnta/Renderer/Transform.h"

#include <cfloat>

#include "Cellnta/Config.h"
#include "Cellnta/Renderer/HypercubeStorage.h"
#include "Cellnta/Renderer/NCellStorage.h"

using namespace Cellnta;

static Eigen::VectorXf getNormalVector(Eigen::MatrixXf vectors) {
  CELLNTA_PROFILE;

  assert(vectors.rows() == vectors.cols() + 1);
  const int N = vectors.rows();

  Eigen::MatrixXf pM = vectors.transpose();
  pM.conservativeResize(pM.rows() + 1, Eigen::NoChange);
  pM.row(N - 1) = Eigen::VectorXf::Zero(N);
  Eigen::MatrixXf baseVectors = Eigen::MatrixXf::Identity(N, N);
  Eigen::VectorXf result = Eigen::VectorXf::Zero(N);

  int signal = 1;
  for (int i = 0; i < N; i++) {
    Eigen::MatrixXf pS(N - 1, N - 1);

    for (int j = 0; j < (N - 1); j++) {
      pS.block(j, 0, 1, i) = pM.block(j, 0, 1, i);
      pS.block(j, i, 1, N - i - 1) = pM.block(j, i + 1, 1, N - i - 1);
    }

    result += signal * baseVectors.row(i) * pS.determinant();
    signal *= -1;
  }

  return result;
}

Eigen::MatrixXf Cellnta::LookAt(const int N, const Eigen::VectorXf& from,
                                const Eigen::VectorXf& to,
                                const Eigen::MatrixXf& ups) {
  CELLNTA_PROFILE;

  assert(N > 2);
  assert(N == from.size());
  assert(N == to.size());
  assert(N == ups.rows());

  Eigen::MatrixXf rot(N, N);  // rotate matrix

  rot.rightCols(1) = (to - from).normalized();

  // std::cout << m << std::endl;
  int numLoops = 0;
  Eigen::MatrixXf vectorsToCross(N, N - 1);
  for (int maxCol = N - 2; maxCol > 0; --maxCol) {
    int curCol = 1;

    // First, cross product all ups, in order
    vectorsToCross.col(0) = ups.col(numLoops);
    for (int i = 1; i < maxCol; ++i, ++curCol)
      vectorsToCross.col(curCol) = ups.col(numLoops + i);

    ++numLoops;

    for (int i = 0; i < numLoops; ++i, ++curCol)
      vectorsToCross.col(curCol) = rot.col(maxCol + i + 1);

    rot.col(maxCol) = getNormalVector(vectorsToCross).normalized();
  }

  rot.col(0) = getNormalVector(rot.rightCols(N - 1)).normalized();

  Eigen::MatrixXf temp = Eigen::MatrixXf::Identity(N + 1, N + 1);
  temp.topLeftCorner(N, N) = rot;

  Eigen::MatrixXf translate = Eigen::MatrixXf::Identity(N + 1, N + 1);

  translate.topRightCorner(N, 1) = -from;

  return temp.transpose() * translate;
}

Eigen::MatrixXf Cellnta::Perspective(const int N, double eyeAngle,
                                     double nearPlane, double farPlane,
                                     double aspect) {
  CELLNTA_PROFILE;

  assert(N > 2);
  Eigen::MatrixXf m = Eigen::MatrixXf::Zero(N + 1, N + 1);
  float f_tan = 1.0f / tan(eyeAngle / 2.0f);

  if (N == 3) {
    m(0, 0) = f_tan / aspect;
    m(1, 1) = f_tan;
    m(2, 2) = (nearPlane + farPlane) / (nearPlane - farPlane);
    m(3, 2) = -1.0f;
    m(2, 3) = 2 * (nearPlane * farPlane) / (nearPlane - farPlane);
    return m;
  }

  m.diagonal().array() = f_tan;
  m(N - 1, N - 1) = 1;
  m(N, N) = 1;
  return m;
}

void Cellnta::NProject(NCellStorage& cells, int cameraDim,
                       const Eigen::MatrixXf& viewProj, bool perspective) {
  CELLNTA_PROFILE;

  const int divPos = ((perspective) ? 2 : 1);
  // TODO: Add feature that behaviour can be disabled in gui
  // FIXME: Use reference
  NCellStorage::VecList visibleRaw = cells.GetVisibleRaw();

  auto iter = cells.GetRaw().MakeWholeIter();

  NCellStorage::Vec pos = NCellStorage::Vec::Zero(cameraDim + 1);
  while (const auto* cell = iter.Next()) {
    assert((cameraDim + 1 <= cell->pos.size()) &&
           "Cell position must be homogeneous");

    pos = cell->pos.topRightCorner(cameraDim + 1, 1);

    pos = viewProj * pos;
    const float div = pos[pos.rows() - divPos];
    pos /= (div == 0.0f) ? FLT_EPSILON : div;

    // Overriding existing data its not bug here
    // because visibleRaw is already copy
    visibleRaw.Move(cell->pos, pos);
  }
}

void Cellnta::NProject(HypercubeStorage& cube, int cameraDim,
                       const Eigen::MatrixXf& viewProj, bool perspective) {
  CELLNTA_PROFILE;

  const int rows = cameraDim + 1;
  const int cols = cube.GetVerticesCount();
  const int divPos = ((perspective) ? 2 : 1);
  float* points = cube.GetPoints();

  for (int vert = 0; vert < cols; ++vert) {
    Eigen::Map<Eigen::VectorXf> pos(&points[vert * cube.GetVertexSize()], rows);

    pos = viewProj * pos;
    const float& div = pos(pos.rows() - divPos);
    pos /= (div == 0.0f) ? FLT_EPSILON : div;
  }
}

Eigen::MatrixXf Cellnta::NRotate(int N, int axis1, int axis2, float angle) {
  assert(axis1 != axis2);
  assert(axis1 < N);
  assert(axis2 < N);

  Eigen::MatrixXf rot_aa = Eigen::MatrixXf::Identity(N + 1, N + 1);

  rot_aa(axis1, axis1) = std::cos(angle);
  rot_aa(axis1, axis2) = std::sin(angle);
  rot_aa(axis2, axis1) = -std::sin(angle);
  rot_aa(axis2, axis2) = std::cos(angle);

  return rot_aa.transpose();
}

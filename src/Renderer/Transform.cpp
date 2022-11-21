#include "Cellnta/Renderer/Transform.h"

#include <cfloat>

#include "Cellnta/Config.h"

using namespace Cellnta;

static Eigen::VectorXf getNormalVector(Eigen::MatrixXf vectors)
{
  CELLNTA_PROFILE;

	assert(vectors.rows() == vectors.cols() + 1);
	const unsigned int N = vectors.rows();

	Eigen::MatrixXf pM = vectors.transpose();
	pM.conservativeResize(pM.rows() + 1, Eigen::NoChange);
	pM.row(N - 1) = Eigen::VectorXf::Zero(N);
	Eigen::MatrixXf baseVectors = Eigen::MatrixXf::Identity(N, N);
	Eigen::VectorXf result = Eigen::VectorXf::Zero(N);

	int signal = 1;
	for (unsigned int i = 0; i < N; i++)
	{
		Eigen::MatrixXf pS(N - 1, N - 1);

		for (unsigned int j = 0; j < (N - 1); j++) {
			pS.block(j, 0, 1, i) = pM.block(j, 0, 1, i);
			pS.block(j, i, 1, N - i - 1) = pM.block(j, i + 1, 1, N - i - 1);
		}

		result += signal * baseVectors.row(i) * pS.determinant();
		signal *= -1;
	}

	return result;
}

Eigen::MatrixXf Cellnta::LookAt(const size_t N, const Eigen::VectorXf& from, const Eigen::VectorXf& to, const Eigen::MatrixXf& ups)
{
  CELLNTA_PROFILE;

	assert(N > 2);
	assert(N == (size_t) from.size());
	assert(N == (size_t) to.size());
	assert(N == (size_t) ups.rows());

	Eigen::MatrixXf rot(N, N); //rotate matrix

	rot.rightCols(1) = (to - from).normalized();

	//std::cout << m << std::endl;
	int numLoops = 0;
	Eigen::MatrixXf vectorsToCross(N, N - 1);
	for (int maxCol = N - 2; maxCol > 0; --maxCol)
	{
		int curCol = 1;

		//First, cross product all ups, in order
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

Eigen::MatrixXf Cellnta::Perspective(const size_t N, double eyeAngle, double nearPlane, double farPlane, double aspect)
{
  CELLNTA_PROFILE;

	assert(N > 2);
	Eigen::MatrixXf m = Eigen::MatrixXf::Zero(N + 1, N + 1);
	float f_tan = 1.0f / tan(eyeAngle / 2.0f);

	if (N == 3)
	{
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

void Cellnta::NProject(NCellStorage& cells, const size_t cameraDim, const Eigen::MatrixXf& viewProj, bool perspective)
{
  CELLNTA_PROFILE;

  const uint32_t divPos = ((perspective) ? 2 : 1);

  for (size_t i = 0; i < cells.size(); ++i)
  {
    Eigen::Map<Eigen::VectorXf> pos(cells.at(i).data(), cameraDim + 1);

    pos = viewProj * pos;
    const float& div = pos(pos.rows() - divPos);
    pos /= (div == 0.0f)
      ? FLT_EPSILON
      : div;
  }
}

void Cellnta::NProject(HypercubeStorage& cube, const size_t cameraDim, const Eigen::MatrixXf& viewProj, bool perspective)
{
  CELLNTA_PROFILE;

	const size_t   rows = cameraDim + 1;
	const size_t   cols = cube.GetVerticesCount();
	const uint32_t divPos = ((perspective) ? 2 : 1);
	float* points = cube.GetPoints();

	for (size_t vert = 0; vert < cols; ++vert)
	{
		Eigen::Map<Eigen::VectorXf> pos(&points[vert * cube.GetVertexSize()], rows);

		pos = viewProj * pos;
		const float& div = pos(pos.rows() - divPos);
		pos /= (div == 0.0f)
      ? FLT_EPSILON
      : div;
	}
}

Eigen::MatrixXf Cellnta::NRotate(size_t N, size_t axis1, size_t axis2, float angle)
{
  assert(axis1 != axis2);
  assert(axis1 < N);
  assert(axis2 < N);

  Eigen::MatrixXf rot_aa = Eigen::MatrixXf::Identity(N + 1, N + 1);

  rot_aa(axis1, axis1) = cos(angle);
  rot_aa(axis1, axis2) = sin(angle);
  rot_aa(axis2, axis1) = -sin(angle);
  rot_aa(axis2, axis2) = cos(angle);

  return rot_aa.transpose();
}

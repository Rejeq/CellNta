#include "Cellnta/Renderer/CameraNd.h"

#include "Cellnta/Renderer/Transform.h"
#include "Cellnta/Config.h"

using namespace Cellnta;

void CameraNd::SetDimensions(size_t dim)
{
  ProfileScope;

	CameraBase::SetDimensions(dim);
	const size_t currDim = GetDimensions();

	m_front.resize(currDim);
	m_pos.resize(currDim);
	m_proj.resize(GetVecSize(), GetVecSize());
	m_view.resize(GetVecSize(), GetVecSize());

	m_ups = Eigen::MatrixXf::Zero(currDim, currDim - 2);
	//This creates the Up vectors in an order that will not rotate the axes
	m_ups.block(1, 0, currDim - 3, currDim - 3) = Eigen::MatrixXf::Identity(currDim - 3, currDim - 3).rowwise().reverse();
	m_ups(currDim - 1, currDim - 3) = 1.f;

	for (int i = 0; i < m_ups.cols(); i++)
		m_ups(0, i) += 1.e-20f;
	//std::cout << "Ups: \n" << m_ups << std::endl << std::endl;
}

void CameraNd::SetUsePerspective(bool newState)
{
	CameraBase::SetUsePerspective(newState);
	p_updateView = true;
}

void CameraNd::UpdateViewMatrix()
{
	m_view = LookAt(GetDimensions(), m_pos, m_front, m_ups);
}

void CameraNd::UpdateProjMatrix()
{
	m_proj = Perspective(GetDimensions(), p_fov, 0.0f, 0.0f, 0.0f);
}

Eigen::MatrixXf CameraNd::CalculateViewProj() const
{
  ProfileScope;

  //std::cout << "View: (" << GetDimensions() << "dim)" << std::endl;
  //std::cout << view << std::endl << std::endl;
  //std::cout << "Proj: (" << GetDimensions() << "dim)" << std::endl;
  //std::cout << proj << std::endl << std::endl;

	Eigen::MatrixXf result = m_proj * m_view;

	//Axis direction correction
	Eigen::MatrixXf aux = Eigen::MatrixXf::Identity(GetVecSize(), GetVecSize());
	//X corrections
	aux(0, 0) = -1;
	//Z correction
	aux(2, 2) = GetDimensions() != 4 ? -1 : 1;
	result = aux * result;

	return result;
}

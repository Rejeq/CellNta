#include "Cellnta/Canvas.h"

//#include <stdexcept>

using namespace Cellnta;

Canvas::Canvas(const AlgoType algo, const uint32_t dimensions)
{
  CELLNTA_PROFILE;

  const size_t D = dimensions;

  m_algo = CreateAlgoInstance(algo);
  if (m_algo == nullptr)
    assert(0);

  m_algo->SetDimension(D);

  Renderer::Init(D);
  Renderer::GenrateHypercube(0.5f);
}

Canvas::~Canvas()
{
  CELLNTA_PROFILE;
}

void Canvas::NextGeneration()
{
  CELLNTA_PROFILE;
  m_algo->Update();
}

void Canvas::RenderGrid()
{
  CELLNTA_PROFILE;
  Renderer::DrawGrid();
}

void Canvas::RenderWorld()
{
  CELLNTA_PROFILE;
  Renderer::Draw();
}

void Canvas::ResetTimer()
{
  m_nextGenTimer = m_nextGenTimerInit;
}

void Canvas::SetTimer(const float time)
{
  m_nextGenTimerInit = time;
  m_nextGenTimer = time;
}

void Canvas::DisableTimer()
{
  m_nextGenTimer = FLT_MAX;
  m_nextGenTimerInit = FLT_MAX;
}

bool Canvas::ChangeAlgoType(const AlgoType type)
{
  std::unique_ptr<AlgoBase> tmp = CreateAlgoInstance(type);
  if (tmp == nullptr)
    return 1;
  tmp->SetupFrom(*m_algo);
  m_algo = std::move(tmp);
  return 0;
}

void Canvas::SetDimension(uint32_t dim)
{
  CELLNTA_PROFILE;
  Renderer::SetDimension(dim);
  if (m_algo != nullptr)
    m_algo->SetDimension(dim);
}

void Canvas::Update(float delta)
{
  CELLNTA_PROFILE;

  //TODO: If the delta is large, it is necessary
  //to make a new generation many times
  if (m_nextGenTimer != FLT_MAX)
  {
    m_nextGenTimer -= delta;
    if (m_nextGenTimer < 0.0f)
    {
      NextGeneration();
      ResetTimer();
    }
  }

  if (m_algo->NeedLoadWorld() || Renderer::GetData().DesireArea())
    m_algo->LoadWorld(&Renderer::GetData());
  Renderer::Update();
}

void Canvas::OnMove(MoveDirection dir, float delta)
{
  p_camera.Move(dir, delta);
}

void Canvas::OnMouseMotion(int32_t xrel, int32_t yrel, float delta)
{
  p_camera.Rotate(xrel, yrel, delta);
}

void Canvas::OnResize(uint32_t width, uint32_t height)
{
  p_camera.Resize(Eigen::Vector2f(width, height));
}

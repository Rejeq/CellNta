#pragma once

#include <cfloat>
#include <memory>

#include "Algorithms/AlgoBase.h"
#include "Algorithms/AlgoList.h"

//#include "Shader.h"
//#include "Camera3d.h"
#include "Renderer/Renderer.h"

namespace Cellnta {

class Canvas: public Renderer
{
public:
	Canvas(const AlgoType algo, const uint32_t dimensions);
	~Canvas();

	void Update(float delta);
	void NextGeneration();
  bool ChangeAlgoType(const AlgoType type);

	void RenderGrid();
	void RenderWorld();

  void ResetTimer();
  void DisableTimer();
  void SetTimer(const float time);
  bool TimerEnabled() const { return m_nextGenTimerInit != FLT_MAX; }
  float GetTimerInit() const { return m_nextGenTimerInit; }
  float GetTimer() const { return m_nextGenTimer; }

	void SetDimension(uint32_t dim);

	AlgoBase* GetAlgo() { return m_algo.get(); }

	void OnMove(MoveDirection dir, float delta);
	void OnMouseMotion(int32_t xrel, int32_t yrel, float delta);
	void OnResize(uint32_t width, uint32_t height);

private:
	std::unique_ptr<AlgoBase> m_algo = nullptr;
  float m_nextGenTimer = FLT_MAX;
  float m_nextGenTimerInit = FLT_MAX;
};
}

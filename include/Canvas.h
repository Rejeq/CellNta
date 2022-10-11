#pragma once

#include <cfloat>
#include <memory>

#include "AlgoBase.h"
#include "AlgoList.h"

#include "Shader.h"
#include "Camera3d.h"
#include "Renderer.h"

class Canvas: public Renderer
{
public:
	Canvas(const Lf::AlgoType algo, const uint32_t dimensions);
	~Canvas();

	void Update(float delta);
	void NextGeneration();
  bool ChangeAlgoType(const Lf::AlgoType type);

	void RenderGrid();
	void RenderWorld();

  void ResetTimer();
  void DisableTimer();
  void SetTimer(const float time);
  bool TimerEnabled() const { return m_nextGenTimerInit != FLT_MAX; }
  float GetTimerInit() const { return m_nextGenTimerInit; }
  float GetTimer() const { return m_nextGenTimer; }

	void SetDimension(uint32_t dim);

	Lf::AlgoBase* GetAlgo() { return m_algo.get(); }

	void OnMove(MoveDirection dir, float delta);
	void OnMouseMotion(int32_t xrel, int32_t yrel, float delta);
	void OnResize(uint32_t width, uint32_t height);

private:
	std::unique_ptr<Lf::AlgoBase> m_algo = nullptr;
  float m_nextGenTimer = FLT_MAX;
  float m_nextGenTimerInit = FLT_MAX;
};

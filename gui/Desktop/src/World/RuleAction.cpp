#include "World/RuleAction.h"

#include <Cellnta/World/Rule.h>
#include <Cellnta/World/World.h>

#include "Context.h"
#include "Log.h"
#include "Renderer/RenderDataAction.h"

using namespace Ui;
using namespace Ui::Action::Rule;

static const Cellnta::Rule& GetRule(Context& ctx) {
  return ctx.GetWorld().GetRule();
}

static bool SetRule(Context& ctx, const Cellnta::Rule& rule) {
  return ctx.GetWorld().SetRule(rule);
}

void SetDimension::Execute() {
  const Cellnta::Rule& rule = GetRule(*p_ctx);

  m_prevRule = std::make_unique<Cellnta::Rule>(rule);
  SetRule(*p_ctx, rule.ChangeDimension(m_dim));
}

void SetDimension::Undo() {
  if (m_prevRule != nullptr)
    SetRule(*p_ctx, *m_prevRule);
}

void SetTotalStates::Execute() {
  const Cellnta::Rule& rule = GetRule(*p_ctx);

  m_prevRule = std::make_unique<Cellnta::Rule>(rule);
  SetRule(*p_ctx, rule.ChangeTotalStates(m_totalStates));
}

void SetTotalStates::Undo() {
  if (m_prevRule != nullptr)
    SetRule(*p_ctx, *m_prevRule);
}

void SetState::Execute() {
  Cellnta::Rule rule = GetRule(*p_ctx);

  m_prevBecome = rule.GetState(m_targetState, m_mask);
  rule.SetState(m_targetState, m_mask, m_become);
  SetRule(*p_ctx, rule);
}

void SetState::Undo() {
  Cellnta::Rule rule = GetRule(*p_ctx);

  rule.SetState(m_targetState, m_mask, m_prevBecome);
  SetRule(*p_ctx, rule);
}

void ResetState::Execute() {
  Cellnta::Rule rule = GetRule(*p_ctx);

  m_prevBecome = rule.GetState(m_targetState, m_mask);
  rule.ResetState(m_targetState, m_mask);
  SetRule(*p_ctx, rule);
}

void ResetState::Undo() {
  Cellnta::Rule rule = GetRule(*p_ctx);

  rule.SetState(m_targetState, m_mask, m_prevBecome);
  SetRule(*p_ctx, rule);
}

void ReplaceState::Execute() {
  Cellnta::Rule rule = GetRule(*p_ctx);

  rule.ReplaceState(m_targetState, m_oldMask, m_newMask);
  SetRule(*p_ctx, rule);
}

void ReplaceState::Undo() {
  Cellnta::Rule rule = GetRule(*p_ctx);

  rule.ReplaceState(m_targetState, m_newMask, m_oldMask);
  SetRule(*p_ctx, rule);
}

void SetFallbackState::Execute() {
  Cellnta::Rule rule = GetRule(*p_ctx);

  m_prevFallback = rule.GetFallbackState(m_targetState);
  rule.SetFallbackState(m_targetState, m_fallback);
  SetRule(*p_ctx, rule);
}

void SetFallbackState::Undo() {
  Cellnta::Rule rule = GetRule(*p_ctx);

  rule.SetFallbackState(m_targetState, m_prevFallback);
  SetRule(*p_ctx, rule);
}

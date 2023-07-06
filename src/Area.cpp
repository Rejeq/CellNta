#include "Cellnta/Area.h"

#include "Cellnta/Axis.h"
#include "Cellnta/Config.h"
#include "Cellnta/Log.h"

using namespace Cellnta;

Area::Area() {
  m_min = Eigen::Vector3i::Zero();
  m_max = Eigen::Vector3i::Zero();
}

Area::Area(const Eigen::Vector3i& min, const Eigen::Vector3i& max) {
  m_min = min;
  m_max = max;
}

Area::Area(const int min, const int max) {
  m_min = Eigen::Vector3i::Constant(min);
  m_max = Eigen::Vector3i::Constant(max);
}

bool Area::PosValid(const Eigen::VectorXi& pos) const {
  CELLNTA_PROFILE;

  for (int i = 0; i < GetSize(); ++i) {
    int point = (i < pos.size()) ? pos(i) : 0;

    if (!Axis::WithinBound(m_min[i], m_max[i], point))
      return false;
  }

  return true;
}

int Area::GetSize() const {
  assert(m_min.size() == m_max.size() && "Min and max sizes must be equal");
  return m_min.size();
}

std::vector<Area> Area::InverseClip(const Area& clipper, const Area& subject) {
  CELLNTA_PROFILE;

  if (!clipper.Valid())
    return {};

  Area inter = Area::Intersection(clipper, subject);
  if (!inter.Valid())
    return {clipper};

  std::vector<Area> out;
  Area tmp;

  // Front area
  tmp =
      Area({inter.m_max.x(), inter.m_min.y(), inter.m_min.z()}, clipper.m_max);
  if (tmp.Valid())
    out.push_back(tmp);

  // Behind area
  tmp =
      Area(clipper.m_min, {inter.m_min.x(), inter.m_max.y(), inter.m_max.z()});
  if (tmp.Valid())
    out.push_back(tmp);

  // Right area
  tmp = Area({inter.m_min.x(), inter.m_min.y(), inter.m_max.z()}, clipper.m_max);
  if (tmp.Valid())
    out.push_back(tmp);

  // Left area
  tmp = Area(clipper.m_min, {inter.m_max.x(), inter.m_min.y(), inter.m_max.z()});
  if (tmp.Valid())
    out.push_back(tmp);

  // Top area
  tmp =
      Area({inter.m_min.x(), inter.m_max.y(), inter.m_min.z()}, clipper.m_max);
  if (tmp.Valid())
    out.push_back(tmp);

  // Bottom area
  tmp =
      Area(clipper.m_min, {inter.m_max.x(), inter.m_max.y(), inter.m_min.z()});
  if (tmp.Valid())
    out.push_back(tmp);

  return out;
}

Area Area::Intersection(const Area& first, const Area& second) {
  CELLNTA_PROFILE;

  Area out;
  out.m_min.x() = std::max(first.m_min.x(), second.m_min.x());
  out.m_min.y() = std::max(first.m_min.y(), second.m_min.y());
  out.m_min.z() = std::max(first.m_min.z(), second.m_min.z());

  out.m_max.x() = std::min(first.m_max.x(), second.m_max.x());
  out.m_max.y() = std::min(first.m_max.y(), second.m_max.y());
  out.m_max.z() = std::min(first.m_max.z(), second.m_max.z());
  return out;
}

bool Area::Valid() const {
  CELLNTA_PROFILE;

  return (m_min.x() < m_max.x() && m_min.y() < m_max.y() &&
          m_min.z() < m_max.z());
}

bool Area::SetMinAxis(int idx, int val) {
  if (idx >= 3) {
    CELLNTA_LOG_TRACE("Unable to set max axis in Area: Out of bound axis '{}'",
                      idx);
    return true;
  }

  m_min[idx] = val;
  return false;
}

int Area::MinAxis(int idx) const {
  return (idx >= 3) ? Axis::GetInvalidLowerBound() : m_min[idx];
}

bool Area::SetMaxAxis(int idx, int val) {
  if (idx >= 3) {
    CELLNTA_LOG_TRACE("Unable to set max axis in Area: Out of bound axis '{}'",
                      idx);
    return true;
  }

  m_max[idx] = val;
  return false;
}

int Area::MaxAxis(int idx) const {
  return (idx >= 3) ? Axis::GetInvalidUpperBound() : m_max[idx];
}

Area Area::operator+(const Area& r) const {
  return Area(m_min + r.m_min, m_max + r.m_max);
}

Area Area::operator+(const Eigen::Vector3i& r) const {
  return Area(m_min + r, m_max + r);
}

Area Area::operator-(const Area& r) const {
  return Area(m_min - r.m_min, m_max - r.m_max);
}

Area Area::operator-(const Eigen::Vector3i& r) const {
  return Area(m_min - r, m_max - r);
}

bool Area::operator==(const Area& r) const {
  return (m_min == r.m_min && m_max == r.m_max);
}

bool Area::operator!=(const Area& r) const {
  return !(*this == r);
}

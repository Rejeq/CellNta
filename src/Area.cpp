#include "Cellnta/Area.h"

#include "Cellnta/Config.h"

using namespace Cellnta;

Area::Area(const Eigen::Vector3i& min, const Eigen::Vector3i& max) {
  this->min = min;
  this->max = max;
}

Area::Area(const int min, const int max) {
  this->min.array() = min;
  this->max.array() = max;
}

bool Area::PosValid(const Eigen::VectorXi& pos) const {
  CELLNTA_PROFILE;

  for (int i = 0; i < std::min(pos.size(), min.size()); ++i) {
    if (!((min(i) <= pos(i)) && (pos(i) <= max(i))))
      return false;
  }

  return true;
}

std::vector<Area> Area::InverseClip(const Area& clipper, const Area& subject) {
  CELLNTA_PROFILE;

  Area inter = Area::Intersection(clipper, subject);
  if (!inter.Valid())
    return {clipper};

  std::vector<Area> out;
  Area tmp;

  // Front area
  tmp = Area({inter.max.x(), inter.min.y(), inter.min.z()},
             clipper.max);
  if (tmp.Valid())
    out.push_back(tmp);

  // Behind area
  tmp = Area(clipper.min,
             {inter.min.x(), inter.max.y(), inter.max.z()});
  if (tmp.Valid())
    out.push_back(tmp);

  // Right area
  tmp = Area({inter.min.x(), inter.min.y(), inter.max.z()},
             clipper.max);
  if (tmp.Valid())
    out.push_back(tmp);

  // Left area
  tmp = Area(clipper.min,
             {inter.max.x(), inter.min.y(), inter.max.z()});
  if (tmp.Valid())
    out.push_back(tmp);

  // Top area
  tmp = Area({inter.min.x(), inter.max.y(), inter.min.z()},
             clipper.max);
  if (tmp.Valid())
    out.push_back(tmp);

  // Bottom area
  tmp = Area(clipper.min,
             {inter.max.x(), inter.max.y(), inter.min.z()});
  if (tmp.Valid())
    out.push_back(tmp);

  return out;
}

Area Area::Intersection(const Area& first, const Area& second) {
  CELLNTA_PROFILE;

  Area out;
  out.min.x() = std::max(first.min.x(), second.min.x());
  out.min.y() = std::max(first.min.y(), second.min.y());
  out.min.z() = std::max(first.min.z(), second.min.z());

  out.max.x() = std::min(first.max.x(), second.max.x());
  out.max.y() = std::min(first.max.y(), second.max.y());
  out.max.z() = std::min(first.max.z(), second.max.z());
  return out;
}

bool Area::Valid() const {
  CELLNTA_PROFILE;

  return (min.x() < max.x() && min.y() < max.y() && min.z() < max.z());
}

Area Area::operator+(const Area& r) const {
  return Area(min + r.min, max + r.max);
}

Area Area::operator+(const Eigen::Vector3i& r) const {
  return Area(min + r, max + r);
}

Area Area::operator-(const Area& r) const {
  return Area(min - r.min, max - r.max);
}

Area Area::operator-(const Eigen::Vector3i& r) const {
  return Area(min - r, max - r);
}

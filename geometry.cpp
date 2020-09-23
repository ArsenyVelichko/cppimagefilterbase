#include "geometry.h"

point::point() {
  m_x = 0;
  m_y = 0;
}

point::point(int x, int y) {
  m_x = x;
  m_y = y;
}

int point::x() const {
  return m_x;
}

int point::y() const {
  return m_y;
}

istream& operator>>(istream &in, point& point) {
  in >> point.m_x >> point.m_y;
  return in;
}

rect::rect(int left, int top, int right, int bottom) {
  m_min = point(left, top);
  m_max = point(right, bottom);
}

rect::rect(const point& min, const point& max) {
  m_min = min,
  m_max = max;
}

int rect::left() const {
  return m_min.x();
}

int rect::top() const {
  return m_min.y();
}

int rect::right() const {
  return m_max.x();
}

int rect::bottom() const {
  return m_max.y();
}

istream& operator>>(istream &in, rect &rect) {
  in >> rect.m_min >> rect.m_max;
  return in;
}
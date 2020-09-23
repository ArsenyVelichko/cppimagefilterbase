#include "geometry.h"
#include <algorithm>

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

point point::operator+=(const point& other) {
  m_x += other.x();
  m_y += other.y();
  return *this;
}

point point::operator-=(const point& other) {
  m_x -= other.x();
  m_y -= other.y();
  return *this;
}

point point::operator+(const point& other) const {
  return point(m_x + other.x(), m_y + other.y());
}

point point::operator-(const point& other) const {
  return point(m_x - other.x(), m_y - other.y());
}

istream& operator>>(istream &in, point& point) {
  in >> point.m_y >> point.m_x;
  return in;
}

rect::rect(int left, int top, int right, int bottom) {
  m_topLeft = point(left, top);
  m_bottomRight = point(right, bottom);
}

rect::rect(const point& min, const point& max) {
  m_topLeft = min,
  m_bottomRight = max;
}

int rect::left() const {
  return m_topLeft.x();
}

int rect::top() const {
  return m_topLeft.y();
}

int rect::right() const {
  return m_bottomRight.x();
}

int rect::bottom() const {
  return m_bottomRight.y();
}

point rect::topLeft() const {
  return m_topLeft;
}

point rect::bottomRight() const {
  return m_bottomRight;
}

rect rect::intersected(const rect& other) const {
  point commonMin = max(m_topLeft, other.topLeft());
  point commonMax = min(m_bottomRight, other.bottomRight());
  return rect(commonMin, commonMax);
}

rect rect::translated(const point& offset) const {
  return rect(m_topLeft + offset, m_bottomRight + offset);
}

istream& operator>>(istream &in, rect &rect) {
  in >> rect.m_topLeft >> rect.m_bottomRight;
  return in;
}

point min(const point& a, const point& b) {
  int minX = min(a.x(), b.x());
  int minY = min(a.y(), b.y());
  return point(minX, minY);
}

point max(const point& a, const point& b) {
  int maxX = max(a.x(), b.x());
  int maxY = max(a.y(), b.y());
  return point(maxX, maxY);
}
#pragma once

#include <iostream>

using namespace std;

class point {
public:
  point();
  point(int x, int y);

  int x() const;
  int y() const;

  point operator+=(const point& other);
  point operator-=(const point& other);
  point operator+(const point& other) const;
  point operator-(const point& other) const;

  friend istream& operator>>(istream &in, point& point);

private:
  int m_x, m_y;
};

class rect {
public:
  rect() {}
  rect(int left, int top, int right, int bottom);
  rect(const point& min, const point& max);

  int left() const;
  int top() const;
  int right() const;
  int bottom() const;

  size_t width() const;
  size_t height() const;

  point topLeft() const;
  point bottomRight() const;

  rect intersected(const rect& other) const;
  rect translated(const point& offset) const;

  friend istream& operator>>(istream &in, rect &rect);

private:
  point m_topLeft, m_bottomRight;
};

point min(const point& a, const point& b);
point max(const point& a, const point& b);

template<class T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
  return (v < lo) ? lo : (hi < v) ? hi : v;
}
#pragma once

#include <iostream>

using namespace std;

class point {
public:
  point();
  point(int x, int y);

  int x() const;
  int y() const;

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

  friend istream& operator>>(istream &in, rect &rect);

private:
  point m_min, m_max;
};
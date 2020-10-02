#pragma once

#include "png_toolkit.h"
#include "geometry.h"
#include <map>
#include <vector>

using namespace std;

class filter {
public:
  filter(const rect& rect);

  virtual void apply(const image_data& imageData) const = 0;

protected:
  rect calcScope(const image_data& imageData) const;

  rect m_rect;
};

enum class FilterType {
  BlackAndWhite,
  Red,
  Threshold,
  Edge,
  Blur,
};

class filter_factory {
public:
  static filter* createByName(const string& name, const rect& rect);
  static filter* createById(FilterType id, const rect& rect);

private:
  static map<string, FilterType> s_nameRegister;
};

class bw_filter : public filter {
public:
  bw_filter(const rect& rect);

  void apply(const image_data& imageData) const override;
};

class red_filter : public filter {
public:
  red_filter(const rect& rect);

  void apply(const image_data& imageData) const override;
};

class threshold_filter : public filter {
public:
  threshold_filter(const rect& rect);

  void apply(const image_data& imageData) const override;
};

class convolut_filter : public filter {
public:
  convolut_filter(const rect& rect);

  void apply(const image_data& imageData) const override;

  virtual vector<vector<int>> getKernel() const = 0;

private:
  vector<stbi_uc> calcConvolut(const image_data& imageData,
                               const point& kernelPos,
                               const rect& rect) const;
};

class edge_filter : public convolut_filter {
public:
  edge_filter(const rect& rect);

  void apply(const image_data& imageData) const override;
  vector<vector<int>> getKernel() const override;
};

class blur_filter : public convolut_filter {
public:
  blur_filter(const rect& rect);

  vector<vector<int>> getKernel() const override;
};
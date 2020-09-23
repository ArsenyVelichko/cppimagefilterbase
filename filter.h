#pragma once

#include "png_toolkit.h"
#include "geometry.h"
#include <map>

using namespace std;

class filter {
public:
  filter(const rect& rect);

  virtual void apply(const image_data& imageData) const = 0;

protected:
  rect calcScope(const image_data& imageData) const;

private:
  rect m_rect;
};

enum class FilterType {
  BlackAndWhite,
  Red,
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

  virtual void apply(const image_data& imageData) const override;
};

class red_filter : public filter {
public:
  red_filter(const rect& rect);

  virtual void apply(const image_data& imageData) const override;
};
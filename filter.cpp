#include "filter.h"
#include <iostream>
#include <vector>
#include <algorithm>

map<string, FilterType> filter_factory::s_nameRegister = {
  {"BlackAndWhite", FilterType::BlackAndWhite},
  {"Red", FilterType::Red},
  {"Threshold", FilterType::Threshold},
};


filter* filter_factory::createByName(const string& name, const rect& rect) {
  try {
    return createById(s_nameRegister.at(name), rect);

  } catch (out_of_range) {
    cout << "Filter name does not registered";
    return nullptr;
  }
}

filter* filter_factory::createById(FilterType id, const rect& rect) {
  filter* filter = nullptr;

  switch (id) {
  case FilterType::BlackAndWhite:
    filter = new bw_filter(rect);
    break;

  case FilterType::Red:
    filter = new red_filter(rect);
    break;

  case FilterType::Threshold:
    filter = new threshold_filter(rect);
    break;

  default:
    cout << "Filter id does not exist";
    break;
  }
  return filter;
}

filter::filter(const rect& rect) {
  m_rect = rect;
}

rect filter::calcScope(const image_data& imageData) const {
  int width = imageData.w;
  int height = imageData.h;

  int left = m_rect.left() ? width / m_rect.left() : 0;
  int top = m_rect.top() ? height / m_rect.top() : 0;
  int right = m_rect.right() ? width / m_rect.right() : 0;
  int bottom = m_rect.bottom() ? height / m_rect.bottom() : 0;

  return rect(left, top, right, bottom);
}

static int mapToImage(const image_data& imageData, const point& point){
  int pixelIndex = point.y() * imageData.w + point.x();
  return pixelIndex * imageData.compPerPixel;
}

static stbi_uc calcIntensity(const stbi_uc* pixelPtr) {
  stbi_uc r = pixelPtr[0];
  stbi_uc g = pixelPtr[1];
  stbi_uc b = pixelPtr[2];
  return (3 * r + 6 * g + b) / 10;
}

bw_filter::bw_filter(const rect& rect) : filter(rect) {}

void bw_filter::apply(const image_data& imageData) const {
  stbi_uc* pixels = imageData.pixels;

  rect scope = calcScope(imageData);
  for (int i = scope.left(); i < scope.right(); i++) {
    for (int j = scope.top(); j < scope.bottom(); j++) {
      int k = mapToImage(imageData, point(i, j));
      pixels[k] = pixels[k + 1] = pixels[k + 2] = calcIntensity(pixels + k);
    }
  }
}

red_filter::red_filter(const rect& rect) : filter(rect) {}

void red_filter::apply(const image_data& imageData) const {
  stbi_uc* pixels = imageData.pixels;

  rect scope = calcScope(imageData);
  for (int i = scope.left(); i < scope.right(); i++) {
    for (int j = scope.top(); j < scope.bottom(); j++) {
      int k = mapToImage(imageData, point(i, j));
      pixels[k] = 255;
      pixels[k + 1] = 0;
      pixels[k + 2] = 0;
    }
  }
}

threshold_filter::threshold_filter(const rect& rect) : filter(rect) {}

static int calcIntensMedian(const image_data& imageData, const rect& rect) {
  stbi_uc* pixels = imageData.pixels;

  vector<int> intensityVec;
  for (int i = rect.left(); i < rect.right(); i++) {
    for (int j = rect.top(); j < rect.bottom(); j++) {
      int k = mapToImage(imageData, point(i, j));
      int intensity = calcIntensity(pixels + k);
      intensityVec.push_back(intensity);
    }
  }

  sort(intensityVec.begin(), intensityVec.end());

  size_t size = intensityVec.size();
  size_t halfSize = size / 2;
  if (size % 2 == 0) {
    return (intensityVec[halfSize - 1] + intensityVec[halfSize]) / 2;

  } else {
    return intensityVec[halfSize];
  }
}

void threshold_filter::apply(const image_data& imageData) const {
  bw_filter bwFilter(m_rect);
  bwFilter.apply(imageData);

  stbi_uc* pixels = imageData.pixels;
  size_t sizeInBytes = imageData.h * imageData.w * imageData.compPerPixel;
  stbi_uc* bufPixels = new stbi_uc[sizeInBytes];
  memcpy(bufPixels, imageData.pixels, sizeInBytes);

  image_data bufData;
  bufData.pixels = bufPixels;
  bufData.compPerPixel = imageData.compPerPixel;
  bufData.w = imageData.w;
  bufData.h = imageData.h;

  rect scope = calcScope(bufData);
  rect intensRect(-2, -2, 2, 2);
  for (int i = scope.left(); i < scope.right(); i++) {
    for (int j = scope.top(); j < scope.bottom(); j++) {
      point center = point(i, j);
      rect currRect = intensRect.translated(center);
      currRect = currRect.intersected(scope);

      int k = mapToImage(bufData, point(i, j));
      int intensity = calcIntensity(pixels + k);
      int median = calcIntensMedian(bufData, currRect);
      
      if (intensity < median) {
        pixels[k] = pixels[k + 1] = pixels[k + 2] = 0;
      }
    }
  }
  free(bufPixels);
}
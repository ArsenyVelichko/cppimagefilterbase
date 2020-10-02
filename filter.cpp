#include "filter.h"
#include <iostream>
#include <algorithm>

map<string, FilterType> filter_factory::s_nameRegister = {
  {"BlackAndWhite", FilterType::BlackAndWhite},
  {"Red", FilterType::Red},
  {"Threshold", FilterType::Threshold},
  {"Edge", FilterType::Edge},
  {"Blur", FilterType::Blur},
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

  case FilterType::Edge:
    filter = new edge_filter(rect);
    break;

  case FilterType::Blur:
    filter = new blur_filter(rect);
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

static int mapToImage(const image_data& imageData, const point& point) {
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

static stbi_uc calcIntensMedian(const image_data& imageData, const rect& rect) {
  stbi_uc* pixels = imageData.pixels;

  vector<stbi_uc> intensVec;
  for (int i = rect.left(); i < rect.right(); i++) {
    for (int j = rect.top(); j < rect.bottom(); j++) {
      int k = mapToImage(imageData, point(i, j));
      intensVec.push_back(pixels[k]);
    }
  }

  size_t halfSize = intensVec.size() / 2;
  nth_element(intensVec.begin(), intensVec.begin() + halfSize, intensVec.end());

  return intensVec[halfSize];
}

void threshold_filter::apply(const image_data& imageData) const {
  bw_filter bwFilter(m_rect);
  bwFilter.apply(imageData);

  stbi_uc* pixels = imageData.pixels;

  rect scope = calcScope(imageData);
  rect intensRect(-2, -2, 3, 3);
  vector<int> zeroedPixels;
  for (int i = scope.left(); i < scope.right(); i++) {
    for (int j = scope.top(); j < scope.bottom(); j++) {
      point center = point(i, j);
      rect currRect = intensRect.translated(center);
      currRect = currRect.intersected(scope);

      stbi_uc median = calcIntensMedian(imageData, currRect);

      int k = mapToImage(imageData, center);
      if (pixels[k] < median) {
        zeroedPixels.push_back(k);
      }
    }
  }

  for (int i : zeroedPixels) {
    pixels[i] = pixels[i + 1] = pixels[i + 2] = 0;
  }
}

convolut_filter::convolut_filter(const rect& rect) : filter(rect) {}

static vector<stbi_uc> calcConvolut(const image_data& imageData, const vector<vector<int>>& kernel,
                                    const point& kernelPos, const rect& rect) {
  vector<int> convolutInt(3);
  for (int color = 0; color < 3; color++) {
    for (int i = rect.left(); i < rect.right(); i++) {
      for (int j = rect.top(); j < rect.bottom(); j++) {
        int k = mapToImage(imageData, point(i, j));
        int weight = kernel[i - kernelPos.x()][j - kernelPos.y()];
        convolutInt[color] += imageData.pixels[k + color] * weight;
      }
    }
    convolutInt[color] = clamp(convolutInt[color], 0, 255);
  }

  vector<stbi_uc> convolutUc(3);
  for (int i = 0; i < 3; i++) {
    convolutUc[i] = stbi_uc(convolutInt[i]);
  } 
  return convolutUc;
}

static void pastePixels(const image_data& dst, const image_data& src, const point& topLeft) {
  for (int i = 0; i < src.w; i++) {
    for (int j = 0; j < src.h; j++) {
      point currPos = point(i, j);
      int k1 = mapToImage(dst, currPos);
      int k2 = mapToImage(dst, currPos + topLeft);

      dst.pixels[k2] = src.pixels[k1];
      dst.pixels[k2 + 1] = src.pixels[k1 + 1];
      dst.pixels[k2 + 2] = src.pixels[k1 + 2];
    }
  }
}

void convolut_filter::apply(const image_data& imageData) const {
  vector<vector<int>> kernel = getKernel();
  int halfW = kernel[0].size() / 2;
  int halfH = kernel.size() / 2;
  rect kernelRect(-halfW, -halfH, halfW + 1, halfH + 1);

  rect scope = calcScope(imageData);
  image_data bufData;
  bufData.w = scope.width();
  bufData.h = scope.height();
  bufData.compPerPixel = imageData.compPerPixel;
  bufData.pixels = new stbi_uc[bufData.w * bufData.h * bufData.compPerPixel];

  for (int i = scope.left(); i < scope.right(); i++) {
    for (int j = scope.top(); j < scope.bottom(); j++) {
      point currPos = point(i, j);
      rect currRect = kernelRect.translated(currPos);
      currRect = currRect.intersected(scope);

      point kernelPos = currPos - point(halfW, halfH);
      vector<stbi_uc> convolut = calcConvolut(imageData, kernel, kernelPos, currRect);

      int k = mapToImage(bufData, currPos - scope.topLeft());
      bufData.pixels[k] = convolut[0];
      bufData.pixels[k + 1] = convolut[1];
      bufData.pixels[k + 2] = convolut[2];
    }
  }

  pastePixels(imageData, bufData, scope.topLeft());
  delete[] bufData.pixels;
}

edge_filter::edge_filter(const rect& rect) : convolut_filter(rect) {}

vector<vector<int>> edge_filter::getKernel() const {
  vector<vector<int>> kernel = {
    {-1, -1, -1},
    {-1, 9, -1},
    {-1, -1, -1},
  };
  return kernel;
}

void edge_filter::apply(const image_data& imageData) const {
  bw_filter bwFilter(m_rect);
  bwFilter.apply(imageData);

  convolut_filter::apply(imageData);
}

blur_filter::blur_filter(const rect& rect) : convolut_filter(rect) {}

vector<vector<int>> blur_filter::getKernel() const {
  vector<vector<int>> kernel = {
    {1, 1, 1},
    {1, 1, 1},
    {1, 1, 1},
  };
  return kernel;
}

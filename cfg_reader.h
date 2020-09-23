#pragma once

#include <fstream>
#include "filter.h"

using namespace std;

class cfg_reader {
public:
  bool open(const char* cfgName);
  filter* readFilter();
  bool atEnd() const;

private:
  ifstream m_cfgFile;
};
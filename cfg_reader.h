#pragma once

#include <fstream>
#include "filter.h"

using namespace std;

class cfg_reader {
public:
  cfg_reader(const char* cfgName);

  filter* readFilter();
  bool atEnd() const;

private:
  ifstream m_cfgFile;
};
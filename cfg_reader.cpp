#include "cfg_reader.h"
#include "geometry.h"

cfg_reader::cfg_reader(const char* cfgName) {
  m_cfgFile = ifstream(cfgName);
}

filter* cfg_reader::readFilter() {
  string name;
  rect rect;
  m_cfgFile >> name >> rect;

  filter* filter = filter_factory::createByName(name, rect);
  return filter;
}

bool cfg_reader::atEnd() const {
  return m_cfgFile.eof();
}
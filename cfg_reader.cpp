#include "cfg_reader.h"
#include "geometry.h"

bool cfg_reader::open(const char* cfgName) {
  m_cfgFile = ifstream(cfgName);
  return m_cfgFile.is_open();
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
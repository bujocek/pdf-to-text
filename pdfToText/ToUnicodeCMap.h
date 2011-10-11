#pragma once
#include "IndirectObject.h"
#include "StringObject.h"
#include "StringUtils.h"
#include <list>

class ToUnicodeCMap
{
private:
  IndirectObject * indirectObject;
public:
  list <pair<StringObject*, StringObject*>> codeRanges;
  map<int, StringObject *> codeCharMap;
  /**
  checks using code ranges of the map if charCode byte sequence of len length is proper char code
  */
  bool isCharCode(unsigned char * charCode, int len);
  ToUnicodeCMap(IndirectObject * io);
  ~ToUnicodeCMap(void);
};

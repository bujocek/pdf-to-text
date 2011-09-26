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
  bool isCharCode(char * charCode, int len);
  ToUnicodeCMap(IndirectObject * io);
  ~ToUnicodeCMap(void);
};

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
  ToUnicodeCMap(IndirectObject * io);
  ~ToUnicodeCMap(void);
};

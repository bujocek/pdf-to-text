#pragma once
#include "IndirectObject.h"
#include "StringObject.h"
#include "StringUtils.h"

class ToUnicodeCMap
{
private:
  IndirectObject * indirectObject;
public:
  int codeRangeFrom;
  int codeRangeTo;
  int codeWidth;
  map<int, StringObject *> codeCharMap;
  ToUnicodeCMap(IndirectObject * io);
  ~ToUnicodeCMap(void);
};

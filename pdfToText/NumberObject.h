#pragma once
#include "pdfobject.h"

class NumberObject :
  public PdfObject
{
public:
  float number;
  NumberObject(float number);
  NumberObject(char ** endKey, char * source);
  ~NumberObject(void);
  static bool canBeNumber(char * source);
};

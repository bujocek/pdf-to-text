#pragma once
#include "pdfobject.h"

class BoolObject :
  public PdfObject
{
public:
  bool boolValue;
  BoolObject(bool value);
  BoolObject(char ** endKey, char * source);
  ~BoolObject(void);
};

#pragma once
#include "pdfobject.h"

class OperatorObject :
  public PdfObject
{
public:
  static bool canBeOperator(char * source);
  char * name;
  OperatorObject(char ** endKey, char * source);
  ~OperatorObject(void);
};

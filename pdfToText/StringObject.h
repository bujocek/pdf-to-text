#pragma once
#include "pdfobject.h"

class StringObject :
  public PdfObject
{
private:
  long numVal;
public:
  bool isHexa;
  char * string;
  StringObject(char ** endKey, char * source);
  ~StringObject(void);
  int toNum();
};

#pragma once
#include "pdfobject.h"

class StringObject :
  public PdfObject
{
private:
  unsigned char * byteString;
  long numVal;
public:
  bool isHexa;
  char * string;
  int length;
  int byteStringLen;
  StringObject(char ** endKey, char * source);
  ~StringObject(void);
  int toNum();
  unsigned char * getByteString();
};

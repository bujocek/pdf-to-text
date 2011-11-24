#pragma once
#include "pdfobject.h"
#include <list>

class ArrayObject :
  public PdfObject
{
public:
  std::vector<PdfObject*> objectList;
  ArrayObject(char ** endKey, char * source);
  ~ArrayObject(void);
};

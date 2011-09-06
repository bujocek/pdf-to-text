#pragma once
#include "pdfobject.h"

class IndirectObjectRefference :
  public PdfObject
{
public:
  long objectNumber;
  long generationNumber;
  IndirectObject * getIndirectObject();
  IndirectObjectRefference(char ** endKey, char * source);
  ~IndirectObjectRefference(void);
};

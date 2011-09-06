#pragma once
#include "pdfobject.h"

class NameObject :
  public PdfObject
{
public:
  char * name;

  static NameObject * getNameObject(char ** endKey, char * source);

  /**
  constructor:
  reads name object (i.e. /somename) and return new instance containing name
  @param source - The source string to get name from
  @param endKey - sets this reference to the position in source where the name ends
  */
  NameObject(char ** endKey, char * source);
  ~NameObject(void);
};

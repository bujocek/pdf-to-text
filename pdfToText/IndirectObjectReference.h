#pragma once
#include "pdfobject.h"

/**
Class for representing reference to IndirectObject. ("n m R")<br>
Provides functionality to get IndirectObject that is referenced by this class.
*/
class IndirectObjectReference :
  public PdfObject
{
public:
  
  /**
  Object number of referenced indirect object.
  */
  long objectNumber;

  /**
  Generation number of referenced indirect object.
  */
  long generationNumber;

  /**
  Method for geting referenced IndirectObject by this object.
  \return returns referenced IndirectObject
  */
  IndirectObject * getIndirectObject();

  /**
  Constructor. <br>
  Constructs IndirectObjectReference from <code>source</code>
  \param endKey Pointer to the end of the reference in <code>source</code> will be stored to this parameter.
  \param source Source string the reference will be read from.
  */
  IndirectObjectReference(char ** endKey, char * source);

  /**
  Destructor
  */
  ~IndirectObjectReference(void);
};

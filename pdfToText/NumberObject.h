#pragma once
#include "pdfobject.h"

/**
Simple class for representing Number PDF Objects.
*/
class NumberObject :
  public PdfObject
{
public:
  
  /**
  The number the object represents
  */
  float number;

  /**
  Constructor.<br>
  Constructs NumberObject from given float number.
  */
  NumberObject(float number);
  
  /**
  Constructor.<br>
  Constructs NumberObject from <code>source</code>.
  \param endKey Pointer to the end of the number in <code>source</code> will be stored to this parameter.
  \param source Source string the number will be read from.
  */
  NumberObject(char ** endKey, char * source);

  /**
  Destructor
  */
  ~NumberObject(void);

  /**
  Determines based on the <code>source</code> (first char) if object in <code>source</code> can be number object.
  \param source Posible source of the number.
  \return True if there is number in <code>source</code>. False if there is other object than number.
  */
  static bool canBeNumber(char * source);
};

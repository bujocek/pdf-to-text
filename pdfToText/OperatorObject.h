#pragma once
#include "pdfobject.h"

/**
Simple class for representing Operator PDF Objects.
*/
class OperatorObject :
  public PdfObject
{
public:
  
  /**
  Determines based on the source if object in <code>source</code> can be operator object.
  \param source Posible source of the operator.
  \return True if there is operator in <code>source</code>. False if there is other object than operator.
  */
  static bool canBeOperator(char * source);

  /**
  Name of the operator
  */
  char * name;

  /**
  Constructor.<br>
  Constructs OperatorObject from <code>source</code>.
  \param endKey Pointer to the end of the operator in <code>source</code> will be stored to this parameter.
  \param source Source string the operator will be read from.
  */
  OperatorObject(char ** endKey, char * source);

  /**
  Destructor
  */
  ~OperatorObject(void);
};

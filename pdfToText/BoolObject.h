#pragma once
#include "PdfObject.h"

/**
A simple class that represents PDF Boolean object. <br>
It is simple boolean value read from source and stored to <code>boolValue</code> property.
*/
class BoolObject :
  public PdfObject
{
public:
  
  /**
  Bool value of the object.
  */
  bool boolValue;
  
  /**
  A Construcor.<br>
  Creates <code>BoolObject</code> from <code>bool</code> value.
  */
  BoolObject(bool value);
  
  /**
  A Construcor.<br>
  Read bool string from source ( "true" or "false" ) and creates <code>BoolObject</code> from it. <br>
  Stores pointer after the value in <code>source</code> to <code>endKey</code>
  */
  BoolObject(char ** endKey, char * source);

  /**
  A Destructor
  */
  ~BoolObject(void);
};

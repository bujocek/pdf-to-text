#pragma once
#include "PdfObject.h"

/*
Class for representing string PDF objects.<br>
It stores literal and hexadecimal strings found in PDF. 
The class also provides some basic functionality for converting hexa string to byte string or to number.
*/
class StringObject :
  public PdfObject
{
private:
  unsigned char * byteString;
  long numVal;
public:
  
  /**
  Flag if this is hexa string or literal string
  */
  bool isHexa;
  
  /**
  The string that this object represents (without () or <>)
  */
  char * string;
  
  /**
  length of the string
  */
  int length;

  /**
  length of byte string
  */
  int byteStringLen;

  /**
  Constructor.<br>
  Constructs StringObject from <code>source</code>.
  \param endKey Pointer to the end of the string in <code>source</code> will be stored to this parameter.
  \param source Source string the string will be read from.
  */
  StringObject(char ** endKey, char * source);

  /**
  Destructor
  */
  ~StringObject(void);

  /**
  If it is hexa string than converts the string to number.
  \return Returns number converted from hexa string.
  */
  int toNum();
  
  /**
  Converts this hexa string to unsigned char string (byte string)
  \return This hexa string converted to bytes.
  */
  unsigned char * getByteString();
};

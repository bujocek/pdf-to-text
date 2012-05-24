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
  char * hexaString;
  char * charString;
  long numVal;

  /**
  length of byte string
  */
  int byteStringLen;
  
  /**
  length of char string
  */
  int charStringLen;

  /**
  length of hexa string
  */
  int hexaStringLen;

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
  length of byte string
  */
  int getByteStringLen();

  /**
  length of hexa string
  */
  int getHexaStringLen();

  /**
  Converts this string to unsigned char string (byte string)
  \return This string converted to bytes.
  */
  unsigned char * getByteString();

  /**
  Converts this string to char string
  \return This string converted to chars.
  */
  char * getCharString();

  /**
  Converts this string to hexa string
  \return This string converted to hexa string.
  */
  char * getHexaString();
};

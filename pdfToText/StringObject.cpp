#include "StdAfx.h"
#include "StringObject.h"

StringObject::StringObject(char ** endKey, char * source)
{
  this->objectType = PdfObject::TYPE_STRING;
  this->numVal = -1;
  this->byteString = null;
  this->byteStringLen = 0;
  this->length = 0;
  source = StringUtils::skipWhiteSpace(source);
  this->source = source;
  if(*source == '<')
  {
    *endKey = strchr(source,'>');
    if(*endKey == null)
    {
      this->string = null;
      cerr << "\nStringObject: Couldn't recognize string in source.\n";
    }
    else
    {
      source += 1;
      this->length = *endKey - source;
      this->string = new char[this->length + 1];
      strncpy(this->string, source, this->length);
      this->string[this->length] = 0;
      this->isHexa = true;
      *endKey += 1;
      this->byteStringLen = this->length/2 + this->length%2;
    }
  }
  else if(*source == '(')
  {
    *endKey = source;
    do //search for closing parantheses and check if there isn't backslash char before
    {
      *endKey = strchr(*endKey+1,')');
    }while(*((*endKey)-1) == '\\' && *endKey != null);

    if(*endKey == null)
    {
      this->string = null;
      cerr << "\nStringObject: Couldn't recognize string in source.\n";
    }
    else
    {
      source += 1;
      this->length = *endKey - source;
      this->string = new char[this->length + 1];
      strncpy(this->string, source, this->length);
      this->string[this->length] = 0;
      this->isHexa = false;
      *endKey += 1;
      this->byteStringLen = this->length;
    }
  }
  else
  {
    cerr << "\nStringObject: No string object found in given source.\n";
  }
}

StringObject::~StringObject(void)
{
}

unsigned char hexachartochar(char hexa)
{
  switch(hexa)
  {
  case '0':
    return 0;
  case '1':
    return 1;
  case '2':
    return 2;
  case '3':
    return 3;
  case '4':
    return 4;
  case '5':
    return 5;
  case '6':
    return 6;
  case '7':
    return 7;
  case '8':
    return 8;
  case '9':
    return 9;
  case 'A':
  case 'a':
    return 10;
  case 'B':
  case 'b':
    return 11;
  case 'C':
  case 'c':
    return 12;
  case 'D':
  case 'd':
    return 13;
  case 'E':
  case 'e':
    return 14;
  case 'F':
  case 'f':
    return 15;
  }
  return -1;
}

int StringObject::toNum()
{
  if(numVal >= 0)
    return numVal;
  if(this->isHexa)
  {
    int result = 0;
    int len = this->byteStringLen;
    for(int i = 0; i<len; i++)
    {
      result *= 16;
      result += this->getByteString()[i];
    }
    this->numVal = result;
    return result;
  }
  return 0;
}

unsigned char * StringObject::getByteString()
{
  if(!this->isHexa)
    return null;
  if(this->byteString == null)
  {
    this->byteString = new unsigned char[this->byteStringLen];
    char c1,c2;
    for(int i = 0; i<this->byteStringLen; i++)
    {
      c1 = this->string[i*2];
      if(i*2+1 < this->length)
        c2 = this->string[i*2+1];
      else
        c2 = '0';
      this->byteString[i] = hexachartochar(c1)*16+hexachartochar(c2);
    }
  }
  return this->byteString;
}
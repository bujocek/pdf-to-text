#include "StdAfx.h"
#include "StringObject.h"

StringObject::StringObject(char ** endKey, char * source)
{
  this->objectType = PdfObject::TYPE_STRING;
  this->numVal = -1;
  source = StringUtils::skipWhiteSpace(source);
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
      int len = *endKey - source;
      this->string = new char[len + 1];
      strncpy(this->string, source, len);
      this->string[len] = 0;
      this->isHexa = true;
      *endKey += 1;
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
      int len = *endKey - source;
      this->string = new char[len + 1];
      strncpy(this->string, source, len);
      this->string[len] = 0;
      this->isHexa = false;
      *endKey += 1;
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

int hexachartoint(char hexa)
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
    int len = strlen(this->string);
    int pow = 1;
    int result = 0;
    for(int i = len-1; i >= 0; i--)
    {
      result += hexachartoint(this->string[i]) * pow;
      pow = pow*16;
    }
    this->numVal = result;
    return result;
  }
  return 0;
}
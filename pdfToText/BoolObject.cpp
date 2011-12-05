#include "stdafx.h"
#include "BoolObject.h"

BoolObject::BoolObject(bool value)
{
  this->objectType = PdfObject::TYPE_BOOL;
  this->boolValue = value;
}

BoolObject::BoolObject(char ** endKey, char * source)
{
  this->objectType = PdfObject::TYPE_BOOL;
  source = StringUtils::skipWhiteSpace(source);
  this->source = source;
  if(*source == 't' && *(source+1) == 'r' && *(source+2) == 'u' && *(source+3) == 'e')
  {
    this->boolValue = true;
    *endKey = source + 4;
  }
  if(*source == 'f' && *(source+1) == 'a' && *(source+2) == 'l' && *(source+3) == 's' && *(source+4) == 'e')
  {
    this->boolValue = true;
    *endKey = source + 5;
  }
}

BoolObject::~BoolObject(void)
{
}

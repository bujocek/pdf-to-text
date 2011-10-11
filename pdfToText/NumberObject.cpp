#include "StdAfx.h"
#include "NumberObject.h"

NumberObject::NumberObject(float number)
{
  this->objectType = PdfObject::TYPE_NUMBER;
  this->number = number;
}

NumberObject::NumberObject(char ** endKey, char * source)
{
  this->objectType = PdfObject::TYPE_NUMBER;
  this->number = 0;
  int len = 0;
  //TODO: http://code.google.com/p/pdf-to-text/issues/detail?id=6
  sscanf(source, "%f%n", &number, &len);
  *endKey = source + len;
}

NumberObject::~NumberObject(void)
{
}

bool NumberObject::canBeNumber(char * source)
{
  if(((*source) >= '0' && (*source) <= '9') || (*source) == '-' || (*source) == '+')
    return true;
  return false;
}

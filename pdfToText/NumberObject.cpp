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
  //TODO: place here some optimalized method instead of this sscanf shit
  sscanf(source, "%f%n", &number, &len);
  *endKey = source + len;
}

NumberObject::~NumberObject(void)
{
}

bool NumberObject::canBeNumber(char * source)
{
  float n;
  if(((*source) >= '0' && (*source) <= '9') || (*source) == '-' || (*source) == '+')
    return true;
  return false;
}

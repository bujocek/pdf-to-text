#include "stdafx.h"
#include "OperatorObject.h"

OperatorObject::OperatorObject(char ** endKey, char * source)
{
  this->objectType = PdfObject::TYPE_OPERATOR;
  this->source = source;
  //find end
  *endKey = source;
  while(!StringUtils::isDelimiter(**endKey) && !StringUtils::isWhiteSpace(**endKey))
  {
    *endKey = *endKey+1;
  }
  int len = *endKey - source;
  if(len > 0)
  {
    this->name = new char[ len + 1];
    strncpy(this->name, source, len);
    this->name[len] = 0;
  }
  else
  {
    delete[] this->name;
    this->name = null;
  }
}

OperatorObject::~OperatorObject(void)
{
  if(this->name != null)
    delete this->name;
}

bool OperatorObject::canBeOperator(char * source)
{
  if(!StringUtils::isDelimiter(*source) && !StringUtils::isWhiteSpace(*source))
    return true;
  return false;
}

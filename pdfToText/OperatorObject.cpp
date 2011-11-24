#include "StdAfx.h"
#include "OperatorObject.h"

OperatorObject::OperatorObject(char ** endKey, char * source)
{
  this->objectType = PdfObject::TYPE_OPERATOR;
  this->name = new char[4];
  this->source = source;
  if(isalpha(*source))
  {
    this->name[0] = (*source);
    if(isalpha(*(source+1)))
      this->name[1] = *(source+1);
    else
    {
      this->name[1] = 0;
      *endKey = source + 1;
    }
    if(isalpha(*(source+2)))
    {
      this->name[2] = *(source+2);
      this->name[3] = 0;
      *endKey = source + 3;
    }
    else
    {
      this->name[2] = 0;
      *endKey = source + 2;
    }
  }
  else if((*source) == '\'' || (*source) == '"')
  {
    this->name[0] = (*source);
    this->name[1] = 0;
    *endKey = source + 1;
  }
  else
  {
    delete[] this->name;
    this->name = null;
  }
}

OperatorObject::~OperatorObject(void)
{
}

bool OperatorObject::canBeOperator(char * source)
{
  if(isalpha(*source) || (*source) == '"' || (*source) == '\'')
    return true;
  return false;
}

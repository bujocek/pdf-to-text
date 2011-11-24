#include "StdAfx.h"
#include "ArrayObject.h"

ArrayObject::ArrayObject(char ** endKey, char * source)
{
  this->objectType = PdfObject::TYPE_ARRAY;
  source = StringUtils::skipWhiteSpace(source);
  PdfObject * value = null;
  this->source = source;
  this->objectList.clear();
  if(*source == '[')
  {
    source += 1; //move after array bracket
    do
    {
      value = PdfObject::readValue(endKey, source);
      this->objectList.push_back(value);
      source = *endKey;
      source = StringUtils::skipWhiteSpace(source);
    }while(value != null && *source != ']');
    *endKey = source + 1;
  }
  else
  {
    cerr<<"\nArrayObject: No array object found in given source.\n";
  }
}

ArrayObject::~ArrayObject(void)
{
}
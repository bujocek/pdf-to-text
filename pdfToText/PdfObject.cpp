#include "stdafx.h"
#include "PdfObject.h"
#include "StringUtils.h"
#include "NameObject.h"
#include "BoolObject.h"
#include "NumberObject.h"
#include "ArrayObject.h"
#include "StringObject.h"
#include "IndirectObjectReference.h"
#include "OperatorObject.h"


PdfObject::PdfObject(void)
{
  this->objectType = 0;
  this->source = null;
}

PdfObject::~PdfObject(void)
{
}

PdfObject * PdfObject::readValue(char ** endKey, char * source, bool isRefferenceAllowed)
{
  source = StringUtils::skipWhiteSpace(source);
  if(*source == '%')
  {
    //Skip comments
    while(!StringUtils::isEOL(*source))
    {
      source += 1;
    }
    source = StringUtils::skipWhiteSpace(source);
  }
  if(NumberObject::canBeNumber(source)) //can be number or reference
  {
    NumberObject * no = new NumberObject(endKey, source);
    *endKey = StringUtils::skipWhiteSpace(*endKey);
    if(!isRefferenceAllowed || **endKey<'0' || **endKey>'9')
    {
      return no;
    }
    else //it is reference or more numbers
    {
      char * ekOld = *endKey;
      long n2 = strtol(*endKey, endKey, 10);
      *endKey = StringUtils::skipWhiteSpace(*endKey);
      if(**endKey == 'R') //it is refference
      {
        delete no;
        return new IndirectObjectReference(endKey, source);
      }
      else //it is more numbers
      {
        *endKey = ekOld;
        return no;
      }
    }
  }
  if(*source == '/')
    return new NameObject(endKey, source);
  if(*source == '<')
  {
    if(*(source+1) == '<')
      return new DictionaryObject(endKey, source);
    else
    {
      return new StringObject(endKey, source);
    }
  }
  if(*source == '(')
  {
    return new StringObject(endKey, source);
  }
  if(*source == '[')
  {
    return new ArrayObject(endKey, source);
  }
  if(*source == 't' && *(source+1) == 'r' && *(source+2) == 'u' && *(source+3) == 'e')
  {
    *endKey = source + 4;
    return new BoolObject(true);
  }
  if(*source == 'f' && *(source+1) == 'a' && *(source+2) == 'l' && *(source+3) == 's' && *(source+4) == 'e')
  {
    *endKey = source + 5;
    return new BoolObject(false);
  }
  if(*source == 'n' && *(source+1) == 'u' && *(source+2) == 'l' && *(source+3) == 'l')
  {
    *endKey = source + 4;
    return PdfObject::getNullObject();
  }
  if(OperatorObject::canBeOperator(source))
    return new OperatorObject(endKey, source);
  cerr<<"\nPdfObject: Something not implemented - unknown object as pdf object value.\n  Starting char is '"<<*source<<"'\n";
  return null;
}

PdfObject * PdfObject::getNullObject()
{
  PdfObject * po = new PdfObject();
  po->objectType = PdfObject::TYPE_NULL;
  return po;
}

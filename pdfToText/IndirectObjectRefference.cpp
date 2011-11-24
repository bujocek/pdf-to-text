#include "StdAfx.h"
#include "IndirectObjectRefference.h"

IndirectObjectRefference::IndirectObjectRefference(char ** endKey, char * source)
{
  this->objectType = PdfObject::TYPE_INDIRECT_OBJECT_REFFERENCE;
  this->source = source;
  this->objectNumber = 0;
  this->generationNumber = 0;
  int len = 0;
  if(2 == sscanf(source, "%d %d%n", &(this->objectNumber), &(this->generationNumber), &len))
  {
    *endKey = source+len +2; //+2 is to skip " R" after numbers
  }
  else
  {
    cerr<<"\nIndirectObjectRefference: Couldn't read indirect object refference.\n";
  }
}

IndirectObjectRefference::~IndirectObjectRefference(void)
{
}

IndirectObject * IndirectObjectRefference::getIndirectObject()
{
  return (*objectMap)[make_pair(this->objectNumber, this->generationNumber)];
}

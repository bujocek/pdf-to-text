#include "stdafx.h"
#include "IndirectObjectReference.h"

IndirectObjectReference::IndirectObjectReference(char ** endKey, char * source)
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
    cerr<<"\nIndirectObjectReference: Couldn't read indirect object refference.\n";
  }
}

IndirectObjectReference::~IndirectObjectReference(void)
{
}

IndirectObject * IndirectObjectReference::getIndirectObject()
{
  return (*objectMap)[make_pair(this->objectNumber, this->generationNumber)];
}

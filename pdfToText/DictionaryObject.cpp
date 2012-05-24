#include "stdafx.h"
#include "DictionaryObject.h"
#include "StringUtils.h"
#include "string.h"
#include "NameObject.h"
#include "IndirectObjectReference.h"

using namespace std;


DictionaryObject::DictionaryObject(char ** endKey, char * source)
{
  this->objectType = PdfObject::TYPE_DICTIONARY;
	this->dictionaryString = NULL;
  this->dictionaryStringLength = 0;

  source = StringUtils::skipWhiteSpace(source);
  this->source = source;
  if(*source == '<' && *(source+1) == '<')
	{
    char * actualPos = source;
    char * ek = null;
    NameObject * key = null;
    PdfObject * value = null;
    do
    {
      key = NameObject::getNameObject(&ek, actualPos);
      actualPos = ek;
      if(key != null)
      {
        value = PdfObject::readValue(&ek, actualPos);
        actualPos = ek;
        dictionaryMap[key->name] = value;
      }
      actualPos = StringUtils::skipWhiteSpace(actualPos);
    }while(key != null && value != null && !(*actualPos == '>' && *(actualPos+1) == '>'));
    if(*actualPos == '>' && *(actualPos+1) == '>')
    {
      actualPos += 2;
    }
    else
    {
      cerr << "\nDictionaryObject: Problem with determining end of the dictionary.\n";
    }
    if(endKey != null)
      *endKey = actualPos;
  }
  else
  {
    cerr<<"\nDictionaryObject.cpp: Couldn't find begining of dictionary.\n";
  }
}

DictionaryObject::~DictionaryObject(void)
{
  //TODO: http://code.google.com/p/pdf-to-text/issues/detail?id=8
}

PdfObject * DictionaryObject::getObject(char * mapKey, bool followToFirstObject)
{
  PdfObject * po = this->dictionaryMap[mapKey];
  if(po == null)
    return null;
  else if(po->objectType == PdfObject::TYPE_INDIRECT_OBJECT_REFFERENCE)
  {
    IndirectObject * io = ((IndirectObjectReference*) po)->getIndirectObject();
    if(followToFirstObject) 
    {
      if(io != null)
      {
        return io->getFirstObject();
      }
    }
    else
      return io;
    
  }
  else
    return po;
  return null;
}

//DEPRECATED
DictionaryObject::DictionaryObject(char * startString, int length, char ** endKey)
{
  this->objectType = PdfObject::TYPE_DICTIONARY;
	this->dictionaryString = NULL;
  this->dictionaryStringLength = 0;
  if(length == -1)
    length = strlen(startString);
	//find dictionary
  char * dictionaryStart = StringUtils::skipWhiteSpace(startString);
  char * dictionaryEnd = NULL;
	if(*dictionaryStart == '<' && *(dictionaryStart+1) == '<')
	{
    int restLength = length - (dictionaryStart - startString);
    int deep = 1;
    for(int i = 2; i < restLength-1; i++)
    {
      if(dictionaryStart[i] == '<' && dictionaryStart[i+1] == '<')
      {
        deep++;
        i++;
      }
      if(dictionaryStart[i] == '>' && dictionaryStart[i+1] == '>')
      {
        deep--;
        i++;
        if(deep == 0)
        {
          dictionaryEnd = dictionaryStart + i + 1;
          break;
        }
      }
    }
    if(dictionaryEnd == NULL)
    {
      cerr<<"\nDictionaryObject.cpp: Couldn't find end of dictionary.\n";
      return;
    }
    if(endKey != null)
      *endKey = dictionaryEnd;
    this->dictionaryStringLength = dictionaryEnd - dictionaryStart;
    this->dictionaryString = new char[this->dictionaryStringLength+1];
    strncpy( this->dictionaryString, dictionaryStart, this->dictionaryStringLength );
    this->dictionaryString[this->dictionaryStringLength] = 0;
    //find keys and values
    this->processDictionaryString();
	}
  else
  {
    cerr<<"\nDictionaryObject.cpp: Couldn't find begining of dictionary.\n";
  }
}

//DEPRECATED
void DictionaryObject::processDictionaryString()
{
  char * actualPos = this->dictionaryString;
  char * endKey = null;
  NameObject * key = null;
  PdfObject * value = null;
  do
  {
    key = NameObject::getNameObject(&endKey, actualPos);
    actualPos = endKey;
    if(key != null)
    {
      value = PdfObject::readValue(&endKey, actualPos);
      actualPos = endKey;
      dictionaryMap[key->name] = value;
    }
    actualPos = StringUtils::skipWhiteSpace(actualPos, this->dictionaryStringLength-(actualPos-this->dictionaryString));
  }while(key != null && value != null && !(*actualPos == '>' && *(actualPos+1) == '>'));
}
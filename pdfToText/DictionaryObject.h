#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include "PdfObject.h"

struct classcomp {
  bool operator() (const char * s1, const char * s2) const
  {return strcmp(s1,s2)<0;}
};

class DictionaryObject : public PdfObject
{
private:
  /**
  this method is DEPRECATED. No more needed and used.
  */
  void processDictionaryString(void);
public:
  /**
  this constructor is DEPRECATED. No more needed and used.
  */
	DictionaryObject(char * startString, int length = -1, char ** endKey = null);

  DictionaryObject(char ** endKey, char * source);
	~DictionaryObject(void);
  std::map <char*, PdfObject*,classcomp> dictionaryMap;
  char * dictionaryString;
  long dictionaryStringLength;

  /**
  gets object from dictionaryMap specified by mapKey parameter and if it is indirect object refference 
  than follows reference and returns the indirect object
  otherwise returns object from map
  */
  PdfObject * getObject(char * mapKey);
};
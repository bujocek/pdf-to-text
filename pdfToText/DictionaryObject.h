#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include "PdfObject.h"

/**
Class for representing Dictionary PDF object.<br>
Creates map from dictionary keys to dictionary values. Keys are strings (char*) and values of the dictionary are PdfObjects
*/
class DictionaryObject : public PdfObject
{
private:
  /**
  Simple struct for comparing strings in map
  */
  struct classcomp {
    bool operator() (const char * s1, const char * s2) const
    {return strcmp(s1,s2)<0;}
  };

  /**
  this method is DEPRECATED. No more needed and used.
  */
  void processDictionaryString(void);
  char * dictionaryString;
  long dictionaryStringLength;
public:
  /**
  This constructor is deprecated. No more needed and used.
  \deprecated
  */
	DictionaryObject(char * startString, int length = -1, char ** endKey = null);
  
  /**
  A Constructor. <br>
  Constructs DictionaryObject from <code>source</code>.
  \param endKey Reference where will be stored pointer after dictionary in source.
  \param source Source string the dictionary is read from.
  */
  DictionaryObject(char ** endKey, char * source);

  /**
  A Destructor
  */
	~DictionaryObject(void);

  /**
  Main DictionaryObject property. Maps dictionary keys to values.<br>
  Here are stored all dictionary entries. Map is indexed with dictionary keys and stores PdfObjects.
  */
  std::map <char*, PdfObject*,classcomp> dictionaryMap;

  /**
  Gets object from dictionaryMap specified by mapKey parameter and if it is an IndirectObjectReference 
  than follows reference and returns the IndirectObject the reference is pointing to.
  Otherwise returns PdfObject from map.
  */
  PdfObject * getObject(char * mapKey);
};
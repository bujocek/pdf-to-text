#pragma once
#include "PdfObject.h"
#include <list>

/**
A simple class that represents PDF Array object. <br>
It uses <code>vector</code> to store items in array. Items in PDF array are other PDF objects
*/
class ArrayObject :
  public PdfObject
{
public:
  
  /**
  List of objects that the array contains.
  */
  std::vector<PdfObject*> objectList;
  
  /**
  A Constructor <br>
  Reads array from <code>source</code> and stores pointer after array to <code>*endKey</code>.
  \param endKey Pointer to the end of the array in <code>source</code> will be stored to this parameter.
  \param source Source string the array will be read from.
  */
  ArrayObject(char ** endKey, char * source);
  
  /**
  A Destructor
  */
  ~ArrayObject(void);
};

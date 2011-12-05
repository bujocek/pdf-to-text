#pragma once
#include "pdfobject.h"

/**
Simple class for representing Name PDF Objects. (syntax: /something)<br>
*/
class NameObject :
  public PdfObject
{
public:
  /**
  String name of the object
  */
  char * name;
  
  /**
  This method provides almost same functionality as constructor with one difference. 
  It searches for the first occurence of some name in <code>source</code>
  \return New name object found in source
  */
  static NameObject * getNameObject(char ** endKey, char * source);

  /**
  Constructor.
  reads name object (i.e. /somename) and return new instance containing name
  \param source Source string the name will be read from.
  \param endKey Pointer to the end of the name in <code>source</code> will be stored to this parameter.
  */
  NameObject(char ** endKey, char * source);
  
  /**
  Destructor
  */
  ~NameObject(void);
};

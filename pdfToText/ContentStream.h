#pragma once
#include "CommonHeader.h"
#include "pdfobject.h"
#include "IndirectObject.h"
#include "PageTreeNode.h"
#include "IndirectObjectRefference.h"
#include "indirectObject.h"
#include "OperatorObject.h"
#include "DictionaryObject.h"
#include "StringObject.h"
#include "wchar.h"
#include "ToUnicodeCMap.h"

class ContentStream :
  public PdfObject
{
private:
  DictionaryObject * currentFont;
  ToUnicodeCMap * currentCMap;
  wchar_t * textFromContent;

  
  ///**
  //#DEPRECATED#
  //An old version of getText - not used any more
  //*/
  //char * getText2();

  ///**
  //#DEPRECATED#
  //An old version of getting text method - not used any more
  //retrieves text from textObject found in this pdf object
  //*/
  //char * processTextObject(char * textObject);
	
	/**
	converts the hexa string in source to CString
	creates new string and returns it
	*/
	char * hexaStringToCString(char * source, int length);

public:
  IndirectObject * indirectObject;
  PageTreeNode * page;
  ContentStream(IndirectObject * io, PageTreeNode * page);
  ~ContentStream(void);
  
  /**
	tries to retrieve text from Pdf object if there is some
	*/
	wchar_t * getText();
  
  /**
  method that processes string object and returns text
  */
  wchar_t * processStringObject(StringObject * stringObject);
};

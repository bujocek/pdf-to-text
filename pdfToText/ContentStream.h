#pragma once
#include "CommonHeader.h"
#include "PdfObject.h"
#include "IndirectObject.h"
#include "PageTreeNode.h"
#include "IndirectObjectReference.h"
#include "IndirectObject.h"
#include "OperatorObject.h"
#include "DictionaryObject.h"
#include "StringObject.h"
#include "wchar.h"
#include "ToUnicodeCMap.h"

/**
Class for representing ContentStream PDF object. <br>
Content Stream in pdf is always Indirect object, that's why is this class constructed from indirect object.<br>
The class provides functionality for retrieving text from the content and needs reference to it's page to get resources from it.
*/
class ContentStream :
  public PdfObject
{
private:
  DictionaryObject * currentFont;
  ToUnicodeCMap * currentCMap;
  wchar_t * textFromContent;
  
  /*
  Method that processes StringObject and returns text.
  */
  wchar_t * processStringObject(StringObject * stringObject);
  wchar_t * convertStringWithToUnicode(StringObject * string, ToUnicodeCMap * cmap);
  wchar_t * convertWithBaseEncoding(StringObject * string, const char * encoding[]);
  void convertUTFtoWchar (char * utf16be,  size_t * wcharlen, wchar_t * wchar);

public:

  /**
  Reference to IndirectObject the ContentStream is created from.
  */
  IndirectObject * indirectObject;

  /**
  Reference to page (PageTreeNode) the content belogs to. <br>
  Page is needed because of resources.
  */
  PageTreeNode * page;

  /**
  Map of PdfObjects in the stream. <br>
  When processing stream there are created PdfObjects for every object in unencoded stream. 
  It is needed for processing operators and is public to be accesible from other ContentStreams when processing operation whose operands are
  still in previous stream.
  */
  map<int,PdfObject*> streamObjectMap;

  /**
  A Constructor<br>
  Constructs ContentStream from IndirectObject and PageTreeNode.
  \param io IndirectObject the content stream is in.
  \param page Page (PageTreeNode) the content belongs to.
  */
  ContentStream(IndirectObject * io, PageTreeNode * page);

  /**
  A Destructor
  */
  ~ContentStream(void);
  
  /**
	Tries to retrieve text from the content if it is possible and there is some.<br>
  \param prevStream Previous stream on same page if there are more of them. Needed when operator's operands are in previous stream and operator is in this stream.
	*/
	wchar_t * getText(ContentStream * prevStream = null);



  //private:
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
	
	///**
	//converts the hexa string in source to CString
	//creates new string and returns it
	//*/
	//char * hexaStringToCString(char * source, int length);
};

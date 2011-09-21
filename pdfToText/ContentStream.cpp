#include "StdAfx.h"
#include "ContentStream.h"

ContentStream::ContentStream(IndirectObject * io, PageTreeNode * page)
{
  this->objectType = PdfObject::TYPE_CONTENT_STREAM;
  this->indirectObject = io;
  if(io == null)
    cerr << "\nContentStream: Missing indirect object - Can't create Content Stream properly.\n";
  this->page = page;
  this->textFromContent = null;
  this->currentFont = null;
  if(io != null)
    this->indirectObject->processAsStream();
}

ContentStream::~ContentStream(void)
{
}

wchar_t * ContentStream::getText()
{
  if(this->textFromContent != null)
    return this->textFromContent;
  DictionaryObject * fonts = this->page->getFonts();
  this->currentFont = null;
  wchar_t * result = new wchar_t[this->indirectObject->unencodedStreamSize * sizeof(wchar_t)]; //assuming that the result string will not be bigger than whole stream
  result[0] = 0;
  char * usingString = this->indirectObject->unencodedStream;
	int usingStringLen = this->indirectObject->unencodedStreamSize;

  if(logEnabled)
    clog << "\nParsing content stream of pdf object " << this->indirectObject->objectNumber <<
      " " << this->indirectObject->generationNumber << " R.";

  char * source = usingString;
  char ** endKey = &source;
  map<int,PdfObject*> streamObjectMap;
  source = StringUtils::skipWhiteSpace(source);
  int index = 0;
  PdfObject * value;
  do
  {
    value = PdfObject::readValue(endKey, source, false, usingStringLen - (source-usingString));
    streamObjectMap[index] = value;
  
    if(value->objectType == PdfObject::TYPE_OPERATOR)
    {
      OperatorObject * operatorObject = (OperatorObject*)value;
      if(strcmp(operatorObject->name,"Tf") == 0)
      {
        if(streamObjectMap[index-2]->objectType == PdfObject::TYPE_NAME)
        {
          this->currentFont = null;
          if(fonts != null)
          {
            PdfObject * font = fonts->getObject(((NameObject*)streamObjectMap[index-2])->name);
            if(font != null && font->objectType == PdfObject::TYPE_DICTIONARY)
              this->currentFont = (DictionaryObject*)font;
            if(font != null && font->objectType == PdfObject::TYPE_INDIRECT_OBJECT)
              this->currentFont = (DictionaryObject*)((IndirectObject*)font)->getFirstObject();
            if(this->currentFont == null)
              cerr << "\nCouldn't get font specified by name before 'Tf' oprerator\n";
            else //try to get /ToUnicode map
            {
              PdfObject * tucm = null;
              this->currentCMap = null;
              tucm = this->currentFont->getObject("/ToUnicode");
              if(tucm != null && tucm->objectType == PdfObject::TYPE_INDIRECT_OBJECT)
              {
                this->currentCMap = new ToUnicodeCMap((IndirectObject*)tucm);
              }
            }
          }
          else
            cerr << "\nCouldn't use font specified by name before 'Tf' oprerator because of missing fonts.\n";
        }
        else
          cerr << "\nCouldn't find font name before 'Tf' oprerator\n";
      }
      else if(strcmp(operatorObject->name,"Tj") == 0)
      {
        wcscat(result, this->processStringObject((StringObject*) streamObjectMap[index - 1]));
      }
      else if(strcmp(operatorObject->name,"'") == 0 || strcmp(operatorObject->name,"\"") == 0)
      {
        //TODO: http://code.google.com/p/pdf-to-text/issues/detail?id=12
        wcscat(result, L"\n");
        wcscat(result, this->processStringObject((StringObject*) streamObjectMap[index - 1]));
      }
      else if(strcmp(operatorObject->name,"TJ") == 0)
      {
        if(streamObjectMap[index-1]->objectType == PdfObject::TYPE_ARRAY)
        {
          list<PdfObject*> objectList = ((ArrayObject*) streamObjectMap[index-1])->objectList;
          list<PdfObject *>::iterator olIterator = objectList.begin();
          for(;olIterator != objectList.end(); olIterator++)
          {
            if((*olIterator)->objectType == PdfObject::TYPE_STRING)
              wcscat(result, this->processStringObject((StringObject*) (*olIterator)));
          }
        }
        else
        {
          cerr << "\nContentStream: Couldn't find an array before 'TJ' operator.\n";
        }
      }
      else if(strcmp(operatorObject->name,"Td") == 0 || strcmp(operatorObject->name,"TD") == 0 || strcmp(operatorObject->name,"TD") == 0)
      {
        //TODO: http://code.google.com/p/pdf-to-text/issues/detail?id=12
        wcscat(result, L"\n");
      }
    }

    index++;
    source = *endKey;
    source = StringUtils::skipWhiteSpace(source);
  }while(source-usingString < usingStringLen && value != null);
  
  this->textFromContent = result;
  if(logEnabled)
    clog << "\nStream parsed.";
  
  //TODO: http://code.google.com/p/pdf-to-text/issues/detail?id=11
  return result;
}

wchar_t * charToWchar(char * source)
{
  size_t origsize = strlen(source) + 1;
  const size_t newsize = origsize * sizeof(wchar_t);
  size_t convertedChars = 0;
  wchar_t * wcstring = new wchar_t[newsize];
  mbstowcs_s(&convertedChars, wcstring, origsize, source, _TRUNCATE);
  return wcstring;
}

wchar_t * convertHexaString(StringObject * string, ToUnicodeCMap * cmap)
{
  //TODO: http://code.google.com/p/pdf-to-text/issues/detail?id=14<<<<<<<<<<<
  char * charCode = null;
  
  //get charcode
  //map charcode
  //convert from UTF-16BE to wchar (unicode)
  //push to result
  return charToWchar(string);
}

wchar_t * ContentStream::processStringObject(StringObject * stringObject)
{
  if(stringObject == null)
  {
    cerr << "\nContentStream: Can't proces null object in processStringObject method. Something went wrong.\n";
    return null;
  }
  
  if(stringObject->isHexa)
  {
    if(this->currentCMap != null)
    {
      return convertHexaString(stringObject, this->currentCMap);
    }
    else
    {
      cerr << "\nContentStream: No ToUnicode map found for decoding string.\n";
      return L"|-- Unknown string --|";
    }
  }
  else
  {
    return charToWchar(stringObject->string);
  }
  return null;
}

//char * ContentStream::getText2()
//{
//  char * usingString;
//	int usingStringLen;
//	if(this->unencodedStreamSize > 0)
//	{
//		usingString = this->unencodedStream;
//		usingStringLen = this->unencodedStreamSize;
//	}
//	else if(this->indirectObject->objectStringSize > 0)
//	{
//		usingString = this->indirectObject->objectString;
//		usingStringLen = this->indirectObject->objectStringSize;
//	}
//	else
//		return null;
//
//	//search for BT and ET
//	char * bt = null;
//	char * et = null;
//	char * str;
//	queue<char *, deque<char*>> strings;
//	long resultSize = 0;
//	do
//	{
//		bt = StringUtils::strStrModified(usingString, "BT", usingStringLen);
//		et = null;
//		if(bt != null)
//			et = StringUtils::strStrModified(bt, "ET", usingStringLen - (bt - usingString));
//		if(et != null)
//		{
//			et += 2;
//			str = new char[et-bt+1];
//			strncpy(str,bt,et-bt);
//			str[et-bt] = 0;
//			resultSize += et-bt;
//			strings.push(str);
//		}
//		usingStringLen = usingString - et;
//		usingString = et;
//	} while(bt != null);
//	
//	if(logEnabled && strings.size() > 0)
//		clog << "\nFound text object(s) in pdf object " << this->indirectObject->objectNumber << ".\nExtracting...";
//
//	char * resultString = new char[resultSize+1];
//	resultString[0] = 0;
//	while(strings.size() > 0)
//	{
//		str = this->processTextObject(strings.front());
//		strcat(resultString, str);
//		delete[] str;
//		strings.pop();
//	}
//	if(logEnabled && resultSize > 0)
//		clog << "\nSomething extracted.";
//	resultString[resultSize] = 0;
//	return resultString;
//}

//char * ContentStream::processTextObject(char * textObject)
//{
//  int len = strlen(textObject);
//	int i =0;
//	char * begin = null;
//	char * end;
//	char * result = new char[len];
//	result[0] = 0;
//	bool hexa = false;
//	int deep = 0;
//
//	for(;i<len; i++)
//	{
//    //New line operators adding
//    if(textObject[i] == 'T' && deep == 0)
//    {
//      switch(textObject[i+1])
//      {
//      case 'd':
//      case 'D':
//      case '*':
//        strncat(result,"\n",1);
//      }
//    }
//    if((textObject[i] == '\'' || textObject[i] == '"') && deep == 0)
//    {
//      strncat(result,"\n",1);
//    }
//
//		if(textObject[i] == '(')
//		{
//			if(deep>0)
//			{
//				deep++;
//			}
//			if(deep==0)
//			{
//				hexa = false;
//				deep++;
//				begin = &textObject[i];
//			}
//		}
//		if(textObject[i] == ')' && textObject[i-1] != '\\')
//		{
//			if(deep>0)
//			{
//				deep--;
//			}
//			if(deep==0)
//			{
//				end = &textObject[i];
//        if(begin == null)
//        {
//          cerr<<"\nContentStream: Problem in finding strings. Probably not text object.\n";
//          return result;
//        }
//				//TODO: process string between begin and end
//				strncat(result, begin+1, end-begin-1);
//			}
//		}
//		if(textObject[i] == '<')
//		{
//			if(deep==0)
//			{
//				hexa = true;
//				deep++;
//				begin = &textObject[i];
//			}
//		}
//		if(textObject[i] == '>')
//		{
//			if(deep==1 && hexa)
//			{
//				deep--;
//				end = &textObject[i];
//				//TODO: use font maps for determining chars from string
//				char * convertedString = hexaStringToCString(begin+1, end - begin -1);
//				strcat(result, convertedString);
//				delete[] convertedString;
//				//strncat(result, begin, end-begin+1);
//			}
//		}
//	}
//	return result;
//}

/**
helper function for hexaStringToCString
*/
char hexaDoubleCharToChar(char h1, char h2)
{
	char hexa[3];
	hexa[0] = h1;
	hexa[1] = h2;
	hexa[2] = 0;
	int charCode = strtol(hexa, null, 16);
	return charCode;
}

char * ContentStream::hexaStringToCString(char * source, int length)
{
	char * newString = new char[length/2+1];
	for(int i=0; i<length; i+=2)
	{
		if(i+1 >= length)
			newString[i/2] = hexaDoubleCharToChar(source[i], '0'); //add '0' char at the end if it is not even count of chars
		else
			newString[i/2] = hexaDoubleCharToChar(source[i], source[i+1]);
	}
	newString[length/2] = 0;
	return newString;
}

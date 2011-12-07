#include "stdafx.h"
#include "ContentStream.h"

#ifdef _WIN32
#define NEWLINE L"\r\n" // windows
#elif __APPLE__
#define NEWLINE L"\r" // mac
#else
#define NEWLINE L"\n" //u**x
#endif


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

wchar_t * ContentStream::getText( ContentStream * prevStream)
{
  if(this->textFromContent != null)
    return this->textFromContent;
  if(this->indirectObject->unencodedStream == null)
  {
    cerr << "\nContentStream: Stream wasn't processed - nothing to extract text from.\n";
    return null;
  }
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
  source = StringUtils::skipWhiteSpace(source);
  int index = 0;
  PdfObject * value;
  do
  {
    value = PdfObject::readValue(endKey, source, false);
    streamObjectMap[index] = value;
  
    if(value->objectType == PdfObject::TYPE_OPERATOR)
    {
      OperatorObject * operatorObject = (OperatorObject*)value;
      if(strcmp(operatorObject->name,"Tf") == 0)
      {
        PdfObject * nameObject;
        if(index >= 2)
          nameObject = streamObjectMap[index-2];
        else if(prevStream != null && prevStream->streamObjectMap.size() >= 2-index)
        {
          nameObject = prevStream->streamObjectMap[prevStream->streamObjectMap.size() - 3 + index];
        }
        else
        {
          cerr << "\nContentStream: Problem with determining operands between two content streams.\n";
          break;
        }
        if(nameObject != null && nameObject->objectType == PdfObject::TYPE_NAME)
        {
          this->currentFont = null;
          if(fonts != null)
          {
            PdfObject * font = fonts->getObject(((NameObject*)nameObject)->name);
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
              char * tucmName = "/ToUnicode";
              tucm = this->currentFont->getObject(tucmName);
              if(tucm != null && tucm->objectType == PdfObject::TYPE_INDIRECT_OBJECT)
              {
                this->currentCMap = new ToUnicodeCMap((IndirectObject*)tucm);
              }
            }
          }
          else
          {
            cerr << "\nContentStream: Couldn't use font specified by name before 'Tf' oprerator because of missing fonts.\n";
            break;
          }
        }
        else
        {
          cerr << "\nContentStream: Couldn't find font name before 'Tf' oprerator\n";
          break;
        }
      }
      else if(strcmp(operatorObject->name,"Tj") == 0)
      {
        PdfObject * stringObject;
        if(index >= 1)
          stringObject = streamObjectMap[index-1];
        else if(prevStream != null && prevStream->streamObjectMap.size() >= 1-index)
        {
          stringObject = prevStream->streamObjectMap[prevStream->streamObjectMap.size() - 2 + index];
        }
        else
        {
          cerr << "\nContentStream: Problem with determining operands between two content streams.\n";
          break;
        }
        if(stringObject == null)
        {
          cerr << "\nContentStream: Couldn't find string before Tj operator.\n";
          break;
        }
        wcscat(result, this->processStringObject((StringObject*) stringObject));
      }
      else if(strcmp(operatorObject->name,"'") == 0 || strcmp(operatorObject->name,"\"") == 0)
      {
        wcscat(result, NEWLINE);
        PdfObject * stringObject;
        if(index >= 1)
          stringObject = streamObjectMap[index-1];
        else if(prevStream != null && prevStream->streamObjectMap.size() >= 1-index)
        {
          stringObject = prevStream->streamObjectMap[prevStream->streamObjectMap.size() - 2 + index];
        }
        else
        {
          cerr << "\nContentStream: Problem with determining operands between two content streams.\n";
          break;
        }
        if(stringObject == null)
        {
          cerr << "\nContentStream: Couldn't find string before ' or \" operator.\n";
          break;
        }
        wcscat(result, this->processStringObject((StringObject*) stringObject));
      }
      else if(strcmp(operatorObject->name,"TJ") == 0)
      {
        PdfObject * arrayObject;
        if(index >= 1)
          arrayObject = streamObjectMap[index-1];
        else if(prevStream != null && prevStream->streamObjectMap.size() >= 1-index)
        {
          arrayObject = prevStream->streamObjectMap[prevStream->streamObjectMap.size() - 2 + index];
        }
        else
        {
          cerr << "\nContentStream: Problem with determining operands between two content streams.\n";
          break;
        }
        if(arrayObject != null && arrayObject->objectType == PdfObject::TYPE_ARRAY)
        {
          vector<PdfObject*> objectList = ((ArrayObject*) arrayObject)->objectList;
          vector<PdfObject *>::iterator olIterator = objectList.begin();
          for(;olIterator != objectList.end(); olIterator++)
          {
            if((*olIterator)->objectType == PdfObject::TYPE_STRING)
              wcscat(result, this->processStringObject((StringObject*) (*olIterator)));
          }
        }
        else
        {
          cerr << "\nContentStream: Couldn't find an array before 'TJ' operator.\n";
          break;
        }
      }
      else if(strcmp(operatorObject->name,"Td") == 0 || strcmp(operatorObject->name,"TD") == 0 || strcmp(operatorObject->name,"TD") == 0)
      {
        wcscat(result, NEWLINE);
      }
    }

    index++;
    source = *endKey;
    source = StringUtils::skipWhiteSpace(source);
  }while(source-usingString < usingStringLen && value != null);
  
  this->textFromContent = result;
  if(logEnabled)
    clog << "\nStream parsed.";
  return result;
}

wchar_t * charToWchar(char * source)
{
  size_t origsize = strlen(source) + 1;
  const size_t newsize = origsize * sizeof(wchar_t);
  wchar_t * wcstring = new wchar_t[newsize];
  mbstowcs(wcstring, source, origsize);
  return wcstring;
}

wchar_t * ContentStream::convertHexaString(StringObject * string, ToUnicodeCMap * cmap)
{
  int maxCharSize = 4;
  unsigned char * charCode = new unsigned char[maxCharSize];
  wchar_t * result = null;
  wchar_t* newbytes = new wchar_t[string->byteStringLen];
  int pos,i,currentLen;
  currentLen = 0;
  i=0;
  for (pos=0;pos<string->byteStringLen;pos++)
  {
    //get charcode
    charCode[i] = string->getByteString()[pos];
    i++;
    if(cmap->isCharCode(charCode, i))
    {
      //map charcode and return string object
      StringObject * stringCharCode = cmap->getUTFChar(charCode, i);
      //convert from UTF-16BE to wchar (unicode)
      if(stringCharCode != null)
      {
    	unsigned char * utf16be = stringCharCode->getByteString();
        
    	size_t iconv_value;
	    size_t len =  stringCharCode->byteStringLen;
	    size_t length = len*sizeof(wchar_t);
	    size_t ls = length;
	    if (!len) 
      {
		    cerr << "\nContentStream:Couldnt convert utf16be char to wchar\n";
		    continue;
	    };
      wchar_t * outbuf = new wchar_t(len);
	    char * outbufch = (char*)outbuf;
	    iconv_value = iconv (conv_desc, (const char**) &utf16be,
			    & len, &outbufch , &length);
	    /* Handle failures. */
	    if (iconv_value == (size_t) -1)
	    {
		    cerr << "\nContentStream:Couldnt convert utf16be char to wchar\n";
	    }
	    memcpy(&newbytes[currentLen], outbuf, ls-length);
	    currentLen += (ls-length)/sizeof(wchar_t);
      }
      i=0;
    }
    if(i>= maxCharSize)
    {
      cerr << "\nContentStream: Problem with determining char codes in string.\n";
      return L"";
    }
  }
  //push to result
  result = new wchar_t[currentLen+1];
  wcsncpy(result, newbytes, currentLen);
  delete[] newbytes;
  result[currentLen] = L'\0';  
  return result;
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

////////////
//OLD CODE//
////////////

///**
//helper function for hexaStringToCString
//*/
//char hexaDoubleCharToChar(char h1, char h2)
//{
//	char hexa[3];
//	hexa[0] = h1;
//	hexa[1] = h2;
//	hexa[2] = 0;
//	int charCode = strtol(hexa, null, 16);
//	return charCode;
//}

//char * ContentStream::hexaStringToCString(char * source, int length)
//{
//	char * newString = new char[length/2+1];
//	for(int i=0; i<length; i+=2)
//	{
//		if(i+1 >= length)
//			newString[i/2] = hexaDoubleCharToChar(source[i], '0'); //add '0' char at the end if it is not even count of chars
//		else
//			newString[i/2] = hexaDoubleCharToChar(source[i], source[i+1]);
//	}
//	newString[length/2] = 0;
//	return newString;
//}

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

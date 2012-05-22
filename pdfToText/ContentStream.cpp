#include "stdafx.h"
#include "ContentStream.h"
#include "EncodingTable.h"

#ifdef _WIN32 || _WIN64
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
  this->currentCMap = null;
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
  if(this->indirectObject->unencodedStreamSize == 0)
  {
    clog << "\nContentStream: An empty content stream found. Skipping this object.";
    return L"";
  }
  DictionaryObject * fonts = this->page->getFonts();
  if(prevStream != null)
  {
    this->currentFont = prevStream->currentFont;
    this->currentCMap = prevStream->currentCMap;
  }
  else
  {
    this->currentFont = null;
    this->currentCMap = null;
  }
  wchar_t * result = new wchar_t[this->indirectObject->unencodedStreamSize * sizeof(wchar_t)+1]; //assuming that the result string will not be bigger than whole stream
  result[0] = L'\0';
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
          nameObject = prevStream->streamObjectMap[prevStream->streamObjectMap.size() - 2 + index];
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
            PdfObject * font = fonts->getObject(((NameObject*)nameObject)->name, true);
            if(font != null && font->objectType == PdfObject::TYPE_DICTIONARY)
            {
              this->currentFont = (DictionaryObject*)font;
            }
            if(this->currentFont == null)
            {
              cerr << "\nCouldn't get font specified by name before 'Tf' oprerator\n";
            }
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
        else if(prevStream != null && prevStream->streamObjectMap.size() >= 1)
        {
          stringObject = prevStream->streamObjectMap[prevStream->streamObjectMap.size() - 1 + index];
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
        wcscat_s(result, this->indirectObject->unencodedStreamSize, this->processStringObject((StringObject*) stringObject));
      }
      else if(strcmp(operatorObject->name,"'") == 0 || strcmp(operatorObject->name,"\"") == 0)
      {
        wcscat_s(result, this->indirectObject->unencodedStreamSize, NEWLINE);
        PdfObject * stringObject;
        if(index >= 1)
          stringObject = streamObjectMap[index-1];
        else if(prevStream != null && prevStream->streamObjectMap.size() >= 1)
        {
          stringObject = prevStream->streamObjectMap[prevStream->streamObjectMap.size() - 1 + index];
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
        wcscat_s(result, this->indirectObject->unencodedStreamSize, this->processStringObject((StringObject*) stringObject));
      }
      else if(strcmp(operatorObject->name,"TJ") == 0)
      {
        PdfObject * arrayObject;
        if(index >= 1)
          arrayObject = streamObjectMap[index-1];
        else if(prevStream != null && prevStream->streamObjectMap.size() >= 1)
        {
          arrayObject = prevStream->streamObjectMap[prevStream->streamObjectMap.size() - 1 + index];
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
              wcscat_s(result, this->indirectObject->unencodedStreamSize, this->processStringObject((StringObject*) (*olIterator)));
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
        wcscat_s(result, this->indirectObject->unencodedStreamSize, NEWLINE);
      }
    }

    index++;
    source = *endKey;
    source = StringUtils::skipWhiteSpace(source);
  }while(source-usingString < usingStringLen && value != null);
  
  this->textFromContent = result;
  if(logEnabled)
    clog << "\nStream parsed.";
  result[this->indirectObject->unencodedStreamSize * sizeof(wchar_t)]=L'\0';
  return result;
}

wchar_t * charToWchar(char * source, int len = -1)
{
  size_t origsize;
  if(len < 0)
    origsize = strlen(source);
  else
    origsize = len;
  wchar_t * wcstring = new wchar_t[origsize+1];
  mbstowcs(wcstring, source, origsize);
  wcstring[origsize] = L'\0';
  return wcstring;
}

int convertUTFtoWchar(StringObject * stringCharCode, wchar_t * newbytes, int freeSpace)
{
  //TODO: move converting code from convertStringWithToUnicode to this method
  if(stringCharCode == null)
  {
    cerr << "\nContentStream: Can't convert null string object to wchar.\n";
    return -1;
  }
  else
  {
	  unsigned char * utf16be = stringCharCode->getByteString();
	  size_t iconv_value;
    size_t len =  stringCharCode->getByteStringLen();
    size_t length = len*sizeof(wchar_t);
    size_t ls = length;
    if (!len) 
    {
      cerr << "\nContentStream:Couldn't convert utf16be char to wchar\n";
      return -1;
    };
    wchar_t * outbuf = new wchar_t[len+1];
    outbuf[stringCharCode->getByteStringLen()] = 0;
    char * outbufch = (char*)outbuf;
#ifdef _WIN32 || _WIN64
    iconv_value = iconv (conv_desc, (const char**) &utf16be,
	      &len, &outbufch , &length);
#else
  	  iconv_value = iconv (conv_desc, (char**) &utf16be,
	      &len, &outbufch , &length);
#endif
    /* Handle failures. */
    if (iconv_value == (size_t) -1)
    {
      cerr << "\nContentStream: Couldn't convert utf16be char to wchar\n";
      return -1;
    }

    int processed = (ls-length)/sizeof(wchar_t);
    if(processed > freeSpace)
    {
      cerr<<"\nContentStream: Unicode char doesn't fit to result array. Array probably too smal.\n";
      processed = freeSpace;
    }
    memcpy(newbytes, outbuf, (ls-length)*sizeof(wchar_t));
    return processed;
  }
}

wchar_t * ContentStream::convertStringWithToUnicode(StringObject * string, ToUnicodeCMap * cmap)
{
  int maxCharSize = 4;
  unsigned char * charCode = new unsigned char[maxCharSize];
  wchar_t * result = null;
  wchar_t* newbytes = new wchar_t[string->getByteStringLen()+1];
  int pos,i,currentLen;
  currentLen = 0;
  i=0;
  newbytes[string->getByteStringLen()] = 0;
  for (pos=0;pos<string->getByteStringLen();pos++)
  {
    //get charcode
    charCode[i] = string->getByteString()[pos];
    i++;
    if(cmap->isCharCode(charCode, i))
    {
      //map charcode and return string object
      StringObject * stringCharCode = cmap->getUTFChar(charCode, i);
      if(stringCharCode == null)
      {
        cerr << "\nContentStream:Couldn't get utf16be char from ToUnicode cmap.\n";
        continue;
      }

      //convert from UTF-16BE to wchar (unicode)
      int processed = convertUTFtoWchar(stringCharCode, &newbytes[currentLen], string->getByteStringLen()-currentLen);
      if( processed >= 0)
        currentLen += processed;
      else
        continue;
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

StringObject * charNameToUTFString(const char * charName)
{
  if(charName == null)
    return null;
  int len = GLYPHLIST_LEN;
  char * _ek = null;
  for (int i = 0; i < len; i++)
  {
    if(strcmp(charName, glyphlist[i][0]) == 0)
      return new StringObject(&_ek, (char*) glyphlist[i][1]);
  }
  return null;
}

wchar_t * ContentStream::convertWithBaseEncoding(StringObject * string,const char * encoding[], char ** differences)
{
  unsigned char charCode;
  wchar_t * result = null;
  wchar_t* newbytes = new wchar_t[string->getByteStringLen()+1];
  int pos,currentLen;
  currentLen = 0;
  newbytes[string->getByteStringLen()] = 0;
  for (pos=0;pos<string->getByteStringLen();pos++)
  {
    //get charcode
    charCode = string->getByteString()[pos];
    
    //map charcode and return string object
    const char * charName = null;
    if(differences != null)
      charName = differences[charCode];
    
    if(charName == null)
      charName = encoding[charCode];
    
    if(charName == null)
    {
      cerr << "\nContentStream: Couldn't map character code to character name using some base encoding.\n";
      continue;
    }
    else
    {
      StringObject * stringCharCode = charNameToUTFString(charName);
      if(stringCharCode == null)
      {
        cerr << "\nContentStream:Couldn't get utf16be char from glyphlist.\n";
        continue;
      }

      //convert from UTF-16BE to wchar (unicode)
      int processed = convertUTFtoWchar(stringCharCode, &newbytes[currentLen], string->getByteStringLen()-currentLen);
      if( processed >= 0)
        currentLen += processed;
      else
        continue;
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
  
  if(this->currentCMap != null) //try toUnicode map
  {
    if(stringObject->isHexa)
      return convertStringWithToUnicode(stringObject, this->currentCMap);
    else  //TODO: find out why literal strings are not in ToUnicode map
      return charToWchar((char*)stringObject->getByteString(), stringObject->getByteStringLen());
  }
  else
  {
    if(this->currentFont != null) //try some basic encoding
    {
      char ** differences = null;
      PdfObject * encoding = this->currentFont->getObject("/Encoding", true);
      if(encoding != null && encoding->objectType == PdfObject::TYPE_DICTIONARY)
      {
        //TODO: get diffs from map
        PdfObject * diffs;
        diffs = ((DictionaryObject*) encoding)->getObject("/Differences", true);
        if(diffs != null)
        {
          if(diffs->objectType == PdfObject::TYPE_ARRAY)
          {
            ArrayObject * diffsArr = (ArrayObject*) diffs;
            differences = new char*[256];
            int di;
            for(di = 0; di < 256; di++)
            {
              differences[di] = null;
            }
            vector<PdfObject*>::iterator diffsArrIt = diffsArr->objectList.begin();
            int charCode = 0;
            for(; diffsArrIt != diffsArr->objectList.end(); diffsArrIt++)
            {
              if((*diffsArrIt)->objectType == PdfObject::TYPE_NUMBER)
              {
                charCode = (int) ((NumberObject*)(*diffsArrIt))->number;
              }
              if((*diffsArrIt)->objectType == PdfObject::TYPE_NAME)
              {
                int nameLen = strlen(((NameObject*)(*diffsArrIt))->name);
                char * diffName = new char[nameLen+1]; 
                strncpy(diffName, ((NameObject*)(*diffsArrIt))->name+1, nameLen-1); //we need to add "+1" because we want remove initial '/' char
                diffName[nameLen] = 0;
                if(charCode >= 0 && charCode < 256)
                  differences[charCode] = diffName;
                else
                {
                  cerr << "\nContentStream: Problem with creating differences array.\n";
                  break;
                }
                charCode++;
              }
            }
            //TODO: set diffs to map
          }
          else
          {
            cerr << "\nContentStream: unknown object for differences array.\n";
          }
        }
        encoding = ((DictionaryObject*) encoding)->getObject("/BaseEncoding", true);
      }
      if(encoding != null && encoding->objectType == PdfObject::TYPE_NAME)
      {
        NameObject * encodingName = (NameObject*) encoding;
        if(strcmp(encodingName->name, "/MacRomanEncoding") == 0)
        {
          return convertWithBaseEncoding(stringObject, encoding_MacRoman, differences);
        }
        else if(strcmp(encodingName->name, "/MacExpertEncoding") == 0)
        {
          return convertWithBaseEncoding(stringObject, encoding_MacExpert, differences);
        }
        else if(strcmp(encodingName->name, "/WinAnsiEncoding") == 0)
        {
          return convertWithBaseEncoding(stringObject, encoding_WinAnsi, differences);
        }
        else
        {
          cerr << "\nContentStream: Unsupported encoding. Using simple conversion.\n";
          return charToWchar((char*)stringObject->getByteString(), stringObject->getByteStringLen());
        }
      }
      else
      {
        return convertWithBaseEncoding(stringObject, encoding_Standard, differences);
      }
    }
    else
    {
      cerr << "\nContentStream: No font found for decoding string.\n";
      return L"|-- Unknown string --|";
    }
    cerr << "\nContentStream: All implemented methods for decoding string failed.\n";
    return L"|-- Unknown string --|";
  }
  
  return null;
}
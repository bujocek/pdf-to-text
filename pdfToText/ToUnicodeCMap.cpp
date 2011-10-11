#include "StdAfx.h"
#include "ToUnicodeCMap.h"

bool compareRangeLen(pair<StringObject*, StringObject*> p1, pair<StringObject*, StringObject*> p2)
{
  if(p1.first->length < p2.first->length)
    return true;
  return false;
}

ToUnicodeCMap::ToUnicodeCMap(IndirectObject * io)
{
  this->indirectObject = io;
  if(io != null)
  {
    //Probably not necessary but harmless
    io->load(); 
    io->processAsStream();
  }
  
  //TODO: http://code.google.com/p/pdf-to-text/issues/detail?id=15
  if(this->indirectObject->unencodedStream != null)
  {
    char * stream = this->indirectObject->unencodedStream;
    char * bcsr = strstr(stream, "begincodespacerange");
    bcsr += 19; //skip 'begincodespacerange' keyword
    bcsr = StringUtils::skipWhiteSpace(bcsr);
    do
    {
      StringObject * codeFrom = new StringObject(&bcsr, bcsr);
      StringObject * codeTo = new StringObject(&bcsr, bcsr);
      if(codeFrom->isHexa && codeTo->isHexa)
      {
        codeRanges.push_front(make_pair(codeFrom, codeTo));
      }
      else
      {
        cerr<<"\nToUnicodeMap: Couldn't read code ranges properly.\n";
        break;
      }
      bcsr = StringUtils::skipWhiteSpace(bcsr);
    }while(*bcsr == '<');
    
    codeRanges.sort(compareRangeLen); //sorting not checked for proper functionality

    char * bbfc = strstr(stream, "beginbfchar");
    if(bbfc != null)
    {
      bbfc += 11; //skip 'beginbfchar' keyword
      do
      {
        StringObject * code = new StringObject(&bbfc, bbfc);
        StringObject * utfString = new StringObject(&bbfc, bbfc);
        if(code->isHexa)
          codeCharMap[code->toNum()] = utfString;
        bbfc = StringUtils::skipWhiteSpace(bbfc);
      }while(*bbfc == '<');
    }
    bbfc = strstr(stream, "beginbfrange");
    if(bbfc != null)
    {
      cerr<<"\nToUnicodeCMap: Not Implemented - CMap defined in code ranges.\n";
    }
  }
}

ToUnicodeCMap::~ToUnicodeCMap(void)
{
}

bool ToUnicodeCMap::isCharCode(unsigned char * charCode, int len)
{
  list <pair<StringObject*, StringObject*>>::iterator cri = this->codeRanges.begin();
  for ( ;cri != this->codeRanges.end(); cri++)
  {
    if(cri->first->byteStringLen == len)
    {
      bool isInRange = true;
      for (int i=0; i<len; i++)
      {
        if(charCode[i] < cri->first->getByteString()[i] || charCode[i] > cri->second->getByteString()[i])
        {
          isInRange = false;
          break;
        }
      }
      if(isInRange)
        return true;
    }
  }
  return false;
}

StringObject * ToUnicodeCMap::getUTFChar(unsigned char * charCode, int len)
{
  //TODO: Issue 18
  return null;
}
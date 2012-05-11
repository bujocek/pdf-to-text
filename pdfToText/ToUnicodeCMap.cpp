#include "stdafx.h"
#include "ToUnicodeCMap.h"

bool compareRangeLen(pair<StringObject*, StringObject*> p1, pair<StringObject*, StringObject*> p2)
{
  if(p1.first->length < p2.first->length)
    return true;
  return false;
}

bool compareBFRanges(bfrange x, bfrange y)
{
  if(x.begin < y.begin)
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
    codeRanges.sort(compareRangeLen);

    char * bbfc = strstr(stream, "beginbfchar");
    while(bbfc != null)
    {
      bbfc += 11; //skip 'beginbfchar' keyword
      do
      {
        StringObject code = StringObject(&bbfc, bbfc);
        StringObject * utfString = new StringObject(&bbfc, bbfc);
        if(code.isHexa)
          codeCharMap[code.toNum()] = utfString;
        bbfc = StringUtils::skipWhiteSpace(bbfc);
      }while(*bbfc == '<');
      bbfc = strstr(bbfc, "beginbfchar");
    }
    
    list<bfrange> rangeMapList;
    bbfc = strstr(stream, "beginbfrange");
    while(bbfc != null)
    {
      bbfc += 12;
      do
      {
        StringObject beginCode = StringObject(&bbfc, bbfc);
        StringObject endCode = StringObject(&bbfc, bbfc);
        PdfObject * objectForRange = PdfObject::readValue(&bbfc, bbfc, false);
        BFRange bfr = BFRange();
        bfr.begin = beginCode.toNum();
        bfr.end = endCode.toNum();
        bfr.object = objectForRange;
        rangeMapList.push_front(bfr);
        bbfc = StringUtils::skipWhiteSpace(bbfc);
      }while(*bbfc == '<');
      bbfc = strstr(bbfc, "beginbfrange");
    }
    rangeMapList.sort(compareBFRanges);

    while(!rangeMapList.empty())
    {
      codeRangeMapVector.push_back(rangeMapList.front());
      rangeMapList.pop_front();
    }
  }
}

ToUnicodeCMap::~ToUnicodeCMap(void)
{
}

bool ToUnicodeCMap::isCharCode(unsigned char * charCode, int len)
{
  list <pair<StringObject*, StringObject*> >::iterator cri = this->codeRanges.begin();
  for ( ;cri != this->codeRanges.end(); cri++)
  {
    if(cri->first->getByteStringLen() == len)
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
  int charCodeNum = 0;
  for(int i = 0; i<len; i++)
  {
    charCodeNum *= 16;
    charCodeNum += charCode[i];
  }
  StringObject * utfChar = codeCharMap[charCodeNum];
  if(utfChar == null)
  {
    //try to find range for char
    BFRange * range = null;
    int endSearch = codeRangeMapVector.size()-1;
    if(endSearch > 0)
    {
      int beginSearch = 0;
      do
      {
        int actualSearch = (beginSearch + endSearch) / 2;
        BFRange * actualItem = &codeRangeMapVector[actualSearch];
        if(actualItem->begin <= charCodeNum &&
          actualItem->end >= charCodeNum)
        {
          range = actualItem;
          break;
        }
        if(actualItem->begin > charCodeNum)
        {
          endSearch = actualSearch-1;
        }
        else
        {
          beginSearch = actualSearch+1;
        }
      }while(beginSearch <= endSearch);
    }
    if(range == null)
    {
      //Couldn't map character properly.
      return null;
    }
    else
    {
      int increment = charCodeNum - range->begin;
      if(range->object->objectType == PdfObject::TYPE_STRING)
      {
        StringObject * string = (StringObject*) range->object;
        int numResult = string->toNum();
        numResult += increment;
        int len = string->getByteStringLen()*2;
        char * hexaString = new char[len+3];
        hexaString[0] = '<';
        hexaString[len+1] = '>';
        hexaString[len+2] = 0;
        for(int i = len; i>0; i--)
        {
          int byte = numResult % 16;
          switch(byte)
          {
          case 0: 
            hexaString[i] = '0';
            break;
          case 1: 
            hexaString[i] = '1';
            break;
          case 2: 
            hexaString[i] = '2';
            break;
          case 3: 
            hexaString[i] = '3';
            break;
          case 4: 
            hexaString[i] = '4';
            break;
          case 5: 
            hexaString[i] = '5';
            break;
          case 6: 
            hexaString[i] = '6';
            break;
          case 7: 
            hexaString[i] = '7';
            break;
          case 8: 
            hexaString[i] = '8';
            break;
          case 9: 
            hexaString[i] = '9';
            break;
          case 10: 
            hexaString[i] = 'A';
            break;
          case 11: 
            hexaString[i] = 'B';
            break;
          case 12: 
            hexaString[i] = 'C';
            break;
          case 13: 
            hexaString[i] = 'D';
            break;
          case 14: 
            hexaString[i] = 'E';
            break;
          case 15: 
            hexaString[i] = 'F';
            break;
          }
          numResult /= 16;
        }
        char * _dummy = null;
        utfChar = new StringObject(&_dummy, hexaString);
      }
      else if(range->object->objectType == PdfObject::TYPE_ARRAY)
      {
        ArrayObject * utfArray = (ArrayObject*) range->object;
        utfChar = (StringObject*) utfArray->objectList[increment];
      }
      else
      {
        //Couldn't find utf value in found range.
        return null;
      }
      // add result to char map - so there is no need to create the string again next time the char is used
      this->codeCharMap[charCodeNum] = utfChar; 
    }
  }
  return utfChar;
}

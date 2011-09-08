#include "StdAfx.h"
#include "ToUnicodeCMap.h"

ToUnicodeCMap::ToUnicodeCMap(IndirectObject * io)
{
  this->indirectObject = io;
  if(io != null)
  {
    //Probably not necessary but harmless
    io->load(); 
    io->processAsStream();
  }
  
  //TODO: finish processing the toUnicode Cmap
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
        codeRanges.push_front(make_pair(codeFrom->toNum(), codeTo->toNum()));
      }
      else
      {
        cerr<<"\nToUnicodeMap: Couldn't read code ranges properly.\n";
        break;
      }
      bcsr = StringUtils::skipWhiteSpace(bcsr);
    }while(*bcsr == '<');

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

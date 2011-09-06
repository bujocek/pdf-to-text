#include "StdAfx.h"
#include "ToUnicodeCMap.h"

ToUnicodeCMap::ToUnicodeCMap(IndirectObject * io)
{
  this->indirectObject = io;
  this->codeRangeFrom = 0;
  this->codeRangeTo = 0;
  this->codeWidth = 0;
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
    StringObject cf = StringObject(&bcsr, bcsr);
    codeWidth = strlen(cf.string)/2;
    codeRangeFrom = cf.toNum();
    codeRangeTo = StringObject(&bcsr, bcsr).toNum();
    bcsr = StringUtils::skipWhiteSpace(bcsr);
    if(*bcsr == '<')
      cerr<<"\ToUnicodeCMap: Not Implemented - More code space ranges in cmap.\n";
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

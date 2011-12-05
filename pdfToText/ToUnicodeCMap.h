#pragma once
#include "IndirectObject.h"
#include "StringObject.h"
#include "ArrayObject.h"
#include "StringUtils.h"
#include <list>

/**
Simple struct for representing range of char codes in bfrange section of ToUnicode map
*/
typedef struct bfrange {
  /**
  begin of the range
  */
  int begin;
  /**
  end of the range
  */
  int end;
  /**
  object that specifies mapping of the range
  */
  PdfObject * object;
}BFRange;

/**
Class for representing ToUnicode CMap PDF object. <br>
*/
class ToUnicodeCMap
{
private:
  IndirectObject * indirectObject;
  list <pair<StringObject*, StringObject*> > codeRanges;
  map<int, StringObject *> codeCharMap;
  vector<BFRange> codeRangeMapVector;
public:
  
  /**
  Checks using code ranges of the map if <code>charCode</code> byte sequence of <code>len</code> length is a proper char code.
  \param charCode Code of char we want to check.
  \param len Length of <code>charCode</code>.
  \return Returns true if <code>charCode</code> is character code. False  otherwise.
  */
  bool isCharCode(unsigned char * charCode, int len);
  
  /**
  Maps <code>charCode</code> byte sequence of <code>len</code> length using this Map to UTF value.
  \param charCode Code of char we want to map.
  \param len Length of <code>charCode</code>.
  \return Returns StringObject containing hexa representation of Unicode value of <code>charCode</code> in UTF-16BE encoding.
  */
  StringObject * getUTFChar(unsigned char * charCode, int len);

  /**
  Constructor.<br>
  Creates ToUnicode Map object from IndirectObject.
  \param io IndirectObject the map will be created from
  */
  ToUnicodeCMap(IndirectObject * io);
  
  /**
  Destructor.
  */
  ~ToUnicodeCMap(void);
};

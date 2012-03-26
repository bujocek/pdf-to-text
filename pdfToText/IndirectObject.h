#pragma once
#include "XRef.h"
#include "PdfObject.h"
#include "zlib.h"
#include "StringUtils.h"
#include "NameObject.h"
#include "NumberObject.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <deque>
#include <map>

using namespace std;

#define IN_USE_OBJECT 'n'
#define FREE_OBJECT 'f'

/**
Class for representing Indirect PDF Objects and Stream PDF Object.<br>
Stream objects in PDF are always Indirect objects thats why this class was choosen to represent both objects.<br>
The class also provides functionality decompress stream.
*/
class IndirectObject : public PdfObject
{
private:
	ifstream * file;
	bool isLoaded;
  PdfObject * firstObject;
  bool isProcessed;
  char * objectString;
	long objectStringSize;

  /**
	deflates stream defined starting on streamStart and with length of streamLen and returns deflated result as a string
	*/
	long deflateStream(char * streamStart, long streamLen, char ** output);
  /**
	decompresses using LZW decompression method stream defined starting on streamStart and with length of streamLen and returns deflated result as a string
	*/
	long deLZWStream(char * streamStart, long streamLen, char ** output);
public:
  
  /**
  Determines if object is in use or not according to reference table record. <br>
  Constants IN_USE_OBJECT and FREE_OBJECT are defined in this file to represent this property.
  */
	char objectState;
	
  /**
  Object number of this indirect object according to reference table record.
  */
	int objectNumber;

  /**
  Used when this indirect object has objectState IN_USE_OBJECT.
  Byte offset is position of the indirect object in the file.<br>
  It is offset from the begining of the file to the begining of the object.
  */
	long byteOffset;

  /**
  Used when this indirect object has objectState FREE_OBJECT.
  It is number of next unused object.
  */
	long nextFreeObject;

  /**
  Generation number of this Indirect Object.
  */
	int generationNumber;
  
  /**
  If this IndirectObject is a stream object, than this property stores stream dictionary
  */
  DictionaryObject * streamDictionary;

  /**
  After decoding stream here is stored resulting unencoded stream.
  */
  char * unencodedStream;

  /**
  After decoding stream here is stored resulting length of unencoded stream.
  */
	long unencodedStreamSize;

	/**
	Loads object from file to memory. Need to be called before using the object.
	*/
	bool load();
  
  /**
  Constructor.<br>
  Constructs indirect object from cross reference table record, object number and pdf file.
  \param row Corresponding record from cross reference table that belongs to the object.
  \param objectNum Object number the object has.
  \param iFile Imput PDF file the object should be foud and loaded from.
  */
	IndirectObject(tableRow row, int objectNum, ifstream& iFile);

  /**
  Destructor
  */
	~IndirectObject(void);

	/**
	Tries to retrieve number from Pdf object if it is numeric object.
  Should be called when it is known from context that the indirect object represents number.
  \return Number from the object.
	*/
	long getNumber();

  /**
  Retrieves first PdfObject found after "nn mm obj" in the object or null if nothing found.
  \return First PdfObject in the IndirectObject
  */
  PdfObject * getFirstObject();

  /**
	Processes loaded object. Finds stream if present and decodes it.
	load() method must be called first!
  \return True if object was successfully processed. False if something went wrong.
	*/
	bool processAsStream();

};

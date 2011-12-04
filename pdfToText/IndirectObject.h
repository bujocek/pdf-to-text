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

class IndirectObject : public PdfObject
{
private:
	ifstream * file;
	bool isLoaded;
  PdfObject * firstObject;
  bool isProcessed;
public:
	char objectState;
	char * objectString;
	long objectStringSize;
	
	int objectNumber;
	long byteOffset;
	long nextFreeObject;
	int generationNumber;

  DictionaryObject * streamDictionary;
  char * unencodedStream;
	long unencodedStreamSize;

	/**
	loads object from file to memory
	*/
	bool load();

	IndirectObject(tableRow row, int objectNum, ifstream& iFile);
	~IndirectObject(void);

	/**
	tries to retrieve number from Pdf object if it is numeric object
	*/
	long getNumber();

  /**
  returns first pdf object found after "nn mm obj" in object string
  of null if nothing found
  */
  PdfObject * getFirstObject();

  /**
	Processes loaded object. Finds stream if present and decodes it.
	load() must be called first
	*/
	bool processAsStream();

	/**
	deflates stream defined starting on streamStart and with length of streamLen and returns deflated result as a string
	*/
	long deflateStream(char * streamStart, long streamLen, char ** output);

};

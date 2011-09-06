#pragma once
#include <iostream>
#include <fstream>
#include "DictionaryObject.h"

using namespace std;


//one row of cross refference table
typedef char tableRow[21];

//Cross refference subsection in section
struct XRefSubsection //create class and move to XRef?
{
	tableRow * table;
	int count;
	int firstObjectNumber;
};

class XRef
{
private:
	long findXRef();
	long findTrailer();
	long findNextXRef();
	XRefSubsection * XRefTable;
	XRef * nextXRef;
public:
	XRefSubsection * getXRef();
	XRef(ifstream& file, long position = -1);
	~XRef(void);
  DictionaryObject * trailerDictionary;
	long XRefIndex;
  long startxrefIndex;
	long nextXRefIndex;
	long XRefTrailerIndex;
	long fileSize;
	int sectionCount;
	ifstream * file;
	XRef * getNextXRef();
};

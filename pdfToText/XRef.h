#pragma once
#include <iostream>
#include <fstream>
#include "DictionaryObject.h"

using namespace std;

//one row of cross refference table
typedef char tableRow[21];

/**
Simple struct for representint Cross refference subsection in section
*/
struct XRefSubsection
{
  /**
  Array of rows of cross reference table in this subsection
  */
	tableRow * table;
  
  /**
  number of rows in this subsection
  */
	int count;

  /**
  Object number of first object in this subsection
  */
	int firstObjectNumber;
};

/**
Class for representing one Cross reference section. <br>
Class provides functionality to find previous section (if any) and to process trailer.
*/
class XRef
{
private:
	long findXRef();
	long findTrailer();
	long findNextXRef();
	XRefSubsection * XRefTable;
	XRef * nextXRef;
  long XRefIndex;
  long startxrefIndex;
	long nextXRefIndex;
	long XRefTrailerIndex;
	long fileSize;
public:
  /**
  Retrieves and returns one subsection constructed from all subsections in this section.
  \return One XRefSubsection containing items from all subsections.
  */
	XRefSubsection * getXRef();

  /**
  Constructor. <br>
  Constructs Reference table retrieved from file.
  If position is specified than skips searching and reads directly the table. If not specified than searches for last trailer to get position from.
	\param file Input PDF file
  \param position Position of Reference section in file.
  */
  XRef(ifstream& file, long position = -1);

  /**
  Destructor.
  */
	~XRef(void);

  /**
  Reference to Trailer Dictionary that folows this section
  */
  DictionaryObject * trailerDictionary;

  /**
  Count of all sections.
  */
	int sectionCount;

  /**
  Input PDF file
  */
	ifstream * file;

  /**
  retrieves and returns next (previous in updated PDF) reference section
  \return XRef section that preceeded this one if any.
  */
	XRef * getNextXRef();
};

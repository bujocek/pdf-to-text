#include "StdAfx.h"
#include "XRef.h"
#include "StringUtils.h"

using namespace std;

//TODO: http://code.google.com/p/pdf-to-text/issues/detail?id=2

XRef::XRef(ifstream& iFile, long position)
{
	file = &iFile;
	file->clear();
	file->seekg (0, ios::end);
	fileSize = file->tellg();
  XRefTable = NULL;
	nextXRef = NULL;
	if(logEnabled)
		clog<< "\nFile Size is: "<< fileSize;

	if(position < 0)
		XRefIndex = findXRef();
	else
		XRefIndex = position;
	if(logEnabled)
		clog<< "\nXRefIndex is: "<< XRefIndex;
  
  getXRef();
  if(logEnabled)
		clog<< "\n";

	XRefTrailerIndex = -1;
	if(XRefIndex > 0)
		XRefTrailerIndex = findTrailer();
	if(logEnabled)
		clog<< "\nXRefTrailer is: "<< XRefTrailerIndex;

	nextXRefIndex = -1;
	if(XRefTrailerIndex > 0)
		nextXRefIndex = findNextXRef();
	if(logEnabled)
		clog << "\nnextXRefIndex is: " << nextXRefIndex;
}

//finds where the cross refference trailer part is in file
//XRefIndex have to be set
long XRef::findTrailer()
{
	const int memBlockSize = 512;
	char memBlock[memBlockSize];
  int count = 0;
  for(int i = 0; i<this->sectionCount;i++)
    count += this->XRefTable[i].count;
  long startSearch = XRefIndex + count * 20;
	file->clear();
  file->seekg (startSearch);
	file->read(memBlock, memBlockSize);

	// -- find trailer key word --
	char* trailerPos = strstr(memBlock, "trailer");
	if(trailerPos != NULL)
    return startSearch + trailerPos - memBlock;
	else
  {
    cerr<<"\nXRef: Couldn't find trailer dictionary after reference table on position " << this->XRefIndex << "\n";
		return -1;
  }
}

long XRef::findNextXRef()
{
  const int memBlockSize = fileSize - XRefTrailerIndex;
	char * memBlock = new char[memBlockSize];
	file->clear();
	file->seekg (XRefTrailerIndex); 
	file->read(memBlock, memBlockSize);
	trailerDictionary = new DictionaryObject(null, memBlock+7); //this mishmash with number 7 is to skip "trailer" key word
  //TODO: http://code.google.com/p/pdf-to-text/issues/detail?id=1
	char* trailerBegin = strstr(memBlock, "<<");
	char* trailerEnd = strstr(memBlock, ">>");
	char* prevKey = strstr(memBlock, "Prev");
	if(prevKey != NULL && trailerBegin != NULL && trailerEnd != NULL && 
		trailerBegin < prevKey && prevKey < trailerEnd)
	{
		prevKey += 5;
		return strtol(prevKey, &trailerEnd, 10);;
	}
	else
	{
		return -1;
	}
}

//finds where the cross refference table is in file
//returns XRefIndex
long XRef::findXRef()
{
	const int memBlockSize = 1024;
	char memBlock[memBlockSize];
	file->clear();
	file->seekg (fileSize-memBlockSize);
	file->read(memBlock, memBlockSize);
  long tt = file->gcount();
	//  -- find last startxref key word --
  char* pos = StringUtils::strStrModified(memBlock, "startxref", memBlockSize);
	if(pos == NULL)
	{
    cerr<<"\nXRef.cpp: Couldn't find reference table in file\n Reason: Couldn't find 'startxref' keyword.\n";
		return -1;
	}
  char* newPos = StringUtils::strStrModified(pos + 1, "startxref", memBlockSize - (pos + 1 - memBlock));
	while(newPos != NULL)
	{
		pos = newPos;
		newPos = StringUtils::strStrModified(pos + 1, "startxref", memBlockSize - (pos + 1 - memBlock));
	}
  
  startxrefIndex = fileSize - memBlockSize + pos - memBlock;

	pos+=9; //skip startxref keyword

  pos = StringUtils::skipWhiteSpace(pos);
	
	//  -- get xref index --
	int index = strtol(pos, NULL, 10);
	if(index > 0)
		{
			return index;
		}
	else
		{
      cerr<<"\nXRef.cpp: Couldn't find reference table index after 'startxref' in the file\n";
			return -1;
		}
}

//finds cross refference table
// XRefIndex need to be set
XRefSubsection * XRef::getXRef()
{
	if(XRefIndex>0)
	{
		if(XRefTable != NULL)
			return XRefTable;
		char memBlock[32];
		file->clear();
		file->seekg (XRefIndex);
    StringUtils::fGetLine(memBlock, fileSize - XRefIndex, *file); // skip "xref"
		XRefSubsection * resultTable = new XRefSubsection [65534];
		sectionCount = 0;
		
		//read cross refference sections
		while(true)
		{
			int count = 0;
			int firstObjectNumber = 0;
			StringUtils::fGetLine(memBlock, this->fileSize, *file); // read object index and number of objects
			if(sscanf_s(memBlock, "%i %i", &firstObjectNumber, &count)<2)
			{
				XRefTable = resultTable;
				return resultTable;
			}
			if(logEnabled)
			{
				clog << "\n\nXRef Section " << sectionCount+1 << ":";
				clog << "\nFirst object index: " << firstObjectNumber;
				clog << "\nLength of XRef section: " << count ;
			}
			tableRow * table = new tableRow [count];
			int i = 0;
			for(; i<count; i++)
			{
				StringUtils::fGetLine(table[i],21,*file);
			}
			resultTable[sectionCount].count = count;
			resultTable[sectionCount].firstObjectNumber = firstObjectNumber;
			resultTable[sectionCount].table = table;
			sectionCount++;
		}
	}
	else
	{
		return NULL;
	}
	//	throw new exception("Could not find xref table. Maybe the XRefIndex wasn't set.");
}

XRef * XRef::getNextXRef()
{
	if(this->nextXRefIndex <= 0)
		return NULL;

	if(this->nextXRef != NULL)
	{
		return this->nextXRef;
	}
	else
	{
		return new XRef(*this->file, this->nextXRefIndex);
	}
}

XRef::~XRef(void)
{
	if(XRefTable != NULL)
		delete XRefTable;
	if(nextXRef != NULL)
		delete nextXRef;
}

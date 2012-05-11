// pdfToText.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "XRef.h"
#include "IndirectObject.h"
#include "DictionaryObject.h"
#include "PageTreeNode.h"
#include "IndirectObjectReference.h"
#include <map>
#include <algorithm>
#include <time.h>

using namespace std;

//functions
void end();

//properties
bool logEnabled;
map < pair <int, int>, IndirectObject* > * objectMap;
clock_t startClock;
ofstream logStream ("logfile.txt", ios::app);
ofstream errStream ("errorfile.txt", ios::app);
streambuf *clogBack, *cerrBack;
iconv_t conv_desc = (iconv_t) -1;

int main(int argc, char* argv[])
{
  time_t timer;
  struct tm *ptm;
  time(&timer);
  ptm = localtime(&timer);
  startClock = clock();
	
	//TODO: Get logEnabled value from command line parameter (http://code.google.com/p/pdf-to-text/issues/detail?id=5)
	logEnabled = true;
	if(logEnabled)
	{
		clogBack = clog.rdbuf(logStream.rdbuf());
    clog << "\n\n-------------------\n Start program " << ptm->tm_mday << "." << ptm->tm_mon + 1 << "." << ptm->tm_year + 1900
      << " " << ptm->tm_hour << ":" << ptm->tm_min << ":" << ptm->tm_sec << "\n--------------" << endl;
	}
	cerrBack = cerr.rdbuf(errStream.rdbuf());
  cerr << "\n\n-------------------\n Start program " << ptm->tm_mday << "." << ptm->tm_mon+1 << "." << ptm->tm_year + 1900
    << " " << ptm->tm_hour << ":" << ptm->tm_min << ":" << ptm->tm_sec << endl;

	if(logEnabled)
	{
		clog << "\nProgram launched with parameters:\n";
    cout << "\nProgram launched with parameters:\n";
		for(int ii = 0; ii < argc; ii++)
		{
			clog << (argv[ii]) << " ";
      cout << (argv[ii]) << " ";
		}
	}

  int fromPage = 1;
  int toPage = 0;

	if(argc != 3) //program inputFile oputputFile
	{
		cerr << "\npdfToText: Wrong arguments. Usage: pdfToText [input pdf file] [otput txt file]\n";
		cout << "\npdfToText: Wrong arguments. Usage: pdfToText(.exe) [input pdf file] [otput txt file]\n";
		end();
		return 1;
	}
		
	char * inputFilePath = argv[1];
	//sprintf_s(inputFilePath,strlen(inputFilePath), "%S", argv[1]);
	//Open the PDF source file:
	if(logEnabled)
		clog << "\nOpening source file " << inputFilePath;
	ifstream filei (inputFilePath, ios::binary);
	if (!filei.is_open() || !filei.good())
	{
		cerr << "\npdfToText: Could not open input file";
    cout << "\nCould not open input file.";
		end();
		return 2; //files couldnt be opened
	}
	filei.clear();

	char * outputFilePath = argv[2];

	//Discard existing output:
	if(logEnabled)
		clog << "\nOpening output file " << outputFilePath;
  FILE * fileo = fopen( outputFilePath, "wb");
	if (fileo == null)
	{
		cerr << "\npdfToText: Could not open output file";
    cout << "\nCould not open output file";
		end();
		return 2; //files couldnt be opened
	}
	conv_desc = iconv_open ("WCHAR_T", "UTF-16BE");
	if (conv_desc == (iconv_t)-1)
	{
		/* Initialization failure. */
		cerr << "\npdfToText: Iconv init failed.\n";
		end();
		return 5;
	}
	//-----------------------------------------------
	//---- Read reference table and find objects ----
  if(logEnabled)
		clog<< "\n\n-=| Finding refference table and its objects |=-\n";

	objectMap = new map < pair<int,int>, IndirectObject*>;
	map <pair<int,int>, IndirectObject*>::iterator objectMapIterator;

	XRef * refTable = new XRef(filei);
  XRef * firstRefTable = refTable;
  if(logEnabled)
    clog<<"\n\nProcessing refference table...";
  while(refTable != null && refTable->getXRef() != null)
	{
		XRefSubsection * sections = refTable->getXRef();
		int i,j;
		for(i=0;i<refTable->sectionCount;i++)
		{
			for(j=0;j<XRefSubsection(sections[i]).count;j++)
			{
				int objectNum = XRefSubsection(sections[i]).firstObjectNumber + j;
				IndirectObject * po = new IndirectObject(
					XRefSubsection(sections[i]).table[j], objectNum, filei);
        (*objectMap)[make_pair(po->objectNumber, po->generationNumber)] = po;
				/*if(logEnabled)
				{
					if(po->objectState == IN_USE_OBJECT)
						clog<<"\n"<<po->objectNumber<<" "<<po->objectState<<" "<<po->byteOffset<<" ("<<po->generationNumber<<")";
					else
						clog<<"\n"<<po->objectNumber<<" "<<po->objectState<<" "<<po->nextFreeObject<<" ("<<po->generationNumber<<")";
				}*/
			}
		}
		refTable = refTable->getNextXRef();
	}

  if(logEnabled)
    clog<<"\nFound " << objectMap->size() << " indirect objects.";

  if(objectMap->size() <= 0)
  {
    cerr << "\npdfToText: No objects found in reference table.\n";
    end();
    return 4;
  }
	
	//----------------------
	//---- Load objects ----
	if(logEnabled)
		clog<< "\n\n-=| Loading Objects |=-\n";
  
  int loadedCount = 0;

	for ( objectMapIterator = objectMap->begin(); objectMapIterator != objectMap->end();
		objectMapIterator++)
  {
    if((*objectMapIterator).second->load())
      loadedCount++;
  }

  if(logEnabled)
    clog<<"\nLoaded " << loadedCount << " in use indirect objects.";

  //------------------------
  //---- Read Page tree ----
  if(logEnabled)
		clog<< "\n\n-=| Reading Page tree |=-\n";
  if(firstRefTable == null || firstRefTable->getXRef() == null || firstRefTable->trailerDictionary == null || firstRefTable->trailerDictionary->getObject("/Root") == null)
  {
    cerr<<"\npdfToText: Couldn't find Document Catalog in PDF file.\n";
    end();
    return 3;
  }
  IndirectObject * indirectObject = (IndirectObject*) firstRefTable->trailerDictionary->getObject("/Root");
  DictionaryObject * documentCatalogDictionary = (DictionaryObject*) indirectObject->getFirstObject();
  if(documentCatalogDictionary == null || documentCatalogDictionary->objectType != PdfObject::TYPE_DICTIONARY)
  {
    cerr<<"\npdfToText: Problem with reading Document Catalog in PDF file.\n";
    end();
    return 3;
  }
  if(logEnabled)
    clog<<"\nDocument catalog found in object "<<indirectObject->objectNumber<<" "<<indirectObject->generationNumber<<" R.";
  if(documentCatalogDictionary->getObject("/Pages") == null)
  {
    cerr<<"\npdfToText: Couldn't find page tree in Document Catalog in PDF file.\n";
    end();
    return 3;
  }
  indirectObject = (IndirectObject*) documentCatalogDictionary->getObject("/Pages");
  DictionaryObject * pageTreeRootDictionary = (DictionaryObject*) indirectObject->getFirstObject();
  if(pageTreeRootDictionary == null || pageTreeRootDictionary->objectType != PdfObject::TYPE_DICTIONARY)
  {
    cerr<<"\npdfToText: Problem with reading Page Tree root node dictionary in PDF file.\n";
    end();
    return 3;
  }
  if(logEnabled)
    clog<<"\nPage tree root node dictionary found in object "<<indirectObject->objectNumber<<" "<<indirectObject->generationNumber<<" R.";
  if(logEnabled)
    clog<<"\nConstructing page tree ...";
  PageTreeNode * pageTreeRoot = new PageTreeNode(pageTreeRootDictionary);
  list<PageTreeNode*> pageList;
  list<PageTreeNode*>::iterator pageListIterator;
  pageTreeRoot->createPageList(&pageList);
  
  //skip specified pages
  if(toPage == 0)
    toPage = pageList.size();
  if((toPage - fromPage + 1) > 0 && (toPage - fromPage + 1) <= pageList.size())
  {
    int ii;
    int endCut = pageList.size() - toPage;
    for(ii = 0; ii<endCut; ii++)
      pageList.pop_back();
    for(ii = 1; ii<fromPage; ii++)
      pageList.pop_front();
  }
  else
  {
    cerr << "\npdfToText: Wrong page range specified in arguments.\n";
    end();
    return 1;
  }

  if(logEnabled)
    clog<<"\nPage tree construction finished. \nFound " << pageList.size() << " pages.";

	//-------------------------
	//---- Process objects ----

  if(logEnabled)
		clog<< "\n\n-=| Processing Pages Content Streams |=-\n";

  if(pageList.size() > 0)
  {
    for(pageListIterator = pageList.begin(); pageListIterator != pageList.end(); pageListIterator++)
    {
      (*pageListIterator)->processPage();
    }
  }
  else
  {
    cerr<<"\nNo pages found in a page tree. Nothing to extract.\n";
    end();
    return 3;
  }
  
  //-------------------------------
	//---- Get text from objects ----
	if(logEnabled)
		clog<< "\n\n-=| Getting text from Content Streams |=-\n";
	
  for(pageListIterator = pageList.begin(); pageListIterator != pageList.end(); pageListIterator++)
  {
    (*pageListIterator)->getText(fileo);
  }
  fclose(fileo);
	end();
	return 0;
}

void end()
{
	clock_t endClock = clock();
	double runningTime = ((double)(endClock - startClock))/CLOCKS_PER_SEC;
	if(logEnabled)
		clog << "\n\n--------------\n End program (" << runningTime << "s)\n-------------------" << endl;
	cerr << "\n\n End program (" << runningTime << "s)\n-------------------" << endl;
  clog.rdbuf(clogBack);
  cerr.rdbuf(cerrBack);
  errStream.close();
  logStream.close();
  if((iconv_t) -1)
	  iconv_close (conv_desc);
}


#include "StdAfx.h"
#include "IndirectObject.h"


using namespace std;

IndirectObject::IndirectObject(tableRow row, int objectNum, ifstream& iFile)
{
  this->objectType = PdfObject::TYPE_INDIRECT_OBJECT;
	this->isLoaded = false;
  this->isProcessed = false;
  this->unencodedStream = null;
	this->unencodedStreamSize = -1;
  this->streamDictionary = null;
	this->objectStringSize = -1;
	this->objectString = null;
  this->firstObject = null;
	this->objectNumber = objectNum;
	this->objectState = char(row[17]); //reed 'f' or 'n' - it's on fixed position 17
	this->file = &iFile;
	char * e;
	if(this->objectState == IN_USE_OBJECT)
	{
		e = &row[10];
		this->nextFreeObject = -1;
		this->byteOffset = strtol(row, &e, 10);
	}
	if(this->objectState == FREE_OBJECT)
	{
		e = &row[10];
		this->byteOffset = -1;
		this->nextFreeObject = strtol(row, &e, 10);
	}
	e = &row[16];
	this->generationNumber = int(strtol(&row[11], &e, 10));
}

bool IndirectObject::load()
{
	//find object in file
	if(this->isLoaded)
		return true;
	if(this->file != null && this->byteOffset > 0)
	{
		//read entire object into the memory
		//find obj and endobj keywords
		queue<char*, deque<char*>> blockQueue;
		int blockSize = 128;
		char * prevBlock = null;
		char * block = null;
		char * endobj = null;
		this->file->clear();
		this->file->seekg(this->byteOffset);
		while(endobj == null)
		{
			prevBlock = block;
			block = new char[blockSize];
			if(this->file->eof())
			{
				cerr<<"\nPdfObject: Couldn't find endobj of " << this->objectNumber << " object. (Input file eof)";
				return false;
			}
			this->file->read(block, blockSize-1);
			if(this->file->fail())
			{
				cerr<<"\nPdfObject: Couldn't find endobj of " << this->objectNumber << " object. (Input file fail)";
				return false;
			}
			block[blockSize-1] = 0; //end this string
			blockQueue.push(block);	
			
			//check if 'endobj' is between blocks
			if(endobj == null && prevBlock != null)
			{
				char joinedBlocks[11];
				memcpy(joinedBlocks, &prevBlock[blockSize-6], 5);
				memcpy(&joinedBlocks[5], block, 5);
				joinedBlocks[10] = 0;
				endobj = StringUtils::strStrModified(joinedBlocks, "endobj", 11);
				if(endobj != null)
				{
					int positionInJoined = endobj - joinedBlocks;
					endobj = &block[positionInJoined -5 +6];
				}
			}

			if(endobj == null)
			{
				endobj = StringUtils::strStrModified(block, "endobj", blockSize);
				if(endobj != null)
					endobj += 6;
			}
		} 
		this->objectStringSize = ( (int) blockQueue.size() - 1)*(blockSize-1) + (endobj - block);
		this->objectString = new char[this->objectStringSize+1];
		char * index = this->objectString;
		while((int) blockQueue.size() > 1)
		{
			block = blockQueue.front();
			memcpy(index, block, blockSize -1);
			index += blockSize -1;
			delete block;
			blockQueue.pop();
		}
		block = blockQueue.front();
		memcpy(index, block, (endobj - block));
		blockQueue.pop();
    this->objectString[this->objectStringSize] = 0;
		/*if(logEnabled)
      clog << "\nObject " << this->objectNumber << " " << this->generationNumber << " R loaded into memory.";*/
		this->isLoaded = true;
		return true;
	}
	return false;
}

long IndirectObject::getNumber()
{
	if(this->isLoaded)
	{
    //TODO: http://code.google.com/p/pdf-to-text/issues/detail?id=16
		char * obj = strstr(this->objectString, "obj"); //skip object numbers
		obj+=3;
		return strtol(obj,null,0);
	}
	return 0;
}

PdfObject * IndirectObject::getFirstObject()
{
  if(this->firstObject == null)
  {
    char * string = this->objectString;
    string = strstr(string, "obj");
    string += 3;
    string = StringUtils::skipWhiteSpace(string);
    this->firstObject = PdfObject::readValue(null, string);
  }
  return this->firstObject;
}

IndirectObject::~IndirectObject(void)
{
}

bool IndirectObject::processAsStream()
{
	if(this->isProcessed)
		return true;
	
	if(this->objectStringSize > 0)
	{
		this->unencodedStream = null;
		this->unencodedStreamSize = -1;
		char * streamString = StringUtils::strStrModified(this->objectString, "stream", this->objectStringSize);
		if(streamString != null)
		{
			streamString+=6;
			char * endstreamString = StringUtils::strStrModified(this->objectString, "endstream", this->objectStringSize);
			if(endstreamString != null)
			{
				//Stream found
				if(streamString < endstreamString)
				{
					//remove newlines and lineFeeds after streamStart  and before streamEnd keywords
					//so between pointers is just byte stream
					if (*streamString == '\r' && *(streamString+1) == '\n')
						streamString+=2;
					else if (*streamString == '\n')
						streamString++;
					if (*(endstreamString-2) == '\r' && *(endstreamString-1) == '\n')
						endstreamString-=2;
					else if (*(endstreamString-1) == '\n')
						endstreamString--;

					if(logEnabled)
						clog << "\nStream found in object " << this->objectNumber <<
              " " << this->generationNumber << " R.";
					
					//Find dictionary and find used filter
          PdfObject * fo = this->getFirstObject();
          if(fo->objectType == PdfObject::TYPE_DICTIONARY)
            this->streamDictionary = (DictionaryObject*) fo;
          else
          {
            cerr << "\nIndirectObject: Couldn't read stream dictionary in object " << this->objectNumber <<
							". \nSkipping this invalid pdf stream object.\n";
            return false;
          }

          //Find length of the stream
          PdfObject * lengthObject = this->streamDictionary->getObject("/Length");
          if(lengthObject == null)
					{
						cerr << "\nIndirectObject: Couldn't find stream length in dictionary of object " << this->objectNumber <<
							". \nSkipping this invalid pdf stream object.\n";
						return false;
					}
          long lengthNumber = -1;
          if(lengthObject->objectType == PdfObject::TYPE_NUMBER)
          {
            lengthNumber = (long) ((NumberObject*) lengthObject)->number;
          }
          if(lengthObject->objectType == PdfObject::TYPE_INDIRECT_OBJECT) 
          {
            IndirectObject * po = (IndirectObject*)lengthObject;
						lengthNumber = po->getNumber();
						if(lengthNumber == 0)
						{
              cerr << "\nIndirectObject: Couldn't get indirect length parameter for stream in object\n" << this->objectNumber <<".";
							return false;
						}
					}
					if(lengthNumber < (endstreamString - streamString)-1 ||
            lengthNumber > (endstreamString - streamString)+1 ) // just for sure that pointers and length match (test for +-1 match)
					{
						cerr << "\nIndirectObject: Possible problem with determining stream\n and length of stream in object " <<
						this->objectNumber << "\n Length from dictionary is: " << lengthNumber <<
						"\n Length between stream and endstream is: " << (endstreamString - streamString) << "\n";
						//correcting endstreamString
						endstreamString = streamString + lengthNumber;
					}
          
          //Find used filter for the stream
          PdfObject * filterObject = this->streamDictionary->getObject("/Filter");
          if(filterObject == null)
					{
					  //than it is unencoded stream (not so common case)
            this->unencodedStream = new char[lengthNumber+1];
            memcpy( this->unencodedStream, streamString, lengthNumber);
            this->unencodedStream[lengthNumber] = 0;
						this->unencodedStreamSize = lengthNumber;
					}
					else
					{
						//TODO: http://code.google.com/p/pdf-to-text/issues/detail?id=10
						//If filter is not array and is flate decode than decode it
            if(filterObject->objectType == PdfObject::TYPE_NAME && strcmp(((NameObject *) filterObject)->name, "/FlateDecode") == 0)
            {
							this->unencodedStreamSize = this->deflateStream(streamString, 
								lengthNumber, &this->unencodedStream);
							if(logEnabled)
								clog << "\nStream successfully decoded by FlateDecode.";
						}
						else
						{
							cerr << "\nIndirectObject: Not implemented: \n Unsupported filters in stream of object " <<
							this->objectNumber << ". \n Skipping this object.\n";
							return false;
						}
					}
				}
			}
		}
		this->isProcessed = true;
		return true;
	}
	return false;
}

long IndirectObject::deflateStream(char * streamStart, long streamLen, char ** output)
{
  long outsize = streamLen*10; //assuming that output will not be bigger than ten times input

  if(this->streamDictionary != null && this->streamDictionary->getObject("DL") != null) 
  {
    if(this->streamDictionary->getObject("DL")->objectType == PdfObject::TYPE_NUMBER)
      outsize = (long)((NumberObject*) this->streamDictionary->getObject("DL"))->number;
    else if(this->streamDictionary->getObject("DL")->objectType == PdfObject::TYPE_INDIRECT_OBJECT)
    {
      IndirectObject * io = (IndirectObject *) this->streamDictionary->getObject("DL");
      if(io != null)
        outsize = io->getNumber();
    }
  }

	char * out = new char [outsize]; 
	z_stream zstrm;
	memset(&zstrm, 0, sizeof(zstrm));
	
  zstrm.avail_in = streamLen;
	zstrm.avail_out = outsize;
	zstrm.next_in = (Bytef*)(streamStart);
	zstrm.next_out = (Bytef*)out;

	int rsti = inflateInit(&zstrm);
	if (rsti == Z_OK)
	{
		int rst2 = inflate(&zstrm, Z_FINISH);
		if (rst2 >= 0)
		{
			//Ok, got something
			*output = new char [zstrm.total_out+1];
			memcpy(*output, out, zstrm.total_out);
			delete[] out;
      (*output)[zstrm.total_out] = 0;
			return zstrm.total_out;
		}
	}

	//if something went wrong
	delete[] out;
	return -1;
}

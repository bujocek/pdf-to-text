#include "StdAfx.h"
#include "StringUtils.h"
#include <iostream>

StringUtils::StringUtils(void)
{
}

StringUtils::~StringUtils(void)
{
}

//TODO: http://code.google.com/p/pdf-to-text/issues/detail?id=4
char * StringUtils::strStrModified(char * strSource, char * searchString, int length, bool caseInsensitive)
{
	if(strSource == NULL || searchString == NULL)
		return NULL;
	if(length < 0)
		length = strlen(strSource);
	int i = 0;
	int len = strlen(searchString);
	if(len > length)
		return NULL;
	for(; i < length-len; i++)
	{
		bool found = true;
		for(int j = 0; j<len; j++)
		{	
			if( strSource[i+j] != searchString[j] ||
				(caseInsensitive && tolower(strSource[i+j]) != tolower(searchString[j]) )) 
			{
				found = false;
				break;
			}
		}
		if(found)
			return &strSource[i];
	}
	return NULL;
}

bool StringUtils::isDelimiter(char character)
{
  switch(character)
  {
    case 40:
    case 41:
    case 60:
    case 62:
    case 91:
    case 93:
    case 123:
    case 125:
    case 47:
    case 37:
      return true;
    default:
      return false;
  }
}

bool StringUtils::isEOL(char character)
{
  switch(character)
  {
    case 10:
    case 13:
      return true;
    default:
      return false;
  }
}

bool StringUtils::isWhiteSpace(char character)
{
  switch(character)
  {
    case 0:
    case 9:
    case 10:
    case 12:
    case 13:
    case 32:
      return true;
    default:
      return false;
  }
}

char * StringUtils::skipWhiteSpace(char * string)
{
  while(StringUtils::isWhiteSpace(*string))
    string++;
  return string;
}

void StringUtils::fGetLine(char * destination, int maxLength, ifstream& iFile)
{
  int i = 0;
  char c;
  do
  {
    iFile.get(c);
    destination[i] = c;
    i++;
  }while(c != EOF && c!='\r' && c!='\n' && i<maxLength-1);
  
  if(c == '\r' && i<maxLength-1) //find out if it is two char EOL ("\r\n")
  {
    iFile.get(c);
    if(c == '\n')
    {
      destination[i] = c;
      i++;
    }
    else //move backwards - it wasn't two char EOL
    {
      iFile.seekg(-1,ios_base::cur);
    }
  }
  destination[i]=0;
}
#include "StdAfx.h"
#include "NameObject.h"

using namespace std;

NameObject * NameObject::getNameObject(char ** endKey, char * source)
{
  if(source == null)
  {
    cerr<<"\nNameObject: Invalid source string. Can not create Name Object.\n";
    return null;
  }
  char * keyStart = strstr(source, "/");
  if(keyStart == null)
  {
    cerr<<"\nNameObject: No name object found in given source. Can not create Name Object.\n";
    return null;
  }
  return new NameObject(endKey, keyStart);
}

NameObject::NameObject(char ** endKey, char * source)
{
  this->objectType = PdfObject::TYPE_NAME;
  if(source == NULL)
  {
    this->name = NULL;
    cerr<<"\nNameObject: Invalid source string. Can not create Name Object.\n";
  }
  else
  {
    char * keyStart = source;
    char * keyEnd = keyStart+1;
    if((*source)!= '/')
    {
      this->name = NULL;
      cerr<<"\nNameObject: No name object found in given source. Can not create Name Object.\n";
    }
    else
    {
      while(!StringUtils::isWhiteSpace(*keyEnd) && *keyEnd != '/' && *keyEnd != '>' && 
            *keyEnd != ']' && *keyEnd != '[' && *keyEnd != '<' && *keyEnd != '(')
        keyEnd++;

      int len = keyEnd-keyStart;
      this->name = new char[ len + 1];
      strncpy(this->name, keyStart, len);
      this->name[len] = 0;
      *endKey = keyEnd;
    }
  }
}

NameObject::~NameObject(void)
{
  delete this->name;
}

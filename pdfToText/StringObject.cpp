#include "stdafx.h"
#include "StringObject.h"

StringObject::StringObject(char ** endKey, char * source)
{
  this->objectType = PdfObject::TYPE_STRING;
  this->numVal = -1;
  this->byteString = null;
  this->charString = null;
  this->hexaString = null;
  this->byteStringLen = -1;
  this->hexaStringLen = -1;
  this->charStringLen = -1;
  this->length = 0;
  source = StringUtils::skipWhiteSpace(source);
  this->source = source;
  if(*source == '<')
  {
    *endKey = strchr(source,'>');
    if(*endKey == null)
    {
      this->string = null;
      cerr << "\nStringObject: Couldn't recognize string in source.\n";
    }
    else
    {
      source += 1;
      this->length = *endKey - source;
      this->string = new char[this->length + 1];
      strncpy(this->string, source, this->length);
      this->string[this->length] = 0;
      this->isHexa = true;
      *endKey += 1;
    }
  }
  else if(*source == '(')
  {
    *endKey = source;
    //search for closing parantheses and check if there isn't backslash char before
    int level = 0;
    do
    {
      *endKey = (*endKey) + 1;
      if( **endKey == '(' && *((*endKey)-1) != '\\')
        level++;
      if( **endKey == ')' && *((*endKey)-1) != '\\')
        level--;
    }while(level > 0 || ! ( **endKey == ')' && *((*endKey)-1) != '\\'));

    if(*endKey == null)
    {
      this->string = null;
      cerr << "\nStringObject: Couldn't recognize string in source.\n";
    }
    else
    {
      source += 1;
      this->length = *endKey - source;
      this->string = new char[this->length + 1];
      strncpy(this->string, source, this->length);
      this->string[this->length] = 0;
      this->isHexa = false;
      *endKey += 1;
    }
  }
  else
  {
    cerr << "\nStringObject: No string object found in given source.\n";
  }
}

StringObject::~StringObject(void)
{
}

unsigned char hexachartochar(char hexa)
{
  switch(hexa)
  {
  case '0':
    return 0;
  case '1':
    return 1;
  case '2':
    return 2;
  case '3':
    return 3;
  case '4':
    return 4;
  case '5':
    return 5;
  case '6':
    return 6;
  case '7':
    return 7;
  case '8':
    return 8;
  case '9':
    return 9;
  case 'A':
  case 'a':
    return 10;
  case 'B':
  case 'b':
    return 11;
  case 'C':
  case 'c':
    return 12;
  case 'D':
  case 'd':
    return 13;
  case 'E':
  case 'e':
    return 14;
  case 'F':
  case 'f':
    return 15;
  }
  return -1;
}

char numToHexaChar(int num)
{
  switch(num)
  {
    case 0:
      return '0';
    case 1:
      return '1';
    case 2:
      return '2';
    case 3:
      return '3';
    case 4:
      return '4';
    case 5:
      return '5';
    case 6:
      return '6';
    case 7:
      return '7';
    case 8:
      return '8';
    case 9:
      return '9';
    case 10:
      return 'A';
    case 11:
      return 'B';
    case 12:
      return 'C';
    case 13:
      return 'D';
    case 14:
      return 'E';
    case 15:
      return 'F';
  }
  return null;
}

int StringObject::toNum()
{
  if(this->numVal >= 0)
    return this->numVal;
  if(this->isHexa)
  {
    int result = 0;
    int len = this->getByteStringLen();
    for(int i = 0; i<len; i++)
    {
      result *= 16;
      result += this->getByteString()[i];
    }
    this->numVal = result;
    return result;
  }
  return 0;
}

int StringObject::getByteStringLen()
{
  if(this->byteStringLen < 0)
  {
    if(this->isHexa)
    {
      this->byteStringLen = this->length/2 + this->length%2;
    }
    else
    {
      getByteString(); //we need to generate the string to know proper size because of back sequences like \n \ddd ...
    }
  }
  return this->byteStringLen;
}

unsigned char charToNum(char ch)
{
  switch(ch)
  {
  case '0':
    return 0;
  case '1':
    return 1;
  case '2':
    return 2;
  case '3':
    return 3;
  case '4':
    return 4;
  case '5':
    return 5;
  case '6':
    return 6;
  case '7':
    return 7;
  case '8':
    return 8;
  case '9':
    return 9;
  }
  return 0xff;
}

unsigned char * StringObject::getByteString()
{
  if(this->byteString == null)
  {
    if(this->isHexa)
    {
      this->byteStringLen = this->length/2 + this->length%2;
      this->byteString = new unsigned char[this->byteStringLen+1];
      char c1,c2;
      for(int i = 0; i < this->byteStringLen; i++)
      {
        c1 = this->string[i*2];
        if(i*2+1 < this->length)
          c2 = this->string[i*2+1];
        else
          c2 = '0';
        this->byteString[i] = hexachartochar(c1)*16+hexachartochar(c2);
      }
      this->byteString[this->byteStringLen] = 0;
    }
    else
    {
      this->byteString = new unsigned char[this->length+1]; //assuming that byte string will be same or shorter than char string
      this->byteString[this->length] = 0;
      int pos = 0;
      for(int j = 0; j < this->length; j++)
      {
        if(this->string[j] == '\\' )
        {
          j++;
          if(j>=this->length)
            break;
          if(this->string[j] >= '0' && this->string[j] <= '9') //is octal representation
          {
            unsigned char decNum = 0;
            int k = 0;
            for(; k < 3; k++)
            {
              if(j+k < this->length)
              {
                unsigned char num = charToNum(this->string[j+k]);
                if(num != 0xff)
                  decNum = decNum*8 + num;
                else
                  break;
              }
              else
                break;
            }
            if(k == 3)
              j+=2;
            else
              j+=k;
            this->byteString[pos] = decNum;
            pos++;
          }
          else
          {
            switch(this->string[j])
            {
            case '\\':
            case ')':
            case '(':
              this->byteString[pos] = this->string[j];
              pos++;
              break;
            case 'n':
              this->byteString[pos] = '\n';
              pos++;
              break;
            case 'r':
              this->byteString[pos] = '\r';
              pos++;
              break;
            case 't':
              this->byteString[pos] = '\t';
              pos++;
              break;
            case 'b':
              this->byteString[pos] = '\b';
              pos++;
              break;
            case 'f':
              this->byteString[pos] = '\f';
              pos++;
              break;
            case '\n':
              break;
            case '\r':
              if(j+1 < this->length && this->string[j+1] == '\n')
              {
                j++;
              }
              break;
            }
          }
        }
        else
        {
          this->byteString[pos] = this->string[j];
          pos++;
        }
      }
      if(byteString[this->length] != 0 || this->length < pos)
      {
        cerr<<"StringObject: something went wrong when parsing char string to byte string.";
        this->byteString[this->length] = 0;
      }
      this->byteStringLen = pos;
      this->byteString[pos] = 0;
    }
  }
  return this->byteString;
}

char * StringObject::getCharString()
{
  if(this->charString == null)
  {
    if(this->isHexa)
    {
      this->charString = (char*) this->getByteString(); //TODO: when needed it should be rewritten to use escape sequences - not just pure bytes
      this->charStringLen = this->getByteStringLen();
    }
    else
    {
      this->charString = this->string;
      this->charStringLen = this->length;
    }
  }
  return this->charString;
}

char * StringObject::getHexaString()
{
  if(this->hexaString == null)
  {
    if(!this->isHexa)
    {
      unsigned char * bytes = this->getByteString();
      this->hexaStringLen = this->getByteStringLen() * 2;
      this->hexaString = new char[this->hexaStringLen+1];
      char c1;
      for(int i = 0; i<this->hexaStringLen; i+=2)
      {
        c1 = bytes[i];
        this->hexaString[i] = numToHexaChar(c1/16);
        this->hexaString[i+1] = numToHexaChar(c1%16);
      }
      this->hexaString[this->hexaStringLen] = 0;
    }
    else
    {
      this->hexaString = this->string;
      this->hexaStringLen = this->length;
    }
  }
  return this->hexaString;
}
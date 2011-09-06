#pragma once

/**
This is base class for representing all main Pdf Objects. See TYPE_... constants to know which objects are represented.
It provides some basic parsing functionality for objects and is used to reference any object via base class.
*/
class PdfObject
{
public:
  static const int TYPE_NUMBER = 1;
  static const int TYPE_INDIRECT_OBJECT = 2;
  static const int TYPE_DICTIONARY = 3;
  static const int TYPE_NAME = 4;
  static const int TYPE_INDIRECT_OBJECT_REFFERENCE = 5;
  static const int TYPE_ARRAY = 6;
  static const int TYPE_STRING = 7;
  static const int TYPE_BOOL = 8;
  static const int TYPE_NULL = 9;
  static const int TYPE_CONTENT_STREAM = 10;
  static const int TYPE_OPERATOR = 11;

  int objectType;
  PdfObject(void);
  ~PdfObject(void);

  /**
  reads value from source string and returns it as pdf object
  reads all types of pdf objects
  sets endKey parameter to the end position of the reading
  */
  static PdfObject * readValue(char ** endKey, char * source, bool isRefferenceAllowed = true, int sourceLen = -1);
  static PdfObject * getNullObject();
};

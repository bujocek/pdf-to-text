#pragma once
/**
This is base class for representing all main Pdf Objects. <br>
See TYPE_... constants to know which objects can be represented.
It provides some basic parsing functionality for objects and is used to reference any object via base class.
*/
class PdfObject
{
public:

  /**
  Constant for objectType NumberObject
  */
  static const int TYPE_NUMBER = 1;
  
  /**
  Constant for objectType IndirectObject
  */
  static const int TYPE_INDIRECT_OBJECT = 2;
  
  /**
  Constant for objectType DictionaryObject
  */
  static const int TYPE_DICTIONARY = 3;
  
  /**
  Constant for objectType NameObject
  */
  static const int TYPE_NAME = 4;
  
  /**
  Constant for objectType IndirectObjectReference
  */
  static const int TYPE_INDIRECT_OBJECT_REFFERENCE = 5;

  /**
  Constant for objectType ArrayObject
  */
  static const int TYPE_ARRAY = 6;

  /**
  Constant for objectType StringObject
  */
  static const int TYPE_STRING = 7;

  /**
  Constant for objectType BoolObject
  */
  static const int TYPE_BOOL = 8;

  /**
  Constant for objectType of Null object (it has no special class)
  */
  static const int TYPE_NULL = 9;

  /**
  Constant for objectType ContentStream
  */
  static const int TYPE_CONTENT_STREAM = 10;

  /**
  Constant for objectType OperatorObject
  */
  static const int TYPE_OPERATOR = 11;

  /**
  Type of the object. See TYPE_... constants for more detail.
  */
  int objectType;

  /**
  Constructor
  */
  PdfObject(void);

  /**
  Destructor
  */
  ~PdfObject(void);

  /**
  Pointer to source the inherited object was created from. 
  Mainly usefull just for debug reasons.
  */
  char * source;

  /**
  Reads value from <code>source</code> string and returns it as PdfObject<br>
  Can process all types of PDF objects.
  \return PdfObject that was read from <code>source</code>
  \param endKey Pointer to the end of found object in <code>source</code> will be stored to this parameter.
  \param source Source string the value will be read from.
  \param isReferenceAllowed If set to false than does not check for occurance of IndirectObjectReference. Default value is <code>true</code>
  */
  static PdfObject * readValue(char ** endKey, char * source, bool isRefferenceAllowed = true);

  /**
  This method provides us with null object because there is no NullObject class for it.
  \return Null PdfObject
  */
  static PdfObject * getNullObject();
};

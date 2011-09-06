#pragma once
#include <fstream>
using namespace std;
class StringUtils
{
public:
	StringUtils(void);
	~StringUtils(void);

	/**this is byte modified strstr() method
	the only difference between strstr() is that 
	in this method strSource can contain 0 bytes (0 byte normaly ends the string)
	and searching does not end but searches further till the length
	is reached
	if length parameter is not specified (or has value < 0) than strlen of strSource is used
	*/
	static char * strStrModified(char * strSource, char * searchString, int length = -1,  bool caseInsensitive = false);

  /**
  determines if character is (as considered in PDF specification) a white space
  which means if it has one of these codes:0,9,10,12,13,32
  */
  static bool isWhiteSpace(char character);

  /**
  skips all white space characters from the begining and returns first position of non white space character
  */
  static char * skipWhiteSpace(char * string);
  
  /**
  Reads one line from file. 
  It has same functionality like ifstream::getline method.
  The only difference is that it is considering '\n', '\r' and combination "\r\n" as new line characters.
  */
  static void fGetLine(char * destination, int maxLength, ifstream& iFile);
};

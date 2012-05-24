#pragma once
#include <fstream>
using namespace std;
class StringUtils
{
public:
	StringUtils(void);
	~StringUtils(void);

	/**
  This is byte modified <code>strstr()</code> method.<br>
	The only difference between <code>strstr()</code> is that 
	in this method <code>strSource</code> can contain NULL bytes (NULL byte normaly ends the string)
	and searching does not end but searches further till the length
	is reached.<br>
	If length parameter is not specified (or has value < 0) than strlen of strSource is used.
  \param strSource Search source
  \param sarchString String to be found in <code>strSource</code>
  \param length Length of the <code>strSource</code>
  \param caseInsensitive Set to true if case insensitive search is needed.
  \return Pointer to the first occurence of the <code>searchString</code> in <code>strSource</code>. NULL if not found.
	*/
	static char * strStrModified(char * strSource, char * searchString, int length = -1,  bool caseInsensitive = false);

  /**
  Determines if character is (as considered in PDF specification) a delimiter.
  Which means if it has one of these codes:40,41,60,62,91,93,123,125,47,37.
  \param character The character we want to test.
  \return True if character is delimiter. False otherwise.
  */
  static bool isDelimiter(char character);

  /**
  Determines if character is (as considered in PDF specification) a end-of-line marker
  which means if it has one of these CR, LF. 
  According to PDF specification the eol marker can be even combination CR+LF but in this case of
  boolean function that determines if it is  EOL it is enogh to check just those two chars.
  \param character The character we want to test.
  \return True if character is eol. False otherwise.
  */
  static bool isEOL(char character);
  
  /**
  Determines if character is (as considered in PDF specification) a white space
  which means if it has one of these codes:0,9,10,12,13,32.
  \param character The character we want to test.
  \return True if character is White space. False otherwise.
  */
  static bool isWhiteSpace(char character);

  /**
  Skips all white space characters from the begining of the string and returns first position of non white space character.
  \param string The string we want to skip white space in.
  \param len Length of the string. If missing than this method can be unsafe.
  \return Position of first non white space character.
  */
  static char * skipWhiteSpace(char * string, int len = -1);
  
  /**
  Reads one line from file. 
  It has almost same functionality like ifstream::getline method.
  The only difference is that it is considering '\n', '\r' and combination "\r\n" as new line characters.
  \param destination Reference to the string the line will be stored.
  \param maxLength Is upper limit for line length
  \param iFile File we read line from.
  */
  static void fGetLine(char * destination, int maxLength, ifstream& iFile);
};

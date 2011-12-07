// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define null NULL

#include "IndirectObject.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iconv.h"
#include <map>

using namespace std;

extern bool logEnabled;
extern iconv_t conv_desc;
extern map < pair < int, int >, IndirectObject* > * objectMap;
// reference additional headers your program requires here

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define null NULL

#include "targetver.h"
#include "IndirectObject.h"

#include <stdio.h>
#include <tchar.h>
#include <map>

using namespace std;

extern bool logEnabled;
extern map < pair < int, int >, IndirectObject* > * objectMap;
// reference additional headers your program requires here

//========================================================================
//
// UnicodeMap.h
//
// Mapping from Unicode to an encoding.
//
// Copyright 2001-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef UNICODEMAP_H
#define UNICODEMAP_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "gtypes.h"
#include "CharTypes.h"

#if MULTITHREADED
#include "GMutex.h"
#endif

class GString;

//------------------------------------------------------------------------

enum UnicodeMapKind {
  unicodeMapUser,		// read from a file
  unicodeMapResident,		// static list of ranges
  unicodeMapFunc		// function pointer
};

typedef int (*UnicodeMapFunc)(Unicode u, char *buf, int bufSize);

struct UnicodeMapRange {
  Unicode start, end;		// range of Unicode chars
  Guint code, nBytes;		// first output code
};

struct UnicodeMapExt;

//------------------------------------------------------------------------

class UnicodeMap {
public:

  // Create the UnicodeMap specified by <encodingName>.  Sets the
  // initial reference count to 1.  Returns NULL on failure.
  static UnicodeMap *parse(GString *encodingNameA);

  // Create a resident UnicodeMap.
  UnicodeMap(const char *encodingNameA, GBool unicodeOutA,
	     UnicodeMapRange *rangesA, int lenA);

  // Create a resident UnicodeMap that uses a function instead of a
  // list of ranges.
  UnicodeMap(const char *encodingNameA, GBool unicodeOutA,
	     UnicodeMapFunc funcA);

  ~UnicodeMap();

  void incRefCnt();
  void decRefCnt();

  GString *getEncodingName() { return encodingName; }

  GBool isUnicode() { return unicodeOut; }

  // Return true if this UnicodeMap matches the specified
  // <encodingNameA>.
  GBool match(GString *encodingNameA);

  // Map Unicode to the target encoding.  Fills in <buf> with the
  // output and returns the number of bytes used.  Output will be
  // truncated at <bufSize> bytes.  No string terminator is written.
  // Returns 0 if no mapping is found.
  int mapUnicode(Unicode u, char *buf, int bufSize);

private:

  UnicodeMap(GString *encodingNameA);

  GString *encodingName;
  UnicodeMapKind kind;
  GBool unicodeOut;
  union {
    UnicodeMapRange *ranges;	// (user, resident)
    UnicodeMapFunc func;	// (func)
  };
  int len;			// (user, resident)
  UnicodeMapExt *eMaps;		// (user)
  int eMapsLen;			// (user)
  int refCnt;
#ifdef MULTITHREADED
  GMutex mutex;
#endif
};

//------------------------------------------------------------------------

#define unicodeMapCacheSize 4

class UnicodeMapCache {
public:

  UnicodeMapCache();
  ~UnicodeMapCache();

  // Get the UnicodeMap for <encodingName>.  Increments its reference
  // count; there will be one reference for the cache plus one for the
  // caller of this function.  Returns NULL on failure.
  UnicodeMap *getUnicodeMap(GString *encodingName);

private:

  UnicodeMap *cache[unicodeMapCacheSize];
};

#endif

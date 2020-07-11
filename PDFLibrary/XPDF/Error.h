//========================================================================
//
// Error.h
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef ERROR_H
#define ERROR_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include <stdio.h>
#include "config.h"

// Catalog.cpp
#define S_CATALOG_WRONG_TYPE "Catalog object is wrong type (%s)"
#define S_PAGES_WRONG_TYPE "Top-level pages object is wrong type (%s)"
#define S_PAGE_COUNT_WRONG_TYPE "Page count in top-level pages object is wrong type (%s)"
#define S_PAGE_COUNT_INCORRECT "Page count in top-level pages object is incorrect"
#define S_UNKNOWN_METADATA_TYPE "Unknown Metadata type: '%s'"
#define S_KIDS_WRONG_TYPE "Kids object (page %d) is wrong type (%s)"
#define S_KID_WRONG_TYPE "Kid object (page %d) is wrong type (%s)"
#define S_BAD_DEST_VALUE "Bad named destination value"

// CharCodeToUnicode.cpp
#define S_OPEN_CIDTOUNICODE_FILE "Couldn't open cidToUnicode file '%s'"
#define S_BAD_LINE_CIDTOUNICODE_FILE "Bad line (%d) in cidToUnicode file '%s'"
#define S_OPEN_UNICODETOUNICODE_FILE "Couldn't open unicodeToUnicode file '%s'"
#define S_BAD_LINE_UNICODETOUNICODE_FILE "Bad line (%d) in unicodeToUnicode file '%s'"
#define S_FIND_TOUNICODE_CMAP "Couldn't find ToUnicode CMap file for '%s'"
#define S_BFCHAR_UNICODE_CMAP "Illegal entry in bfchar block in ToUnicode CMap"
#define S_BFRANGE_UNICODE_CMAP "Illegal entry in bfrange block in ToUnicode CMap"
#define S_ILLEGAL_UNICODE_CMAP "Illegal entry in ToUnicode CMap"

// CMap.cpp
#define S_FIND_CMAP_FILE "Couldn't find '%s' CMap file for '%s' collection"
#define S_ILLEGAL_CODESPACERANGE_CMAP "Illegal entry in codespacerange block in CMap"
#define S_ILLEGAL_CIDRANGE_CMAP "Illegal entry in cidrange block in CMap"
#define S_COLLISION_USECMAP "Collision in usecmap"
#define S_INVALID_CID_CMAP "Invalid CID (%*x - %*x) in CMap"

// Function.cpp
/*
#define S_EXP_FUNCTION "Expected function dictionary or stream"
#define S_FUNCTION_WRONG_TYPE "Function type is missing or wrong type"
#define S_UNI_FUNCTION_TYPE "Unimplemented function type (%d)"
#define S_FUNCTION_MISSING_DOMAIN "Function is missing domain"
#define S_FUNCTIONS_UNSUPPORTED "Functions with more than %d inputs are unsupported"
#define S_ILL_FUNC_DOMAIN_ARRAY "Illegal value in function domain array"
#define S_ILL_FUNC_RANGE_ARRAY "Illegal value in function range array"
#define S_TYPE0_MISS_RANGE "Type 0 function is missing range"
#define S_TYPE0_NOT_STREAM "Type 0 function isn't a stream"
#define S_FUNC_MISS_SIZE_ARR "Function has missing or invalid size array"
#define S_ILL_FUNC_SIZE_ARRAY "Illegal value in function size array"
#define S_FUNC_MISS_BITSPERSAMPLE "Function has missing or invalid BitsPerSample"
#define S_ILL_ENCODE_ARRAY "Illegal value in function encode array"
#define S_ILL_DECODE_ARRAY "Illegal value in function decode array"
#define S_EXP_FUNC_MORE_1_INPUT "Exponential function with more than one input"
#define S_FUNC_CO_ARRAY_BAD_LEN "Function's C0 array is wrong length"
#define S_ILL_VALUE_FUNC_CO_ARRAY "Illegal value in function C0 array"
#define S_FUNC_C1_ARRAY_BAD_LEN "Function's C1 array is wrong length"
#define S_ILL_VALUE_FUNC_C1_ARRAY "Illegal value in function C1 array"
#define S_FUNC_MISS_N "Function has missing or invalid N"
*/

void error(int pos, const char *msg, ...);

extern int GetError();
extern void SetError(int aCode);

#endif

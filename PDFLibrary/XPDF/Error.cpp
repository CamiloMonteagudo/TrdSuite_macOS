//========================================================================
//
// Error.cc
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include "GlobalParams.h"
#include "Error.h"

int iErrorCode;

//void CDECL error(int pos, char *msg, ...) {
void error(int pos, const char *msg, ...) {
  if (pos != -1) SetError(pos);

#ifdef _DEBUG
  va_list args;

  // NB: this can be called before the globalParams object is created
  if (globalParams && globalParams->getErrQuiet()) {
    return;
  }
  if (pos >= 0) {
    fprintf(stderr, "Error (%d): ", pos);
  } else {
    fprintf(stderr, "Error: ");
  }
  va_start(args, msg);
  vfprintf(stderr, msg, args);
  va_end(args);
  fprintf(stderr, "\n");
  fflush(stderr);
#endif
}

int GetError() {
	return iErrorCode;
};

void SetError(int aCode) {
	iErrorCode = aCode;
}
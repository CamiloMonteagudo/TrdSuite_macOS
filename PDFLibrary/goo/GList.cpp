//========================================================================
//
// GList.cc
//
// Copyright 2001-2003 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <string.h>
#include <stdlib.h>
#include "gmem.h"
#include "GList.h"

//------------------------------------------------------------------------
// GList
//------------------------------------------------------------------------

GList::GList() {
  size = 8;
  data = (void **)gmalloc(size * sizeof(void*));
  length = 0;
  inc = 0;
}

GList::GList(int sizeA) {
  size = sizeA;
  data = (void **)gmalloc(size * sizeof(void*));
  length = 0;
  inc = 0;
}

GList::~GList() {
  gfree(data);
}

void GList::append(void *p) {
  if (length >= size) {
    expand();
  }
  data[length++] = p;
}

void GList::append(GList *list) {
  int i;

  while (length + list->length > size) {
    expand();
  }
  for (i = 0; i < list->length; ++i) {
    data[length++] = list->data[i];
  }
}

void GList::insert(int i, void *p) {
  if (length >= size) {
    expand();
  }
  if (i < length) {
    memmove(data+i+1, data+i, (length - i) * sizeof(void *));
  } else {
		append(p);
		return;
	}
  data[i] = p;
  ++length;
}

void GList::set(int i, void *p) {
  if (i < length) {
		data[i] = p;
	}
}

void *GList::del(int i) {
  void *p;

  p = data[i];
  if (i < length - 1) {
    memmove(data+i, data+i+1, (length - i - 1) * sizeof(void *));
  }
  --length;
  if (size - length >= ((inc > 0) ? inc : size/2)) {
    shrink();
  }
  return p;
}

int GList::indexOf(void *p) {
	for(int i=0; i<length; i++) {
		if (data[i] == p)
			return i;
	}
	
	return -1;
}

void GList::clear(void) {
  length = 0;
  if (size - length >= ((inc > 0) ? inc : size/2)) {
    shrink();
  }
}

void GList::sort(int (__cdecl *compare )(const void *elem1, const void *elem2 )) {
  qsort(data, length, sizeof(void *), compare);
}

void GList::expand() {
  size += (inc > 0) ? inc : size;
  data = (void **)grealloc(data, size * sizeof(void*));
}

void GList::shrink() {
  size -= (inc > 0) ? inc : size/2;
  data = (void **)grealloc(data, size * sizeof(void*));
}

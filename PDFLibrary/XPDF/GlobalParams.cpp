//========================================================================
//
// GlobalParams.cc
//
// Copyright 2001-2003 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#if HAVE_PAPER_H
#include <paper.h>
#endif
#include "gmem.h"
#include "GString.h"
#include "GList.h"
#include "GHash.h"
#include "gfile.h"
#include "Error.h"
#include "NameToCharCode.h"
#include "CharCodeToUnicode.h"
#include "UnicodeMap.h"
#include "CMap.h"
#include "BuiltinFontTables.h"
#include "FontEncodingTables.h"
#include "GlobalParams.h"

#if MULTITHREADED
#  define lockGlobalParams            gLockMutex(&mutex)
#  define lockUnicodeMapCache         gLockMutex(&unicodeMapCacheMutex)
#  define lockCMapCache               gLockMutex(&cMapCacheMutex)
#  define unlockGlobalParams          gUnlockMutex(&mutex)
#  define unlockUnicodeMapCache       gUnlockMutex(&unicodeMapCacheMutex)
#  define unlockCMapCache             gUnlockMutex(&cMapCacheMutex)
#else
#  define lockGlobalParams
#  define lockUnicodeMapCache
#  define lockCMapCache
#  define unlockGlobalParams
#  define unlockUnicodeMapCache
#  define unlockCMapCache
#endif

#include "NameToUnicodeTable.h"
#include "UnicodeMapTables.h"
#include "UTF8.h"

//------------------------------------------------------------------------

#define cidToUnicodeCacheSize     4
#define unicodeToUnicodeCacheSize 4

//------------------------------------------------------------------------

static struct {
  const char *name;
  const char *fileName;
} displayFontTab[] = {
  {"Courier",               "n022003l.pfb"},
  {"Courier-Bold",          "n022004l.pfb"},
  {"Courier-BoldOblique",   "n022024l.pfb"},
  {"Courier-Oblique",       "n022023l.pfb"},
  {"Helvetica",             "n019003l.pfb"},
  {"Helvetica-Bold",        "n019004l.pfb"},
  {"Helvetica-BoldOblique", "n019024l.pfb"},
  {"Helvetica-Oblique",     "n019023l.pfb"},
  {"Symbol",                "s050000l.pfb"},
  {"Times-Bold",            "n021004l.pfb"},
  {"Times-BoldItalic",      "n021024l.pfb"},
  {"Times-Italic",          "n021023l.pfb"},
  {"Times-Roman",           "n021003l.pfb"},
  {"ZapfDingbats",          "d050000l.pfb"},
  {NULL}
};

static const char *displayFontDirs[] = {
  "/usr/share/ghostscript/fonts",
  "/usr/local/share/ghostscript/fonts",
  "/usr/share/fonts/default/Type1",
  NULL
};

//------------------------------------------------------------------------

GlobalParams *globalParams = NULL;

//------------------------------------------------------------------------
// DisplayFontParam
//------------------------------------------------------------------------

DisplayFontParam::DisplayFontParam(GString *nameA,
				   DisplayFontParamKind kindA) {
  name = nameA;
  kind = kindA;
  switch (kind) {
  case displayFontT1:
    t1.fileName = NULL;
    break;
  case displayFontTT:
    tt.fileName = NULL;
    break;
  }
}

DisplayFontParam::~DisplayFontParam() {
  delete name;
  switch (kind) {
  case displayFontT1:
    if (t1.fileName) {
      delete t1.fileName;
    }
    break;
  case displayFontTT:
    if (tt.fileName) {
      delete tt.fileName;
    }
    break;
  }
}

//------------------------------------------------------------------------
// PSFontParam
//------------------------------------------------------------------------

PSFontParam::PSFontParam(GString *pdfFontNameA, int wModeA,
			 GString *psFontNameA, GString *encodingA) {
  pdfFontName = pdfFontNameA;
  wMode = wModeA;
  psFontName = psFontNameA;
  encoding = encodingA;
}

PSFontParam::~PSFontParam() {
  delete pdfFontName;
  delete psFontName;
  if (encoding) {
    delete encoding;
  }
}

//------------------------------------------------------------------------
// parsing
//------------------------------------------------------------------------

GlobalParams::GlobalParams(const char *cfgFileName) {
  UnicodeMap *map;
  GString *fileName;
  FILE *f;
  int i;

#if MULTITHREADED
  gInitMutex(&mutex);
  gInitMutex(&unicodeMapCacheMutex);
  gInitMutex(&cMapCacheMutex);
#endif

  initBuiltinFontTables();

  // scan the encoding in reverse because we want the lowest-numbered
  // index for each char name ('space' is encoded twice)
  macRomanReverseMap = new NameToCharCode();
  for (i = 255; i >= 0; --i) {
    if (macRomanEncoding[i]) {
      macRomanReverseMap->add(macRomanEncoding[i], (CharCode)i);
    }
  }

  nameToUnicode = new NameToCharCode();
  cidToUnicodes = new GHash(gTrue);
  unicodeToUnicodes = new GHash(gTrue);
  residentUnicodeMaps = new GHash();
  unicodeMaps = new GHash(gTrue);
  cMapDirs = new GHash(gTrue);
  toUnicodeDirs = new GList();
  displayFonts = new GHash();
  displayCIDFonts = new GHash();
  displayNamedCIDFonts = new GHash();
#if HAVE_PAPER_H
  char *paperName;
  const struct paper *paperType;
  paperinit();
  if ((paperName = systempapername())) {
    paperType = paperinfo(paperName);
    psPaperWidth = (int)paperpswidth(paperType);
    psPaperHeight = (int)paperpsheight(paperType);
  } else {
    error(-1, "No paper information available - using defaults");
    psPaperWidth = defPaperWidth;
    psPaperHeight = defPaperHeight;
  }
  paperdone();
#else
  psPaperWidth = defPaperWidth;
  psPaperHeight = defPaperHeight;
#endif
  psImageableLLX = psImageableLLY = 0;
  psImageableURX = psPaperWidth;
  psImageableURY = psPaperHeight;
  psCrop = gTrue;
  psExpandSmaller = gFalse;
  psShrinkLarger = gTrue;
  psCenter = gTrue;
  psDuplex = gFalse;
  psLevel = psLevel2;
  psFile = NULL;
  psFonts = new GHash();
  psNamedFonts16 = new GList();
  psFonts16 = new GList();
  psEmbedType1 = gTrue;
  psEmbedTrueType = gTrue;
  psEmbedCIDPostScript = gTrue;
  psEmbedCIDTrueType = gTrue;
  psOPI = gFalse;
  psASCIIHex = gFalse;
  textEncoding = new GString("Latin1");
#if defined(WIN32)
  textEOL = eolDOS;
#elif defined(MACOS)
  textEOL = eolMac;
#else
  textEOL = eolUnix;
#endif
  textPageBreaks = gTrue;
  textKeepTinyChars = gFalse;
  fontDirs = new GList();
  initialZoom = new GString("125");
  enableT1lib = gTrue;
  enableFreeType = gTrue;
  antialias = gTrue;
  urlCommand = NULL;
  movieCommand = NULL;
  mapNumericCharNames = gTrue;
  printCommands = gFalse;
  errQuiet = gFalse;

  cidToUnicodeCache = new CharCodeToUnicodeCache(cidToUnicodeCacheSize);
  unicodeToUnicodeCache =
      new CharCodeToUnicodeCache(unicodeToUnicodeCacheSize);
  unicodeMapCache = new UnicodeMapCache();
  cMapCache = new CMapCache();

  // set up the initial nameToUnicode table
  for (i = 0; nameToUnicodeTab[i].name; ++i) {
    nameToUnicode->add(nameToUnicodeTab[i].name, nameToUnicodeTab[i].u);
  }

  // set up the residentUnicodeMaps table
  map = new UnicodeMap("Latin1", gFalse,
		       latin1UnicodeMapRanges, latin1UnicodeMapLen);
  residentUnicodeMaps->add(map->getEncodingName(), map);
  map = new UnicodeMap("ASCII7", gFalse,
		       ascii7UnicodeMapRanges, ascii7UnicodeMapLen);
  residentUnicodeMaps->add(map->getEncodingName(), map);
  map = new UnicodeMap("Symbol", gFalse,
		       symbolUnicodeMapRanges, symbolUnicodeMapLen);
  residentUnicodeMaps->add(map->getEncodingName(), map);
  map = new UnicodeMap("ZapfDingbats", gFalse, zapfDingbatsUnicodeMapRanges,
		       zapfDingbatsUnicodeMapLen);
  residentUnicodeMaps->add(map->getEncodingName(), map);
  map = new UnicodeMap("UTF-8", gTrue, &mapUTF8);
  residentUnicodeMaps->add(map->getEncodingName(), map);
  map = new UnicodeMap("UCS-2", gTrue, &mapUCS2);
  residentUnicodeMaps->add(map->getEncodingName(), map);

  // look for a user config file, then a system-wide config file
  f = NULL;
  fileName = NULL;
  if (cfgFileName && cfgFileName[0]) {
    fileName = new GString(cfgFileName);
    if (!(f = fopen(fileName->getCString(), "r"))) {
      delete fileName;
    }
  }
  if (!f) {
    fileName = appendToPath(getHomeDir(), xpdfUserConfigFile);
    if (!(f = fopen(fileName->getCString(), "r"))) {
      delete fileName;
    }
  }
  if (!f) {
#if defined(WIN32) && !defined(__CYGWIN32__)
    char buf[512];
    i = GetModuleFileName(NULL, buf, sizeof(buf));
    if (i <= 0 || i >= sizeof(buf)) {
      // error or path too long for buffer - just use the current dir
      buf[0] = '\0';
    }
    fileName = grabPath(buf);
    appendToPath(fileName, xpdfSysConfigFile);
#else
    fileName = new GString(xpdfSysConfigFile);
#endif
    if (!(f = fopen(fileName->getCString(), "r"))) {
      delete fileName;
    }
  }
  if (f) {
    parseFile(fileName, f);
    delete fileName;
    fclose(f);
  }
}

void GlobalParams::parseFile(GString *fileName, FILE *f) {
  int line;
  GList *tokens;
  GString *cmd, *incFile;
  char *p1, *p2;
  char buf[512];
  FILE *f2;

  line = 1;
  while (getLine(buf, sizeof(buf) - 1, f)) {

    // break the line into tokens
    tokens = new GList();
    p1 = buf;
    while (*p1) {
      for (; *p1 && isspace(*p1); ++p1) ;
      if (!*p1) {
	break;
      }
      if (*p1 == '"' || *p1 == '\'') {
	for (p2 = p1 + 1; *p2 && *p2 != *p1; ++p2) ;
	++p1;
      } else {
	for (p2 = p1 + 1; *p2 && !isspace(*p2); ++p2) ;
      }
      tokens->append(new GString(p1, p2 - p1));
      p1 = *p2 ? p2 + 1 : p2;
    }

    if (tokens->getLength() > 0 &&
	((GString *)tokens->get(0))->getChar(0) != '#') {
      cmd = (GString *)tokens->get(0);
      if (!cmd->cmp("include")) {
	if (tokens->getLength() == 2) {
	  incFile = (GString *)tokens->get(1);
	  if ((f2 = fopen(incFile->getCString(), "r"))) {
	    parseFile(incFile, f2);
	    fclose(f2);
	  } else {
	    error(-1, "Couldn't find included config file: '%s' (%s:%d)",
		  incFile->getCString(), fileName->getCString(), line);
	  }
	} else {
	  error(-1, "Bad 'include' config file command (%s:%d)",
		fileName->getCString(), line);
	}
      } else if (!cmd->cmp("nameToUnicode")) {
	parseNameToUnicode(tokens, fileName, line);
      } else if (!cmd->cmp("cidToUnicode")) {
	parseCIDToUnicode(tokens, fileName, line);
      } else if (!cmd->cmp("unicodeToUnicode")) {
	parseUnicodeToUnicode(tokens, fileName, line);
      } else if (!cmd->cmp("unicodeMap")) {
	parseUnicodeMap(tokens, fileName, line);
      } else if (!cmd->cmp("cMapDir")) {
	parseCMapDir(tokens, fileName, line);
      } else if (!cmd->cmp("toUnicodeDir")) {
	parseToUnicodeDir(tokens, fileName, line);
      } else if (!cmd->cmp("displayFontT1")) {
	parseDisplayFont(tokens, displayFonts, displayFontT1, fileName, line);
      } else if (!cmd->cmp("displayFontTT")) {
	parseDisplayFont(tokens, displayFonts, displayFontTT, fileName, line);
      } else if (!cmd->cmp("displayNamedCIDFontT1")) {
	parseDisplayFont(tokens, displayNamedCIDFonts,
			 displayFontT1, fileName, line);
      } else if (!cmd->cmp("displayCIDFontT1")) {
	parseDisplayFont(tokens, displayCIDFonts,
			 displayFontT1, fileName, line);
      } else if (!cmd->cmp("displayNamedCIDFontTT")) {
	parseDisplayFont(tokens, displayNamedCIDFonts,
			 displayFontTT, fileName, line);
      } else if (!cmd->cmp("displayCIDFontTT")) {
	parseDisplayFont(tokens, displayCIDFonts,
			 displayFontTT, fileName, line);
      } else if (!cmd->cmp("psFile")) {
	parsePSFile(tokens, fileName, line);
      } else if (!cmd->cmp("psFont")) {
	parsePSFont(tokens, fileName, line);
      } else if (!cmd->cmp("psNamedFont16")) {
	parsePSFont16("psNamedFont16", psNamedFonts16,
		      tokens, fileName, line);
      } else if (!cmd->cmp("psFont16")) {
	parsePSFont16("psFont16", psFonts16, tokens, fileName, line);
      } else if (!cmd->cmp("psPaperSize")) {
	parsePSPaperSize(tokens, fileName, line);
      } else if (!cmd->cmp("psImageableArea")) {
	parsePSImageableArea(tokens, fileName, line);
      } else if (!cmd->cmp("psCrop")) {
	parseYesNo("psCrop", &psCrop, tokens, fileName, line);
      } else if (!cmd->cmp("psExpandSmaller")) {
	parseYesNo("psExpandSmaller", &psExpandSmaller,
		   tokens, fileName, line);
      } else if (!cmd->cmp("psShrinkLarger")) {
	parseYesNo("psShrinkLarger", &psShrinkLarger, tokens, fileName, line);
      } else if (!cmd->cmp("psCenter")) {
	parseYesNo("psCenter", &psCenter, tokens, fileName, line);
      } else if (!cmd->cmp("psDuplex")) {
	parseYesNo("psDuplex", &psDuplex, tokens, fileName, line);
      } else if (!cmd->cmp("psLevel")) {
	parsePSLevel(tokens, fileName, line);
      } else if (!cmd->cmp("psEmbedType1Fonts")) {
	parseYesNo("psEmbedType1", &psEmbedType1, tokens, fileName, line);
      } else if (!cmd->cmp("psEmbedTrueTypeFonts")) {
	parseYesNo("psEmbedTrueType", &psEmbedTrueType,
		   tokens, fileName, line);
      } else if (!cmd->cmp("psEmbedCIDPostScriptFonts")) {
	parseYesNo("psEmbedCIDPostScript", &psEmbedCIDPostScript,
		   tokens, fileName, line);
      } else if (!cmd->cmp("psEmbedCIDTrueTypeFonts")) {
	parseYesNo("psEmbedCIDTrueType", &psEmbedCIDTrueType,
		   tokens, fileName, line);
      } else if (!cmd->cmp("psOPI")) {
	parseYesNo("psOPI", &psOPI, tokens, fileName, line);
      } else if (!cmd->cmp("psASCIIHex")) {
	parseYesNo("psASCIIHex", &psASCIIHex, tokens, fileName, line);
      } else if (!cmd->cmp("textEncoding")) {
	parseTextEncoding(tokens, fileName, line);
      } else if (!cmd->cmp("textEOL")) {
	parseTextEOL(tokens, fileName, line);
      } else if (!cmd->cmp("textPageBreaks")) {
	parseYesNo("textPageBreaks", &textPageBreaks,
		   tokens, fileName, line);
      } else if (!cmd->cmp("textKeepTinyChars")) {
	parseYesNo("textKeepTinyChars", &textKeepTinyChars,
		   tokens, fileName, line);
      } else if (!cmd->cmp("fontDir")) {
	parseFontDir(tokens, fileName, line);
      } else if (!cmd->cmp("initialZoom")) {
	parseInitialZoom(tokens, fileName, line);
      } else if (!cmd->cmp("enableT1lib")) {
	parseYesNo("enableT1lib", &enableT1lib, tokens, fileName, line);
      } else if (!cmd->cmp("enableFreeType")) {
	parseYesNo("enableFreeType", &enableFreeType, tokens, fileName, line);
      } else if (!cmd->cmp("antialias")) {
	parseYesNo("antialias", &antialias, tokens, fileName, line);
      } else if (!cmd->cmp("urlCommand")) {
	parseCommand("urlCommand", &urlCommand, tokens, fileName, line);
      } else if (!cmd->cmp("movieCommand")) {
	parseCommand("movieCommand", &movieCommand, tokens, fileName, line);
      } else if (!cmd->cmp("mapNumericCharNames")) {
	parseYesNo("mapNumericCharNames", &mapNumericCharNames,
		   tokens, fileName, line);
      } else if (!cmd->cmp("printCommands")) {
	parseYesNo("printCommands", &printCommands, tokens, fileName, line);
      } else if (!cmd->cmp("errQuiet")) {
	parseYesNo("errQuiet", &errQuiet, tokens, fileName, line);
      } else {
	error(-1, "Unknown config file command '%s' (%s:%d)",
	      cmd->getCString(), fileName->getCString(), line);
	if (!cmd->cmp("displayFontX") ||
	    !cmd->cmp("displayNamedCIDFontX") ||
	    !cmd->cmp("displayCIDFontX")) {
	  error(-1, "-- Xpdf no longer supports X fonts");
	} else if (!cmd->cmp("t1libControl") || !cmd->cmp("freetypeControl")) {
	  error(-1, "-- The t1libControl and freetypeControl options have been replaced");
	  error(-1, "   by the enableT1lib, enableFreeType, and antialias options");
	} else if (!cmd->cmp("fontpath") || !cmd->cmp("fontmap")) {
	  error(-1, "-- the config file format has changed since Xpdf 0.9x");
	}
      }
    }

    deleteGList(tokens, GString);
    ++line;
  }
}

void GlobalParams::parseNameToUnicode(GList *tokens, GString *fileName,
					 int line) {
  GString *name;
  char *tok1, *tok2;
  FILE *f;
  char buf[256];
  int line2;
  Unicode u;

  if (tokens->getLength() != 2) {
    error(-1, "Bad 'nameToUnicode' config file command (%s:%d)",
	  fileName->getCString(), line);
    return;
  }
  name = (GString *)tokens->get(1);
  if (!(f = fopen(name->getCString(), "r"))) {
    error(-1, "Couldn't open 'nameToUnicode' file '%s'",
	  name->getCString());
    return;
  }
  line2 = 1;
  while (getLine(buf, sizeof(buf), f)) {
    tok1 = strtok(buf, " \t\r\n");
    tok2 = strtok(NULL, " \t\r\n");
    if (tok1 && tok2) {
      sscanf(tok1, "%x", &u);
      nameToUnicode->add(tok2, u);
    } else {
      error(-1, "Bad line in 'nameToUnicode' file (%s:%d)", name, line2);
    }
    ++line2;
  }
  fclose(f);
}

void GlobalParams::parseCIDToUnicode(GList *tokens, GString *fileName,
				     int line) {
  GString *collection, *name, *old;

  if (tokens->getLength() != 3) {
    error(-1, "Bad 'cidToUnicode' config file command (%s:%d)",
	  fileName->getCString(), line);
    return;
  }
  collection = (GString *)tokens->get(1);
  name = (GString *)tokens->get(2);
  if ((old = (GString *)cidToUnicodes->remove(collection))) {
    delete old;
  }
  cidToUnicodes->add(collection->copy(), name->copy());
}

void GlobalParams::parseUnicodeToUnicode(GList *tokens, GString *fileName,
					 int line) {
  GString *font, *file, *old;

  if (tokens->getLength() != 3) {
    error(-1, "Bad 'unicodeToUnicode' config file command (%s:%d)",
	  fileName->getCString(), line);
    return;
  }
  font = (GString *)tokens->get(1);
  file = (GString *)tokens->get(2);
  if ((old = (GString *)unicodeToUnicodes->remove(font))) {
    delete old;
  }
  unicodeToUnicodes->add(font->copy(), file->copy());
}

void GlobalParams::parseUnicodeMap(GList *tokens, GString *fileName,
				   int line) {
  GString *encodingName, *name, *old;

  if (tokens->getLength() != 3) {
    error(-1, "Bad 'unicodeMap' config file command (%s:%d)",
	  fileName->getCString(), line);
    return;
  }
  encodingName = (GString *)tokens->get(1);
  name = (GString *)tokens->get(2);
  if ((old = (GString *)unicodeMaps->remove(encodingName))) {
    delete old;
  }
  unicodeMaps->add(encodingName->copy(), name->copy());
}

void GlobalParams::parseCMapDir(GList *tokens, GString *fileName, int line) {
  GString *collection, *dir;
  GList *list;

  if (tokens->getLength() != 3) {
    error(-1, "Bad 'cMapDir' config file command (%s:%d)",
	  fileName->getCString(), line);
    return;
  }
  collection = (GString *)tokens->get(1);
  dir = (GString *)tokens->get(2);
  if (!(list = (GList *)cMapDirs->lookup(collection))) {
    list = new GList();
    cMapDirs->add(collection->copy(), list);
  }
  list->append(dir->copy());
}

void GlobalParams::parseToUnicodeDir(GList *tokens, GString *fileName,
				     int line) {
  if (tokens->getLength() != 2) {
    error(-1, "Bad 'toUnicodeDir' config file command (%s:%d)",
	  fileName->getCString(), line);
    return;
  }
  toUnicodeDirs->append(((GString *)tokens->get(1))->copy());
}

void GlobalParams::parseDisplayFont(GList *tokens, GHash *fontHash,
				    DisplayFontParamKind kind,
				    GString *fileName, int line) {
  DisplayFontParam *param, *old;

  if (tokens->getLength() < 2) {
    goto err1;
  }
  param = new DisplayFontParam(((GString *)tokens->get(1))->copy(), kind);
  
  switch (kind) {
  case displayFontT1:
    if (tokens->getLength() != 3) {
      goto err2;
    }
    param->t1.fileName = ((GString *)tokens->get(2))->copy();
    break;
  case displayFontTT:
    if (tokens->getLength() != 3) {
      goto err2;
    }
    param->tt.fileName = ((GString *)tokens->get(2))->copy();
    break;
  }

  if ((old = (DisplayFontParam *)fontHash->remove(param->name))) {
    delete old;
  }
  fontHash->add(param->name, param);
  return;

 err2:
  delete param;
 err1:
  error(-1, "Bad 'display*Font*' config file command (%s:%d)",
	fileName->getCString(), line);
}

void GlobalParams::parsePSPaperSize(GList *tokens, GString *fileName,
				    int line) {
  GString *tok;

  if (tokens->getLength() == 2) {
    tok = (GString *)tokens->get(1);
    if (!setPSPaperSize(tok->getCString())) {
      error(-1, "Bad 'psPaperSize' config file command (%s:%d)",
	    fileName->getCString(), line);
    }
  } else if (tokens->getLength() == 3) {
    tok = (GString *)tokens->get(1);
    psPaperWidth = atoi(tok->getCString());
    tok = (GString *)tokens->get(2);
    psPaperHeight = atoi(tok->getCString());
    psImageableLLX = psImageableLLY = 0;
    psImageableURX = psPaperWidth;
    psImageableURY = psPaperHeight;
  } else {
    error(-1, "Bad 'psPaperSize' config file command (%s:%d)",
	  fileName->getCString(), line);
  }
}

void GlobalParams::parsePSImageableArea(GList *tokens, GString *fileName,
					int line) {
  if (tokens->getLength() != 5) {
    error(-1, "Bad 'psImageableArea' config file command (%s:%d)",
	  fileName->getCString(), line);
    return;
  }
  psImageableLLX = atoi(((GString *)tokens->get(1))->getCString());
  psImageableLLY = atoi(((GString *)tokens->get(2))->getCString());
  psImageableURX = atoi(((GString *)tokens->get(3))->getCString());
  psImageableURY = atoi(((GString *)tokens->get(4))->getCString());
}

void GlobalParams::parsePSLevel(GList *tokens, GString *fileName, int line) {
  GString *tok;

  if (tokens->getLength() != 2) {
    error(-1, "Bad 'psLevel' config file command (%s:%d)",
	  fileName->getCString(), line);
    return;
  }
  tok = (GString *)tokens->get(1);
  if (!tok->cmp("level1")) {
    psLevel = psLevel1;
  } else if (!tok->cmp("level1sep")) {
    psLevel = psLevel1Sep;
  } else if (!tok->cmp("level2")) {
    psLevel = psLevel2;
  } else if (!tok->cmp("level2sep")) {
    psLevel = psLevel2Sep;
  } else if (!tok->cmp("level3")) {
    psLevel = psLevel3;
  } else if (!tok->cmp("level3Sep")) {
    psLevel = psLevel3Sep;
  } else {
    error(-1, "Bad 'psLevel' config file command (%s:%d)",
	  fileName->getCString(), line);
  }
}

void GlobalParams::parsePSFile(GList *tokens, GString *fileName, int line) {
  if (tokens->getLength() != 2) {
    error(-1, "Bad 'psFile' config file command (%s:%d)",
	  fileName->getCString(), line);
    return;
  }
  if (psFile) {
    delete psFile;
  }
  psFile = ((GString *)tokens->get(1))->copy();
}

void GlobalParams::parsePSFont(GList *tokens, GString *fileName, int line) {
  PSFontParam *param;

  if (tokens->getLength() != 3) {
    error(-1, "Bad 'psFont' config file command (%s:%d)",
	  fileName->getCString(), line);
    return;
  }
  param = new PSFontParam(((GString *)tokens->get(1))->copy(), 0,
			  ((GString *)tokens->get(2))->copy(), NULL);
  psFonts->add(param->pdfFontName, param);
}

void GlobalParams::parsePSFont16(const char *cmdName, GList *fontList,
				 GList *tokens, GString *fileName, int line) {
  PSFontParam *param;
  int wMode;
  GString *tok;

  if (tokens->getLength() != 5) {
    error(-1, "Bad '%s' config file command (%s:%d)",
	  cmdName, fileName->getCString(), line);
    return;
  }
  tok = (GString *)tokens->get(2);
  if (!tok->cmp("H")) {
    wMode = 0;
  } else if (!tok->cmp("V")) {
    wMode = 1;
  } else {
    error(-1, "Bad '%s' config file command (%s:%d)",
	  cmdName, fileName->getCString(), line);
    return;
  }
  param = new PSFontParam(((GString *)tokens->get(1))->copy(),
			  wMode,
			  ((GString *)tokens->get(3))->copy(),
			  ((GString *)tokens->get(4))->copy());
  fontList->append(param);
}

void GlobalParams::parseTextEncoding(GList *tokens, GString *fileName,
				     int line) {
  if (tokens->getLength() != 2) {
    error(-1, "Bad 'textEncoding' config file command (%s:%d)",
	  fileName->getCString(), line);
    return;
  }
  delete textEncoding;
  textEncoding = ((GString *)tokens->get(1))->copy();
}

void GlobalParams::parseTextEOL(GList *tokens, GString *fileName, int line) {
  GString *tok;

  if (tokens->getLength() != 2) {
    error(-1, "Bad 'textEOL' config file command (%s:%d)",
	  fileName->getCString(), line);
    return;
  }
  tok = (GString *)tokens->get(1);
  if (!tok->cmp("unix")) {
    textEOL = eolUnix;
  } else if (!tok->cmp("dos")) {
    textEOL = eolDOS;
  } else if (!tok->cmp("mac")) {
    textEOL = eolMac;
  } else {
    error(-1, "Bad 'textEOL' config file command (%s:%d)",
	  fileName->getCString(), line);
  }
}

void GlobalParams::parseFontDir(GList *tokens, GString *fileName, int line) {
  if (tokens->getLength() != 2) {
    error(-1, "Bad 'fontDir' config file command (%s:%d)",
	  fileName->getCString(), line);
    return;
  }
  fontDirs->append(((GString *)tokens->get(1))->copy());
}

void GlobalParams::parseInitialZoom(GList *tokens,
				    GString *fileName, int line) {
  if (tokens->getLength() != 2) {
    error(-1, "Bad 'initialZoom' config file command (%s:%d)",
	  fileName->getCString(), line);
    return;
  }
  delete initialZoom;
  initialZoom = ((GString *)tokens->get(1))->copy();
}

void GlobalParams::parseCommand(const char *cmdName, GString **val,
				GList *tokens, GString *fileName, int line) {
  if (tokens->getLength() != 2) {
    error(-1, "Bad '%s' config file command (%s:%d)",
	  cmdName, fileName->getCString(), line);
    return;
  }
  if (*val) {
    delete *val;
  }
  *val = ((GString *)tokens->get(1))->copy();
}

void GlobalParams::parseYesNo(const char *cmdName, GBool *flag,
			      GList *tokens, GString *fileName, int line) {
  GString *tok;

  if (tokens->getLength() != 2) {
    error(-1, "Bad '%s' config file command (%s:%d)",
	  cmdName, fileName->getCString(), line);
    return;
  }
  tok = (GString *)tokens->get(1);
  if (!parseYesNo2(tok->getCString(), flag)) {
    error(-1, "Bad '%s' config file command (%s:%d)",
	  cmdName, fileName->getCString(), line);
  }
}

GBool GlobalParams::parseYesNo2(char *token, GBool *flag) {
  if (!strcmp(token, "yes")) {
    *flag = gTrue;
  } else if (!strcmp(token, "no")) {
    *flag = gFalse;
  } else {
    return gFalse;
  }
  return gTrue;
}

GlobalParams::~GlobalParams() {
  GHashIter *iter;
  GString *key;
  GList *list;

  freeBuiltinFontTables();

  delete macRomanReverseMap;

  delete nameToUnicode;
  deleteGHash(cidToUnicodes, GString);
  deleteGHash(unicodeToUnicodes, GString);
  deleteGHash(residentUnicodeMaps, UnicodeMap);
  deleteGHash(unicodeMaps, GString);
  deleteGList(toUnicodeDirs, GString);
  deleteGHash(displayFonts, DisplayFontParam);
  deleteGHash(displayCIDFonts, DisplayFontParam);
  deleteGHash(displayNamedCIDFonts, DisplayFontParam);
  if (psFile) {
    delete psFile;
  }
  deleteGHash(psFonts, PSFontParam);
  deleteGList(psNamedFonts16, PSFontParam);
  deleteGList(psFonts16, PSFontParam);
  delete textEncoding;
  deleteGList(fontDirs, GString);
  delete initialZoom;
  if (urlCommand) {
    delete urlCommand;
  }
  if (movieCommand) {
    delete movieCommand;
  }

  cMapDirs->startIter(&iter);
  while (cMapDirs->getNext(&iter, &key, (void **)&list)) {
    deleteGList(list, GString);
  }
  delete cMapDirs;

  delete cidToUnicodeCache;
  delete unicodeToUnicodeCache;
  delete unicodeMapCache;
  delete cMapCache;

#if MULTITHREADED
  gDestroyMutex(&mutex);
  gDestroyMutex(&unicodeMapCacheMutex);
  gDestroyMutex(&cMapCacheMutex);
#endif
}

//------------------------------------------------------------------------

void GlobalParams::setupBaseFonts(char *dir) {
  GString *fontName;
  GString *fileName;
  FILE *f;
  DisplayFontParam *dfp;
  int i;

  for (i = 0; displayFontTab[i].name; ++i) {
    fontName = new GString(displayFontTab[i].name);
    if (getDisplayFont(fontName)) {
      delete fontName;
      continue;
    }
    fileName = NULL;
    if (dir) {
      fileName = appendToPath(new GString(dir), displayFontTab[i].fileName);
      if ((f = fopen(fileName->getCString(), "rb"))) {
	fclose(f);
      } else {
	delete fileName;
	fileName = NULL;
      }
    }
#ifndef WIN32
    for (int j = 0; !fileName && displayFontDirs[j]; ++j) {
      fileName = appendToPath(new GString(displayFontDirs[j]),
			      displayFontTab[i].fileName);
      if ((f = fopen(fileName->getCString(), "rb"))) {
	fclose(f);
      } else {
	delete fileName;
	fileName = NULL;
      }
    }
#endif
    if (!fileName) {
      error(-1, "No display font for '%s'", displayFontTab[i].name);
      delete fontName;
      continue;
    }
    dfp = new DisplayFontParam(fontName, displayFontT1);
    dfp->t1.fileName = fileName;
    globalParams->addDisplayFont(dfp);
  }
}

//------------------------------------------------------------------------
// accessors
//------------------------------------------------------------------------

CharCode GlobalParams::getMacRomanCharCode(const char *charName) {
  // no need to lock - macRomanReverseMap is constant
  return macRomanReverseMap->lookup(charName);
}

Unicode GlobalParams::mapNameToUnicode(const char *charName) {
  // no need to lock - nameToUnicode is constant
  return nameToUnicode->lookup(charName);
}

UnicodeMap *GlobalParams::getResidentUnicodeMap(GString *encodingName) {
  UnicodeMap *map;

  lockGlobalParams;
  map = (UnicodeMap *)residentUnicodeMaps->lookup(encodingName);
  unlockGlobalParams;
  if (map) {
    map->incRefCnt();
  }
  return map;
}

FILE *GlobalParams::getUnicodeMapFile(GString *encodingName) {
  GString *fileName;
  FILE *f;

  lockGlobalParams;
  if ((fileName = (GString *)unicodeMaps->lookup(encodingName))) {
    f = fopen(fileName->getCString(), "r");
  } else {
    f = NULL;
  }
  unlockGlobalParams;
  return f;
}

FILE *GlobalParams::findCMapFile(GString *collection, GString *cMapName) {
  GList *list;
  GString *dir;
  GString *fileName;
  FILE *f;
  int i;

  lockGlobalParams;
  if (!(list = (GList *)cMapDirs->lookup(collection))) {
    unlockGlobalParams;
    return NULL;
  }
  for (i = 0; i < list->getLength(); ++i) {
    dir = (GString *)list->get(i);
    fileName = appendToPath(dir->copy(), cMapName->getCString());
    f = fopen(fileName->getCString(), "r");
    delete fileName;
    if (f) {
      unlockGlobalParams;
      return f;
    }
  }
  unlockGlobalParams;
  return NULL;
}

FILE *GlobalParams::findToUnicodeFile(GString *name) {
  GString *dir, *fileName;
  FILE *f;
  int i;

  lockGlobalParams;
  for (i = 0; i < toUnicodeDirs->getLength(); ++i) {
    dir = (GString *)toUnicodeDirs->get(i);
    fileName = appendToPath(dir->copy(), name->getCString());
    f = fopen(fileName->getCString(), "r");
    delete fileName;
    if (f) {
      unlockGlobalParams;
      return f;
    }
  }
  unlockGlobalParams;
  return NULL;
}

DisplayFontParam *GlobalParams::getDisplayFont(GString *fontName) {
  DisplayFontParam *dfp;

  lockGlobalParams;
  dfp = (DisplayFontParam *)displayFonts->lookup(fontName);
  unlockGlobalParams;
  return dfp;
}

DisplayFontParam *GlobalParams::getDisplayCIDFont(GString *fontName,
						  GString *collection) {
  DisplayFontParam *dfp;

  lockGlobalParams;
  if (!fontName ||
      !(dfp = (DisplayFontParam *)displayNamedCIDFonts->lookup(fontName))) {
    dfp = (DisplayFontParam *)displayCIDFonts->lookup(collection);
  }
  unlockGlobalParams;
  return dfp;
}

GString *GlobalParams::getPSFile() {
  GString *s;

  lockGlobalParams;
  s = psFile ? psFile->copy() : (GString *)NULL;
  unlockGlobalParams;
  return s;
}

int GlobalParams::getPSPaperWidth() {
  int w;

  lockGlobalParams;
  w = psPaperWidth;
  unlockGlobalParams;
  return w;
}

int GlobalParams::getPSPaperHeight() {
  int h;

  lockGlobalParams;
  h = psPaperHeight;
  unlockGlobalParams;
  return h;
}

void GlobalParams::getPSImageableArea(int *llx, int *lly, int *urx, int *ury) {
  lockGlobalParams;
  *llx = psImageableLLX;
  *lly = psImageableLLY;
  *urx = psImageableURX;
  *ury = psImageableURY;
  unlockGlobalParams;
}

GBool GlobalParams::getPSCrop() {
  GBool f;

  lockGlobalParams;
  f = psCrop;
  unlockGlobalParams;
  return f;
}

GBool GlobalParams::getPSExpandSmaller() {
  GBool f;

  lockGlobalParams;
  f = psExpandSmaller;
  unlockGlobalParams;
  return f;
}

GBool GlobalParams::getPSShrinkLarger() {
  GBool f;

  lockGlobalParams;
  f = psShrinkLarger;
  unlockGlobalParams;
  return f;
}

GBool GlobalParams::getPSCenter() {
  GBool f;

  lockGlobalParams;
  f = psCenter;
  unlockGlobalParams;
  return f;
}

GBool GlobalParams::getPSDuplex() {
  GBool d;

  lockGlobalParams;
  d = psDuplex;
  unlockGlobalParams;
  return d;
}

PSLevel GlobalParams::getPSLevel() {
  PSLevel level;

  lockGlobalParams;
  level = psLevel;
  unlockGlobalParams;
  return level;
}

PSFontParam *GlobalParams::getPSFont(GString *fontName) {
  PSFontParam *p;

  lockGlobalParams;
  p = (PSFontParam *)psFonts->lookup(fontName);
  unlockGlobalParams;
  return p;
}

PSFontParam *GlobalParams::getPSFont16(GString *fontName,
				       GString *collection, int wMode) {
  PSFontParam *p;
  int i;

  lockGlobalParams;
  p = NULL;
  if (fontName) {
    for (i = 0; i < psNamedFonts16->getLength(); ++i) {
      p = (PSFontParam *)psNamedFonts16->get(i);
      if (!p->pdfFontName->cmp(fontName) &&
	  p->wMode == wMode) {
	break;
      }
      p = NULL;
    }
  }
  if (!p && collection) {
    for (i = 0; i < psFonts16->getLength(); ++i) {
      p = (PSFontParam *)psFonts16->get(i);
      if (!p->pdfFontName->cmp(collection) &&
	  p->wMode == wMode) {
	break;
      }
      p = NULL;
    }
  }
  unlockGlobalParams;
  return p;
}

GBool GlobalParams::getPSEmbedType1() {
  GBool e;

  lockGlobalParams;
  e = psEmbedType1;
  unlockGlobalParams;
  return e;
}

GBool GlobalParams::getPSEmbedTrueType() {
  GBool e;

  lockGlobalParams;
  e = psEmbedTrueType;
  unlockGlobalParams;
  return e;
}

GBool GlobalParams::getPSEmbedCIDPostScript() {
  GBool e;

  lockGlobalParams;
  e = psEmbedCIDPostScript;
  unlockGlobalParams;
  return e;
}

GBool GlobalParams::getPSEmbedCIDTrueType() {
  GBool e;

  lockGlobalParams;
  e = psEmbedCIDTrueType;
  unlockGlobalParams;
  return e;
}

GBool GlobalParams::getPSOPI() {
  GBool opi;

  lockGlobalParams;
  opi = psOPI;
  unlockGlobalParams;
  return opi;
}

GBool GlobalParams::getPSASCIIHex() {
  GBool ah;

  lockGlobalParams;
  ah = psASCIIHex;
  unlockGlobalParams;
  return ah;
}

GString *GlobalParams::getTextEncodingName() {
  GString *s;

  lockGlobalParams;
  s = textEncoding->copy();
  unlockGlobalParams;
  return s;
}

EndOfLineKind GlobalParams::getTextEOL() {
  EndOfLineKind eol;

  lockGlobalParams;
  eol = textEOL;
  unlockGlobalParams;
  return eol;
}

GBool GlobalParams::getTextPageBreaks() {
  GBool pageBreaks;

  lockGlobalParams;
  pageBreaks = textPageBreaks;
  unlockGlobalParams;
  return pageBreaks;
}

GBool GlobalParams::getTextKeepTinyChars() {
  GBool tiny;

  lockGlobalParams;
  tiny = textKeepTinyChars;
  unlockGlobalParams;
  return tiny;
}

GString *GlobalParams::findFontFile(GString *fontName, const char **exts) {
  GString *dir, *fileName;
  const char **ext;
  FILE *f;
  int i;

  lockGlobalParams;
  for (i = 0; i < fontDirs->getLength(); ++i) {
    dir = (GString *)fontDirs->get(i);
    for (ext = exts; *ext; ++ext) {
      fileName = appendToPath(dir->copy(), fontName->getCString());
      fileName->append(*ext);
      if ((f = fopen(fileName->getCString(), "rb"))) {
	fclose(f);
	unlockGlobalParams;
	return fileName;
      }
      delete fileName;
    }
  }
  unlockGlobalParams;
  return NULL;
}

GString *GlobalParams::getInitialZoom() {
  GString *s;

  lockGlobalParams;
  s = initialZoom->copy();
  unlockGlobalParams;
  return s;
}

GBool GlobalParams::getEnableT1lib() {
  GBool f;

  lockGlobalParams;
  f = enableT1lib;
  unlockGlobalParams;
  return f;
}

GBool GlobalParams::getEnableFreeType() {
  GBool f;

  lockGlobalParams;
  f = enableFreeType;
  unlockGlobalParams;
  return f;
}


GBool GlobalParams::getAntialias() {
  GBool f;

  lockGlobalParams;
  f = antialias;
  unlockGlobalParams;
  return f;
}

GBool GlobalParams::getMapNumericCharNames() {
  GBool map;

  lockGlobalParams;
  map = mapNumericCharNames;
  unlockGlobalParams;
  return map;
}

GBool GlobalParams::getPrintCommands() {
  GBool p;

  lockGlobalParams;
  p = printCommands;
  unlockGlobalParams;
  return p;
}

GBool GlobalParams::getErrQuiet() {
  GBool q;

  lockGlobalParams;
  q = errQuiet;
  unlockGlobalParams;
  return q;
}

CharCodeToUnicode *GlobalParams::getCIDToUnicode(GString *collection) {
  GString *fileName;
  CharCodeToUnicode *ctu;

  lockGlobalParams;
  if (!(ctu = cidToUnicodeCache->getCharCodeToUnicode(collection))) {
    if ((fileName = (GString *)cidToUnicodes->lookup(collection)) &&
	(ctu = CharCodeToUnicode::parseCIDToUnicode(fileName, collection))) {
      cidToUnicodeCache->add(ctu);
    }
  }
  unlockGlobalParams;
  return ctu;
}

CharCodeToUnicode *GlobalParams::getUnicodeToUnicode(GString *fontName) {
  CharCodeToUnicode *ctu;
  GHashIter *iter;
  GString *fontPattern, *fileName;

  lockGlobalParams;
  fileName = NULL;
  unicodeToUnicodes->startIter(&iter);
  while (unicodeToUnicodes->getNext(&iter, &fontPattern, (void **)&fileName)) {
    if (strstr(fontName->getCString(), fontPattern->getCString())) {
      unicodeToUnicodes->killIter(&iter);
      break;
    }
    fileName = NULL;
  }
  if (fileName) {
    if (!(ctu = unicodeToUnicodeCache->getCharCodeToUnicode(fileName))) {
      if ((ctu = CharCodeToUnicode::parseUnicodeToUnicode(fileName))) {
	unicodeToUnicodeCache->add(ctu);
      }
    }
  } else {
    ctu = NULL;
  }
  unlockGlobalParams;
  return ctu;
}

UnicodeMap *GlobalParams::getUnicodeMap(GString *encodingName) {
  return getUnicodeMap2(encodingName);
}

UnicodeMap *GlobalParams::getUnicodeMap2(GString *encodingName) {
  UnicodeMap *map;

  if (!(map = getResidentUnicodeMap(encodingName))) {
    lockUnicodeMapCache;
    map = unicodeMapCache->getUnicodeMap(encodingName);
    unlockUnicodeMapCache;
  }
  return map;
}

CMap *GlobalParams::getCMap(GString *collection, GString *cMapName) {
  CMap *cMap;

  lockCMapCache;
  cMap = cMapCache->getCMap(collection, cMapName);
  unlockCMapCache;
  return cMap;
}

UnicodeMap *GlobalParams::getTextEncoding() {
  return getUnicodeMap2(textEncoding);
}

//------------------------------------------------------------------------
// functions to set parameters
//------------------------------------------------------------------------

void GlobalParams::addDisplayFont(DisplayFontParam *param) {
  DisplayFontParam *old;

  lockGlobalParams;
  if ((old = (DisplayFontParam *)displayFonts->remove(param->name))) {
    delete old;
  }
  displayFonts->add(param->name, param);
  unlockGlobalParams;
}

void GlobalParams::setPSFile(char *file) {
  lockGlobalParams;
  if (psFile) {
    delete psFile;
  }
  psFile = new GString(file);
  unlockGlobalParams;
}

GBool GlobalParams::setPSPaperSize(char *size) {
  lockGlobalParams;
  if (!strcmp(size, "match")) {
    psPaperWidth = psPaperHeight = -1;
  } else if (!strcmp(size, "letter")) {
    psPaperWidth = 612;
    psPaperHeight = 792;
  } else if (!strcmp(size, "legal")) {
    psPaperWidth = 612;
    psPaperHeight = 1008;
  } else if (!strcmp(size, "A4")) {
    psPaperWidth = 595;
    psPaperHeight = 842;
  } else if (!strcmp(size, "A3")) {
    psPaperWidth = 842;
    psPaperHeight = 1190;
  } else {
    unlockGlobalParams;
    return gFalse;
  }
  psImageableLLX = psImageableLLY = 0;
  psImageableURX = psPaperWidth;
  psImageableURY = psPaperHeight;
  unlockGlobalParams;
  return gTrue;
}

void GlobalParams::setPSPaperWidth(int width) {
  lockGlobalParams;
  psPaperWidth = width;
  psImageableLLX = 0;
  psImageableURX = psPaperWidth;
  unlockGlobalParams;
}

void GlobalParams::setPSPaperHeight(int height) {
  lockGlobalParams;
  psPaperHeight = height;
  psImageableLLY = 0;
  psImageableURY = psPaperHeight;
  unlockGlobalParams;
}

void GlobalParams::setPSImageableArea(int llx, int lly, int urx, int ury) {
  lockGlobalParams;
  psImageableLLX = llx;
  psImageableLLY = lly;
  psImageableURX = urx;
  psImageableURY = ury;
  unlockGlobalParams;
}

void GlobalParams::setPSCrop(GBool crop) {
  lockGlobalParams;
  psCrop = crop;
  unlockGlobalParams;
}

void GlobalParams::setPSExpandSmaller(GBool expand) {
  lockGlobalParams;
  psExpandSmaller = expand;
  unlockGlobalParams;
}

void GlobalParams::setPSShrinkLarger(GBool shrink) {
  lockGlobalParams;
  psShrinkLarger = shrink;
  unlockGlobalParams;
}

void GlobalParams::setPSCenter(GBool center) {
  lockGlobalParams;
  psCenter = center;
  unlockGlobalParams;
}

void GlobalParams::setPSDuplex(GBool duplex) {
  lockGlobalParams;
  psDuplex = duplex;
  unlockGlobalParams;
}

void GlobalParams::setPSLevel(PSLevel level) {
  lockGlobalParams;
  psLevel = level;
  unlockGlobalParams;
}

void GlobalParams::setPSEmbedType1(GBool embed) {
  lockGlobalParams;
  psEmbedType1 = embed;
  unlockGlobalParams;
}

void GlobalParams::setPSEmbedTrueType(GBool embed) {
  lockGlobalParams;
  psEmbedTrueType = embed;
  unlockGlobalParams;
}

void GlobalParams::setPSEmbedCIDPostScript(GBool embed) {
  lockGlobalParams;
  psEmbedCIDPostScript = embed;
  unlockGlobalParams;
}

void GlobalParams::setPSEmbedCIDTrueType(GBool embed) {
  lockGlobalParams;
  psEmbedCIDTrueType = embed;
  unlockGlobalParams;
}

void GlobalParams::setPSOPI(GBool opi) {
  lockGlobalParams;
  psOPI = opi;
  unlockGlobalParams;
}

void GlobalParams::setPSASCIIHex(GBool hex) {
  lockGlobalParams;
  psASCIIHex = hex;
  unlockGlobalParams;
}

void GlobalParams::setTextEncoding(char *encodingName) {
  lockGlobalParams;
  delete textEncoding;
  textEncoding = new GString(encodingName);
  unlockGlobalParams;
}

GBool GlobalParams::setTextEOL(char *s) {
  lockGlobalParams;
  if (!strcmp(s, "unix")) {
    textEOL = eolUnix;
  } else if (!strcmp(s, "dos")) {
    textEOL = eolDOS;
  } else if (!strcmp(s, "mac")) {
    textEOL = eolMac;
  } else {
    unlockGlobalParams;
    return gFalse;
  }
  unlockGlobalParams;
  return gTrue;
}

void GlobalParams::setTextPageBreaks(GBool pageBreaks) {
  lockGlobalParams;
  textPageBreaks = pageBreaks;
  unlockGlobalParams;
}

void GlobalParams::setTextKeepTinyChars(GBool keep) {
  lockGlobalParams;
  textKeepTinyChars = keep;
  unlockGlobalParams;
}

void GlobalParams::setInitialZoom(char *s) {
  lockGlobalParams;
  delete initialZoom;
  initialZoom = new GString(s);
  unlockGlobalParams;
}

GBool GlobalParams::setEnableT1lib(char *s) {
  GBool ok;

  lockGlobalParams;
  ok = parseYesNo2(s, &enableT1lib);
  unlockGlobalParams;
  return ok;
}

GBool GlobalParams::setEnableFreeType(char *s) {
  GBool ok;

  lockGlobalParams;
  ok = parseYesNo2(s, &enableFreeType);
  unlockGlobalParams;
  return ok;
}


GBool GlobalParams::setAntialias(char *s) {
  GBool ok;

  lockGlobalParams;
  ok = parseYesNo2(s, &antialias);
  unlockGlobalParams;
  return ok;
}

void GlobalParams::setMapNumericCharNames(GBool map) {
  lockGlobalParams;
  mapNumericCharNames = map;
  unlockGlobalParams;
}

void GlobalParams::setPrintCommands(GBool printCommandsA) {
  lockGlobalParams;
  printCommands = printCommandsA;
  unlockGlobalParams;
}

void GlobalParams::setErrQuiet(GBool errQuietA) {
  lockGlobalParams;
  errQuiet = errQuietA;
  unlockGlobalParams;
}

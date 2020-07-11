
//# line 2 "LengDic.y"
////////////////////////////////////////////////////////////////////////////
//   Funciones LEX y YACC para lenguaje de diccionario                    //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         7/10/93                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//  Se implementa Funciones LEX y YACC para lenguaje de diccionario       // 
//                                                                        //
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <ctype.h>
#include "UtilFun.h"
#include "COra.h"
#include "Lengdic.h"
#include "TranslationData.h"

inline int my_isascii(int _Ch) {	/* check ascii */
	return ((unsigned)(_Ch) <= 0x7f); }

//#define YYDEBUG

#ifdef YYDEBUG
	#define YYTRACE(...) TRACE(__VA_ARGS__)
#else
	#define YYTRACE(...)	{}
#endif

////////////////////////////////////////////////////////////////////////////
static const char *tok[] = {
#include "LengDic.tok"
    NULL
  };

////////////////////////////////////////////////////////////////////////////

#define YYREDMAX  3000
#define YYMAXDEPTH  500
#define DIM(a)  ((int)(sizeof(a)/sizeof(a[0])))
#define YYERRCODE   256

////////////////////////////////////////////////////////////////////////////
// Variables externas utilizadas                                          //
////////////////////////////////////////////////////////////////////////////

enum ldLEX {
           ldBLANCO,
           ldSTRING,
           ldCLAVE,
           ldTRAD,
           ldNUMERO,
           ldCHAR,
           ldCOMENTARIO,
           ldDATA,
           ldESP,               // @[A-Z][A-Z]
           ldSEMANTICA          // [casa;pipa;queso]
           };                            
           
////////////////////////////////////////////////////////////////////////////
//static YYSTYPE yyv[YYMAXDEPTH];    
////////////////////////////////////////////////////////////////////////////

#define CHARSTRING 257
#define SEMANTICA 258
#define PALABRA 259
#define NUMERO 260
#define TIPOWORD 261
#define SET_ARTICULO 262
#define SET_MODO 263
#define ISNEXT_SETNEXT 264
#define EnCD 265
#define EnCI 266
#define EnCCT 267
#define EnCCL 268
#define EnCCM 269
#define EnSujeto 270
#define EnVerbo 271
#define LW1 272
#define LW2 273
#define LW3 274
#define NW1 275
#define NW2 276
#define NW3 277
#define SETL1 278
#define SETL2 279
#define SETN1 280
#define SETN2 281
#define SETVERBO 282
#define SETNEXTPREP 283
#define CADENA 284
#define FWORD 285
#define LWORD 286
#define SETLASTWORD 287
#define SETFIRSTWORD 288
#define LPARTE1 289
#define NPARTE1 290
#define SUJETO 291
#define VERBO 292
#define CCL 293
#define CCT 294
#define CD 295
#define CI 296
#define WV 297
#define CCM 298
#define TPRESENTE 299
#define TPASADO 300
#define TFUTURO 301
#define _DELETE_ 302
#define SETFW 303
#define IsLastPPNotArt 304
#define SET_REFLEXIVO 305
#define IsLastNotDDSet 306
#define EXIST_LEFT 307
#define EXIST_RIGHT 308
#define SET_PER 309
#define IS_FIRST_UPPER 310
#define IS_LOWER 311
#define IS_UPPER 312
#define SET_LAST_PREP_NO_ART 313
#define SET_TIEMPO 314
#define SET_KEY_SUF 315
#define SET_KEY_PREF 316
#define SET_PLURAL 317
#define SET_SINGULAR 318
#define SET_FEMENINO 319
#define DATA 320
#define SET_PREFIJO 321
#define SET_TRAD_SUF 322
#define SET_TRAD_PREF 323
#define SET_TIPO2 324
#define SET_SUFIJO 325
#define TIPO2 326
#define IS_ABREVIA 327
#define SET_CASO 328
#define IS_ALL 329
#define IS_TIPO 330
#define IS_WORD 331
#define IS_NOT_TIPO 332
#define IS_NOT_WORD 333
#define SET_TIPO 334
#define ADVERBIO 335
#define ARTICULO 336
#define SUSTANTIVO 337
#define ADJETIVO 338
#define PRONOMBRE 339
#define GERUNDIO 340
#define IS_IN_MODE 341
#define IS_IN_TIME 342
#define PARTICIPIO 343
#define PPARTICIPIO 344
#define PASADO 345
#define OFNOTART 346
#define OFAS 347
#define FIRSTNOTART 348
#define PREPOSICION 349
#define LNDD 350
#define NNDD 351
#define LNTYPE 352
#define NNTYPE 353
#define NEXTX 354
#define LASTX 355
#define SET_WORD 356
#define ESP 357
#define INSERT 358
#define SET_PATRON 359
#define IS_KEY_SUFFIX 360
#define EXIST_LEFT_T 361
#define EXIST_RIGHT_T 362
#define SET_ESDRUJULA 363
#define IS_CADENA_DATO 364
#define m_MODO 365
#define m_INFINITIVO 366
#define m_INDICATIVO 367
#define m_GERUNDIO 368
#define m_PARTICIPIO 369
#define m_IMPERATIVO 370
#define m_SUBJ_IMP 371
#define m_SUBJ_IMP_IND 372
#define m_IMP_PART_IND 373
#define m_SUBJ_IMP_IND_PART 374
#define m_PART_IND 375
#define m_IMP_IND 376
#define m_IMP_INF 377
#define m_SUBJ_IND 378
#define m_POT_IND 379
#define m_SUBJ_PART 380
#define m_POTENCIAL 381
#define m_SUBJUNTIVO 382
#define ADICIONA_NODO 383
#define IS_SUFIX 384
#define ENTRE_COMILLAS 385
#define INSERT_WORD_AFTER 386
#define m_PRIMERA_PER 387
#define m_SEGUNDA_PER 388
#define m_TERCERA_PER 389
#define m_SINGULAR 390
#define m_PLURAL 391
#define DEL_ACCENT 392
#define SET_NEGADO 393
#define INFORMAL 394
//YYSTYPE yylval, yyval;
#define YYERRCODE 256

//# line 1528 "LengDic.y"

/***************************************************************************/
static PalClave Claves[] =
         {
         {"ADD",ADICIONA_NODO},
         {"ADJETIVO",ADJETIVO},
         {"ADVERBIO",ADVERBIO},
         {"ARTICULO",ARTICULO},
//         {"CCL",CCL},
//         {"CCT",CCT},
//         {"CCM",CCM},
//         {"CD",CD},                                                    
//         {"CI",CI},
         {"DEL",_DELETE_},               
         {"DELACCENT",DEL_ACCENT},
         {"ECOMILLA",ENTRE_COMILLAS},
         {"ELEFT",EXIST_LEFT},               
         {"ELEFTT",EXIST_LEFT_T},               
         {"ERIGHT",EXIST_RIGHT},               
         {"ERIGHTT",EXIST_RIGHT_T},               
         {"FIRSTNOTART",FIRSTNOTART},
         {"FTR",TFUTURO},
         {"FUPPER",IS_FIRST_UPPER},
         {"FW",FWORD},
         {"GERUNDIO",GERUNDIO},
//         {"ICD",EnCD},
//         {"ICI",EnCI},
//         {"ICL",EnCCL},
//         {"ICM",EnCCM},
//         {"ICT",EnCCT},
		 {"INFORMAL",INFORMAL},
         {"INS",INSERT},
         {"INSWAFTER",INSERT_WORD_AFTER},
         {"ISABR",IS_ABREVIA},
         {"ISALL",IS_ALL},
         {"ISINMODE",IS_IN_MODE}, 
         {"ISINTIME",IS_IN_TIME}, 
         {"ISKEYSUFIX",IS_KEY_SUFFIX}, 
         {"ISLNDDSET",IsLastNotDDSet},
         {"ISLPNOTART",IsLastPPNotArt},
         {"ISNT",IS_NOT_TIPO}, 
         {"ISNW",IS_NOT_WORD}, 
         {"ISSUFIX",IS_SUFIX}, 
         {"IST",IS_TIPO}, 
//         {"ISTRDSUF",IS_SUFIJO_TRAD}, 
//         {"ISU",EnSujeto}, 
         {"ISW",IS_WORD}, 
//         {"IVE",EnVerbo},
         {"L1",LW1},
         {"L2",LW2},
         {"L3",LW3},
         {"LAST",IsLastNotDDSet},
         {"LASTX",LASTX},  
         {"LNDD",LNDD},
         {"LNTYPE",LNTYPE},
         {"LOWER",IS_LOWER},
//         {"LP1",LPARTE1},
         {"LW",LWORD},
         {"MGERUNDIO",m_GERUNDIO},
         {"MIMPERATIVO",m_IMPERATIVO},
         {"MIMPIND",m_IMP_IND},
         {"MIMPINF",m_IMP_INF},
         {"MIMPPARTIND",m_IMP_PART_IND},
         {"MINDICATIVO",m_INDICATIVO},
         {"MINFINITIVO",m_INFINITIVO},
         {"MMODO",m_MODO},
         {"MPARTICIPIO",m_PARTICIPIO},
         {"MPARTIND",m_PART_IND},
         {"MPOTENCIAL",m_POTENCIAL},
         {"MPOTIND",m_POT_IND},
         {"MSUBJIMP",m_SUBJ_IMP},
         {"MSUBJIMPIND",m_SUBJ_IMP_IND},
         {"MSUBJIMPINDPART",m_SUBJ_IMP_IND_PART},
         {"MSUBJIND",m_SUBJ_IND},
         {"MSUBJPART",m_SUBJ_PART},
         {"MSUBJUNTIVO",m_SUBJUNTIVO},
         {"N1",NW1},           
         {"N2",NW2},  
         {"N3",NW3},  
         {"NEXT",ISNEXT_SETNEXT},
         {"NEXTX",NEXTX},  
         {"NNDD",NNDD},  
         {"NNTYPE",NNTYPE},
//         {"NP1",NPARTE1},
         {"OFAS",OFAS},
         {"OFNOTART",OFNOTART},
         {"PARTICIPIO",PARTICIPIO},
         {"PASADO",PASADO},
         {"PLURAL",m_PLURAL},
         {"PPARTICIPIO",PPARTICIPIO},
         {"PREPOSICION",PREPOSICION},
         {"PRIMERAP",m_PRIMERA_PER},
         {"PRONOMBRE",PRONOMBRE},
//         {"PSD",TPASADO},
//         {"PST",TPRESENTE},
//         {"S",SUJETO},         
         {"SEGUNDAP",m_SEGUNDA_PER},
         {"SETART",SET_ARTICULO},          
         {"SETCASO",SET_CASO},          
         {"SETESDRUJULA",SET_ESDRUJULA},          
         {"SETFEMENINO",SET_FEMENINO},          
         {"SETFW",SETFW},          
         {"SETKEYPREF",SET_KEY_PREF},          
         {"SETKEYSUF",SET_KEY_SUF},          
         {"SETMODO",SET_MODO},          
         {"SETNEGADO",SET_NEGADO},
         {"SETPART",SET_LAST_PREP_NO_ART},          
         {"SETPATRON",SET_PATRON},          
         {"SETPERSONA",SET_PER},          
         {"SETPLURAL",SET_PLURAL},          
         {"SETPREFIJO",SET_PREFIJO},          
         {"SETREF",SET_REFLEXIVO},          
         {"SETSINGULAR",SET_SINGULAR},
         {"SETSUFIJO",SET_SUFIJO},          
         {"SETTIPO",SET_TIPO},          
         {"SETTIPO2",SET_TIPO2},          
         {"SETTRADPREF",SET_TRAD_PREF},          
         {"SETTRADSUF",SET_TRAD_SUF},          
         {"SETVERBO",SETVERBO},
         {"SETW",SET_WORD},          
         {"SINGULAR",m_SINGULAR},
         {"SL1",SETL1},
         {"SL2",SETL2},
         {"SN1",SETN1},
         {"SN2",SETN2},
         {"SNP",SETNEXTPREP},
         {"SUSTANTIVO",SUSTANTIVO},
         {"TERCERAP",m_TERCERA_PER},
         {"TIME",SET_TIEMPO},
         {"TIPO2",TIPO2},
         {"UPPER",IS_UPPER},
//         {"V",VERBO},
         {"VERBO",VERBO},
         {"W",PALABRA},
//         {"WV",WV}
         };

const int SizeofClaves = sizeof(Claves)/sizeof(PalClave);
//extern PalClave FAR Tipo[];
//extern int SizeofTipo;
/***************************************************************************/
void CLengDic::Execute(POSITION aNodo, CStringA &aDato, BOOL aSyntaxChk, BOOL aLlenaAcep)  
  {       // Llamada a lenguaje de diccionario    
  ASSERT(aNodo != NULL);
    
  m_SyntaxChk = aSyntaxChk;             // Pone modo ejecución
    
  if(aDato.IsEmpty() == TRUE)
    {
    //TRACE("cadena vacía a lenguaje de diccionario\n");
    return;
    }

	m_LlenaAcep = aLlenaAcep;    
  ASSERT(GetPtrSentence() != NULL);
  _PtrNodo = aNodo;
  SetPtrWord((LPCWORD)GetPtrSentence()->m_Ora.GetAt(aNodo));

  m_Dato = aDato;
  _PtrIni = m_Dato.c_str();     //GetPtrWord()->m_Data;
  ASSERT(_PtrIni != NULL);
  token = -1;   // input token 
  errct = 0;    // error count 
  errfl = 0;    // error flag 
  //ASSERT( RE_PatDic != NULL);
                           
  _PtrIni    = m_Dato.c_str();
  ASSERT(_PtrIni != NULL);
  token = -1; // input token 
  errct = 0;  // error count 
  errfl = 0;  // error flag 
  yyparse();
  YYTRACE("Fin Primer pase");
     
  if(directo == FALSE && m_Error == FALSE && m_SyntaxChk == FALSE)
    {
    YYTRACE("Necesita Segundo pase");
    // Segunda vuelta para ejecutar las funciones en la condicion TRUE 
                                                                                
    m_Dato = m_Trad;
            
    _PtrIni = m_Dato.c_str();
    ASSERT(_PtrIni != NULL);
    token = -1; // input token 
    errct = 0;  // error count 
    errfl = 0;  // error flag 
    yyparse();
    }                                    
         
  if(m_SyntaxChk == FALSE)     
    aDato = m_Trad;                       
  //YYTRACE("Fin Segundo pase");
  }

int CLengDic::yylex()
	{
	if (*_PtrIni == 0)
		return -1;

	int ret = -1;            // token a retornar 
	int   l = 0;             // longitud de la string que machea con la RE 
	int mat = -1;            // RE que machea 

	ASSERT( _PtrIni != NULL);

	BOOL otravez = FALSE;
  
	do {
		otravez = FALSE;
		while (*_PtrIni != '\0' && strchr(" \t\n\r",*_PtrIni)) 
			_PtrIni++;
            
		m_ColNo = (int)(_PtrIni - m_Dato.c_str());

		if(*_PtrIni == '{')
			{
			mat = ldSTRING;
			for(l=0; *(_PtrIni+l) != '\0' && *(_PtrIni+l) != '}';l++)
				;
			if(*(_PtrIni+l) == '}')
				l++;    
			}
    else if(*_PtrIni == '\"')
      {
      mat = ldTRAD;
      for(l=1; *(_PtrIni+l) != '\0' && *(_PtrIni+l) != '\"';l++)
        ;
      if(*(_PtrIni+l) == '\"')
        l++;    
      }
    else if(*_PtrIni == '\'')
			{
			mat = ldDATA;
			for(l=1; *(_PtrIni+l) != '\0' && *(_PtrIni+l) != '\'';l++)
					;
			if(*(_PtrIni+l) == '\'')
					l++;    
			}
		else if(*_PtrIni == '¶')
			{
			mat = ldCOMENTARIO;
			for(l=1; *(_PtrIni+l) != '\0' && *(_PtrIni+l) != '¶';l++)
					;
			if(*(_PtrIni+l) == '¶')
					l++;    
			}
		else if(isalpha(*_PtrIni))
			{
			YYTRACE("alfa");
			mat = ldCLAVE;
			for(l=0; *(_PtrIni+l) != '\0' && isalnum(*(_PtrIni+l));l++)
					;
			}
		else if(isdigit(*_PtrIni))
			{
			YYTRACE("digit");
			mat = ldNUMERO;
			for(l=0; *(_PtrIni+l) != '\0' && isdigit(*(_PtrIni+l));l++)
					;
			}
		else if(*_PtrIni == '@' 
			&& *(_PtrIni+1) != '\0'
			&& *(_PtrIni+2)!= '\0' 
			&& isupper(*(_PtrIni+1))
			&& isupper(*(_PtrIni+2))
			&& isalpha(*(_PtrIni+1))
			&& isalpha(*(_PtrIni+2)))
			{
			YYTRACE("esp");
			mat = ldESP;
			l = 3;
			}
		else if(*_PtrIni == '@' 
			&& *(_PtrIni+1) == '[')
			{
			for(l=2; *(_PtrIni+l) != '\0' && *(_PtrIni+l) != ']';l++)
					;
			if(*(_PtrIni+l) == ']')
					l++;    
			mat = ldSEMANTICA;
			}
		else if(strchr("-$()?|:&!,~@*+=",*_PtrIni))
			{       
			YYTRACE("CHAR");
			mat = ldCHAR;
			l = 1;
			}            
		else
			{
			l = 1;
			_PtrIni++;
			continue;
			}    

    switch(mat)
      {
			case ldSTRING:         
						{
						CStringA tmp(_PtrIni+1,l-2);
						yylval.string = tmp;
						ret = CHARSTRING;
						yyerrtok = tmp;
						break;
						}
			case ldDATA:         
						{
						CStringA tmp(_PtrIni+1,l-2);
						yylval.string = tmp;
						ret = DATA;
						yyerrtok = tmp;
						break;
						}
			case ldCLAVE:
						{
						CStringA tmp(_PtrIni,l);
						yylval.string = tmp;
						ret = FindHotWord(yylval.string,Claves,SizeofClaves);
						if(ret == -1)
								{            
								//if(m_SyntaxChk == TRUE)
										{
										CStringA stmp;
										BOOL encontre = GetPtrSourceLanguageData()->GetPtrTypeAndPatternDic()->IsKey(tmp);
										if(encontre == TRUE)
											 ret = TIPOWORD;
										}
								//else
								//   ret = TIPOWORD;
								}
						yyerrtok = tmp;
						break;
						}
			case ldNUMERO:
						{
						CStringA buffer(_PtrIni, l);
						yylval.integer = atoics(buffer);
						ret = NUMERO;
						yyerrtok = buffer;
						break;
						}
			case ldTRAD:
						{
						CStringA tmp(_PtrIni+1,l-2);
						yylval.string = tmp;
						ret = CADENA;
						yyerrtok = tmp;
						break;
						}
			case ldCHAR:
						{
						ret = *_PtrIni;
						yyerrtok = *_PtrIni;
						break;
						}
			case ldESP:
						{
						CStringA tmp(_PtrIni+1,l-1);
						yylval.string = tmp;
						ret = ESP;
						yyerrtok = tmp;
						break;
						}
			case ldSEMANTICA:
						{
						CStringA tmp(_PtrIni+2,l-3);
						yylval.string = tmp;
						ret = SEMANTICA;
						yyerrtok = tmp;
						break;
						}
			case ldCOMENTARIO:
						{
						otravez = TRUE;
						break;
						}
			}

    _PtrIni += l;
    } while(otravez == TRUE && *_PtrIni);

  return ret;
  }      
/****************************************************************************/
/* yyerror - improved error reporting */

void CLengDic::yyerror(const char *s, const char *t)
    { 
    if(m_Error == TRUE)
          return;
    m_Error = TRUE; 
    char buff[255];
    if (s != NULL && *s) 
        sprintf_(buff, "Palabra:[%s] - En la columna %d cerca de \"%s\": %s actual: [%s]",CS2SZ(GetPtrWord()->m_Orig),m_ColNo,(const char *)CS2SZ(yyerrtok),s,t);
    else if (t != NULL && *t) 
        sprintf_(buff, "Esperando: %s",t);
        
#if !defined(MERCADO)
    AfxMessageBox(buff,MB_OK|MB_ICONSTOP);
#endif
    }

/****************************************************************************/
/* yydisplay - returns pointer to printable form for token type in 'ch' */

const char *CLengDic::yydisplay(int ch)
    { 
    static char buf[16];

    switch (ch) 
        {
    case         0: return ("[fin de cadena]");
    case YYERRCODE: return ("[error]");
    case      '\a': return ("'\\a'");
    case      '\b': return ("'\\b'");
    case      '\f': return ("'\\f'");
    case      '\n': return ("'\\n'");
    case      '\r': return ("'\\r'");
    case      '\t': return ("'\\t'");
    case      '\v': return ("'\\v'");
        }
    if (ch > YYERRCODE && ch < YYERRCODE + DIM(tok))
        return (tok[ch-(YYERRCODE + 1)]);       /* is %token */
    if (my_isascii(ch) && isprint(ch))
        sprintf_(buf,"'%c'", ch);           /* printable */
    else
        sprintf_(buf,"char %d", ch);        /* unprintable */
    return(buf);
    }
LPCWORD CLengDic::GetPtrWord()
	{return m_ptrWord;}
void CLengDic::SetPtrWord(LPCWORD ptrWord)
	{
	ASSERT(ptrWord != NULL);
	m_ptrWord = ptrWord;
	}
/****************************************************************************/

#include <stdio.h>
FILE *yytfilep;
char *yytfilen;
int yytflag = 0;
static const int yyexca[] = {
  -1, 1,
  0, -1,
  -2, 0,
  0,
};

#define YYNPROD 187
#define YYLAST 1084

static const int yyact[] = {
      17,     304,     303,      27,     120,     219,     202,       5,
       9,      35,      36,     417,     414,     331,     413,     412,
      22,      23,      24,      19,      20,      21,     411,     410,
     409,     408,     401,     398,     252,      25,      26,     207,
     397,     396,     225,     395,     209,     329,     323,     321,
     318,      28,     314,      44,      45,      46,     228,     229,
     233,     234,     230,     235,     236,     237,     238,     239,
     240,     241,     242,     243,     244,     231,     232,      42,
      43,      41,      39,      37,      38,     219,      40,     313,
       2,     312,     311,     310,     295,     293,     114,     208,
     210,     211,     212,     213,     214,     285,     284,     215,
     216,     217,     283,     282,     393,     218,      29,      30,
      31,      32,      33,      34,     209,     272,     271,     270,
     269,     219,     268,     194,     193,     406,     228,     229,
     233,     234,     230,     235,     236,     237,     238,     239,
     240,     241,     242,     243,     244,     231,     232,     219,
     391,     305,      17,     245,     246,     247,     248,     249,
     209,       5,     407,     405,     404,     403,     402,     208,
     210,     211,     212,     213,     214,     400,     320,     215,
     216,     217,     325,     297,     294,     218,     209,     220,
     221,     250,     267,     266,     254,     330,     123,     125,
     443,     124,     264,     265,      12,     390,     227,     123,
      18,     226,     124,     208,     210,     211,     212,     213,
     214,     121,     388,     215,     216,     217,     199,     382,
     381,     218,     380,     379,     378,     377,     326,     327,
     376,     208,     210,     211,     212,     213,     214,      13,
     374,     215,     216,     217,      14,     372,     366,     218,
     365,     358,     357,     319,     355,     350,     121,       4,
       7,     111,      27,     349,     348,      83,      89,     103,
      47,      48,      50,      49,      51,      52,      53,      22,
      23,      24,      19,      20,      21,      73,      74,      75,
      76,      84,     122,       8,      25,      26,     347,     346,
      35,      36,      42,      43,      41,      39,      37,      38,
      28,      40,      44,      45,      46,      79,      82,     105,
      85,     104,      54,      55,     109,      59,      61,      60,
      87,      94,      97,      96,     101,     100,     102,     122,
      90,      99,      98,      92,      91,      10,      64,      86,
      63,      65,      67,      66,      68,      93,     206,      17,
     256,     259,     260,     255,      71,      72,     137,     345,
     344,     106,     107,     108,     343,      29,      30,      31,
      32,      33,      34,      77,     110,      80,      95,      70,
      56,      57,      78,      58,     442,     386,     387,     119,
     441,     440,     257,     258,     439,     224,     253,     438,
     437,     436,     134,     132,     130,     128,      81,      69,
      62,     113,       7,     126,      27,     435,     434,     112,
      88,      15,      47,      48,      50,      49,      51,      52,
      53,      22,      23,      24,      19,      20,      21,     392,
     394,     119,     433,     432,     198,     116,      25,      26,
     399,     431,      35,      36,      42,      43,      41,      39,
      37,      38,      28,      40,      44,      45,      46,     418,
     415,     416,     117,     389,      54,      55,     430,      59,
      61,      60,     219,     135,     133,     131,     129,     119,
     429,     428,      27,     308,     127,     427,     119,      10,
      64,     198,      63,      65,      67,      66,      68,      22,
      23,      24,      19,      20,      21,     426,      71,      72,
     117,     209,     425,     424,      25,      26,     423,      29,
      30,      31,      32,      33,      34,     422,       6,     421,
      28,      70,      56,      57,     115,      58,     420,     118,
     419,     385,     384,     383,     375,     373,     371,     370,
     136,     369,     368,     367,     364,     363,     362,     361,
     360,      69,      62,     359,     208,     210,     211,     212,
     213,     214,     356,      15,     215,     216,     217,     354,
     353,     352,     218,     351,     342,     118,     341,     340,
     339,     338,     337,     336,     118,      29,      30,      31,
      32,      33,      34,     335,     334,     333,     328,     324,
     322,     317,     316,     315,     298,     296,     287,     197,
     196,      27,     195,     192,     191,     190,     189,      47,
      48,      50,      49,      51,      52,      53,      22,      23,
      24,      19,      20,      21,     332,     188,     187,     186,
     185,     184,     116,      25,      26,     183,     182,      35,
      36,      42,      43,      41,      39,      37,      38,      28,
      40,      44,      45,      46,     181,     200,     201,     180,
     179,      54,      55,     178,      59,      61,      60,     177,
     176,     175,     174,     173,     172,     171,     170,      27,
     263,     306,     169,     168,      10,      64,     167,      63,
      65,      67,      66,      68,      22,      23,      24,      19,
      20,      21,     166,      71,      72,     165,     164,     163,
     162,      25,      26,     161,      29,      30,      31,      32,
      33,      34,     160,     159,     158,      28,      70,      56,
      57,      27,      58,     219,     157,     156,     155,     154,
     153,     152,     151,     150,     149,     148,      22,      23,
      24,      19,      20,      21,     147,     146,      69,      62,
     145,     144,     223,      25,      26,     143,     142,     141,
      15,     140,     209,     139,     138,     203,      16,      28,
       3,      44,      45,      46,       1,       0,       0,       0,
       0,       0,      29,      30,      31,      32,      33,      34,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,     288,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,     208,     210,     211,
     212,     213,     214,       0,       0,     215,     216,     217,
       0,       0,       0,     218,      29,      30,      31,      32,
      33,      34,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,     228,     229,     233,     234,
     230,     235,     236,     237,     238,     239,     240,     241,
     242,     243,     244,     231,     232,       0,       0,       0,
       0,     245,     246,     247,     248,     249,     111,       0,
       0,       0,      83,      89,     103,       0,       0,       0,
       0,       0,       0,       0,       0,      27,     302,       0,
       0,       0,      73,      74,      75,      76,      84,       0,
       0,       0,      22,      23,      24,      19,      20,      21,
       0,       0,       0,       0,       0,       0,       0,      25,
      26,       0,      79,      82,     105,      85,     104,       0,
       0,     109,       0,      28,       0,      87,      94,      97,
      96,     101,     100,     102,       0,      90,      99,      98,
      92,      91,       0,       0,      86,       0,       0,       0,
       0,       0,      93,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,     106,     107,
     108,       0,       0,       0,       0,       0,       0,       0,
      77,     110,      80,      95,       0,      27,     299,      78,
      29,      30,      31,      32,      33,      34,       0,       0,
       0,       0,      22,      23,      24,      19,      20,      21,
       0,       0,       0,      81,       0,       0,     113,      25,
      26,      27,     292,       0,     112,      88,       0,       0,
       0,      11,       0,      28,       0,       0,      22,      23,
      24,      19,      20,      21,      27,       0,       0,       0,
       0,       0,       0,      25,      26,       0,       0,       0,
       0,      22,      23,      24,      19,      20,      21,      28,
       0,       0,       0,       0,       0,       0,      25,      26,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,      28,       0,      27,       0,       0,       0,
      29,      30,      31,      32,      33,      34,       0,       0,
       0,      22,      23,      24,      19,      20,      21,       0,
       0,       0,       0,       0,       0,       0,      25,      26,
       0,       0,       0,       0,      29,      30,      31,      32,
      33,      34,      28,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,      29,
      30,      31,      32,      33,      34,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,     204,     205,       0,       0,       0,
       0,       0,     222,     251,       0,       0,       0,       0,
     261,     262,       0,       0,       0,       0,       0,      29,
      30,      31,      32,      33,      34,     273,     274,     275,
     276,     277,     278,     279,     280,     281,       0,       0,
       0,       0,     286,       0,     289,     290,     291,       0,
       0,       0,       0,       0,       0,     300,     301,       0,
       0,       0,     307,     309,
};

static const int yypact[] = {
     -33,   -1000,   -1000,   -1000,   -1000,      97,     377,   -1000,
     124,     123,     294,     288,     287,     286,     285,   -1000,
   -1000,     270,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,
   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,     628,
     627,     625,     623,   -1000,   -1000,   -1000,   -1000,   -1000,
   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,
   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,     622,     621,
     617,     616,     613,   -1000,   -1000,   -1000,   -1000,   -1000,
     612,     605,     604,     603,     602,     601,     600,     599,
     598,     597,     596,     588,     587,     586,     579,     576,
     575,     574,     573,     570,     558,     555,     554,     550,
     549,     548,     547,     546,     545,     544,     543,     539,
     536,     535,     532,     518,     517,     513,     512,     511,
     510,     509,     494,     493,     492,     491,    -176,    -177,
     490,     488,     486,     339,     161,      97,     270,     270,
     633,     697,     697,   -1000,   -1000,     508,     149,     149,
     374,    -256,    -228,    -228,    -280,    -280,     697,     697,
   -1000,     270,     149,     149,     -97,     -98,    -178,    -180,
    -181,    -182,    -183,     697,     697,     697,     697,     697,
     697,     697,     697,     697,    -193,    -194,    -198,    -199,
     697,     485,     657,     697,     638,    -207,    -104,    -208,
     484,    -105,     483,     610,     522,    -318,    -319,    -132,
     332,     159,    -209,    -210,    -211,    -213,    -242,     482,
     481,     480,    -244,    -134,    -245,     479,    -246,     478,
    -106,     133,     133,     477,    -247,   -1000,   -1000,     107,
     297,   -1000,     515,   -1000,   -1000,   -1000,   -1000,   -1000,
   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,
   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,
   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,
   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,
   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,
   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,
   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,     384,
     476,     475,     474,     466,     465,     464,     463,     462,
     461,     459,     272,     268,     267,     211,     210,     184,
     183,     177,     458,     456,     455,     454,     176,   -1000,
   -1000,     449,     174,     173,     442,     439,     438,     437,
   -1000,     436,   -1000,     435,     172,     170,     434,     433,
     432,     430,     429,     169,     428,     164,     427,     156,
     153,     152,     151,   -1000,   -1000,   -1000,     150,     148,
     147,     426,   -1000,     425,   -1000,     424,     633,     633,
   -1000,     142,      97,     129,   -1000,   -1000,   -1000,   -1000,
   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,    -156,
    -192,    -249,    -251,    -252,    -257,    -320,    -111,   -1000,
   -1000,   -1000,   -1000,    -258,   -1000,    -118,    -119,   -1000,
   -1000,   -1000,   -1000,   -1000,   -1000,    -120,    -121,   -1000,
   -1000,   -1000,   -1000,   -1000,    -152,   -1000,    -122,   -1000,
    -259,    -260,    -261,    -262,    -269,    -270,    -272,   -1000,
   -1000,   -1000,     515,     515,    -273,   -1000,     508,     423,
     421,     414,     412,     405,     402,     401,     396,     380,
     376,     375,     365,     344,     338,     337,     317,     316,
     304,     303,     302,     299,     296,     295,     291,   -1000,
   -1000,     127,     123,   -1000,   -1000,   -1000,   -1000,   -1000,
   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,
   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,   -1000,
   -1000,   -1000,   -1000,   -1000,
};

static const int yypgo[] = {
       0,     676,      72,     672,     454,       4,       6,      13,
       8,     176,     905,      31,     172,     207,     212,      34,
     177,     174,     670,
};

static const int yyr1[] = {
       0,       1,       1,       1,       2,       2,       2,       3,
       3,       3,       5,       5,       5,       6,       6,       7,
       7,       8,       8,       9,       9,       9,       9,       9,
       9,       9,       9,       9,       9,       9,       9,       9,
       9,       9,       9,       9,       9,       9,       9,       9,
       9,       9,       9,       9,       9,       9,       9,       9,
       9,       9,       9,       9,       9,       9,       9,       9,
       9,       9,       9,       9,       9,       9,       9,       9,
       9,       9,       9,       4,       4,       4,       4,       4,
       4,       4,       4,       4,       4,       4,       4,       4,
       4,       4,       4,       4,       4,       4,       4,       4,
       4,       4,       4,       4,       4,       4,       4,       4,
       4,      10,      10,      10,      10,      10,      10,      10,
      10,      10,      10,      10,      10,      10,      10,      10,
      10,      11,      11,      11,      11,      11,      11,      11,
      11,      11,      11,      11,      11,      12,      12,      18,
      18,      18,      18,      18,      18,      18,      18,      18,
      18,      18,      18,      18,      18,      18,      18,      18,
      18,      18,      18,      18,      18,      18,      18,      18,
      18,      18,      18,      15,      15,      15,      15,      15,
      15,      15,      15,      15,      15,      15,      15,      15,
      15,      15,      15,      15,      16,      16,      16,      17,
      17,      13,      13,      13,      13,      13,      13,      13,
      14,      14,      14,
};

static const int yyr2[] = {
       0,       1,       1,       1,       3,       5,       1,       6,
       4,       1,       0,       1,       1,       0,       1,       0,
       1,       1,       3,       4,       4,       4,       4,       6,
       3,       3,       4,       6,       6,       4,       4,       4,
       4,       3,       4,       3,       4,       6,       6,       4,
       4,       4,       4,       4,       6,       4,       6,       4,
       6,       6,       6,       6,       3,       3,       3,       6,
       6,       4,       6,       3,       4,       3,       4,       5,
       5,       3,       6,       3,       3,       3,       3,       3,
       3,       3,       3,       3,       3,       3,       3,       3,
       3,       3,       3,       3,       3,       3,       3,       3,
       3,       3,       3,       3,       3,       1,       1,       3,
       2,       1,       1,       1,       1,       1,       1,       1,
       1,       1,       1,       1,       1,       4,       4,       4,
       4,       1,       1,       1,       1,       1,       1,       1,
       1,       1,       1,       1,       1,       1,       1,       1,
       1,       1,       1,       1,       1,       1,       4,       4,
       4,       4,       4,       1,       1,       1,       1,       1,
       4,       6,       6,       6,       6,       6,       6,       6,
       6,       6,       6,       1,       1,       1,       1,       1,
       1,       1,       1,       1,       1,       1,       1,       1,
       1,       1,       1,       1,       1,       1,       1,       1,
       1,       1,       1,       1,       1,       1,       1,       1,
       1,       1,       1,
};

static const int yychk[] = {
   -1000,      -1,      -2,      -3,     256,      40,      -4,     257,
     284,      -8,     326,     -10,     -12,     -13,     -14,     394,
     -18,      33,      -9,     275,     276,     277,     272,     273,
     274,     285,     286,     259,     297,     350,     351,     352,
     353,     354,     355,     289,     290,     295,     296,     294,
     298,     293,     291,     292,     299,     300,     301,     265,
     266,     268,     267,     269,     270,     271,     307,     308,
     361,     362,     364,     310,     312,     311,     385,     329,
     327,     330,     332,     331,     333,     384,     360,     341,
     342,     278,     279,     280,     281,     356,     363,     302,
     358,     383,     303,     262,     282,     305,     328,     313,
     393,     263,     321,     325,     324,     334,     314,     359,
     316,     315,     323,     322,     318,     317,     319,     264,
     306,     304,     346,     347,     348,     309,     357,     258,
     392,     386,      -2,      -4,     284,      63,     124,      38,
      -5,      61,     126,      42,      45,      44,      61,     126,
      61,     126,      61,     126,      61,     126,      61,     126,
      -4,      40,      40,      40,      40,      40,      40,      40,
      40,      40,      40,      40,      40,      40,      40,      40,
      40,      40,      40,      40,      40,      40,      40,      40,
      40,      40,      40,      40,      40,      40,      40,      40,
      40,      40,      40,      40,      40,      40,      40,      40,
      40,      40,      40,      40,      40,      40,      40,      40,
      40,      40,      40,      40,      40,      40,      40,      40,
      40,     284,     284,      40,      40,      41,      41,      -2,
      -4,      -4,      -6,      36,     -10,     -10,      -9,     -11,
     335,     292,     336,     337,     338,     339,     340,     343,
     344,     345,     349,     261,     -11,     -11,     -10,     284,
     -14,     -15,     -16,     -17,     366,     367,     370,     381,
     382,     368,     369,     371,     372,     373,     374,     375,
     376,     377,     378,     379,     380,     387,     388,     389,
     390,     391,     -11,     -10,     284,     -14,     -15,     -16,
     -17,     -13,     -13,     -12,     -12,     -10,     -10,      -4,
     -11,     -11,     260,     260,     284,     284,     284,     284,
     284,     -10,     -10,     -10,     -10,     -10,     -10,     -10,
     -10,     -10,     284,     284,     284,     284,     -10,      41,
      41,     -10,     -10,     -10,     260,     284,     260,     284,
      41,     260,      41,     260,     -10,     -10,     260,     320,
     320,     261,     261,     -10,     260,     -10,     284,     284,
     284,     284,     284,      41,      41,      41,     284,     -11,
     284,     284,      41,     284,      41,     260,      -5,      -5,
      41,     284,      58,      -7,      33,      41,      41,      41,
      41,      41,      41,      41,      41,      41,      41,      44,
      44,      44,      44,      44,      44,      44,      44,      41,
      41,      41,      41,      44,      41,      44,      44,      41,
      41,      41,      41,      41,      41,      44,      44,      41,
      41,      41,      41,      41,      44,      41,      44,      41,
      44,      44,      44,      44,      44,      44,      44,      41,
      41,      41,      -6,      -6,      44,      -2,      44,     284,
     -11,     284,     -11,     284,     284,     284,     284,     -15,
     260,     284,     260,     260,     260,     260,     261,     260,
     284,     284,     284,     284,     284,     284,     284,      -7,
      -7,     284,      -8,      41,      41,      41,      41,      41,
      41,      41,      41,      41,      41,      41,      41,      41,
      41,      41,      41,      41,      41,      41,      41,      41,
      41,      41,      41,      41,
};

static const int yydef[] = {
       0,      -2,       1,       2,       3,       0,       0,       6,
      10,       9,       0,       0,       0,       0,       0,      93,
      94,       0,      17,      97,      98,      99,     100,     101,
     102,     103,     104,     105,     106,     107,     108,       0,
       0,       0,       0,     125,     126,     177,     178,     179,
     180,     181,     182,     183,     184,     185,     186,     127,
     128,     129,     130,     131,     132,     133,       0,       0,
       0,       0,       0,     139,     140,     141,     142,     143,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
      13,       0,       0,      11,      12,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
      96,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,      10,      10,       0,       0,       4,      95,       0,
      67,      68,      15,      14,      77,      78,      18,      69,
     113,     114,     115,     116,     117,     118,     119,     120,
     121,     122,     123,     124,      70,      71,      73,      75,
      83,      85,      89,      91,     155,     156,     157,     158,
     159,     160,     161,     162,     163,     164,     165,     166,
     167,     168,     169,     170,     171,     172,     173,     174,
     175,     176,      72,      74,      76,      84,      86,      90,
      92,      79,      80,      81,      82,      87,      88,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,      24,
      25,       0,       0,       0,       0,       0,       0,       0,
      33,       0,      35,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,      52,      53,      54,       0,       0,
       0,       0,      59,       0,      61,       0,      13,      13,
      65,       0,       0,       8,      16,     109,     110,     111,
     112,     134,     135,     136,     137,     138,     144,       0,
       0,       0,       0,       0,       0,       0,       0,      19,
      20,      21,      22,       0,      26,       0,       0,      29,
      30,      31,      32,      34,      36,       0,       0,      39,
      40,      41,      42,      43,       0,      45,       0,      47,
       0,       0,       0,       0,       0,       0,       0,      57,
      60,      62,      15,      15,       0,       5,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,       0,
       0,       0,       0,       0,       0,       0,       0,      63,
      64,       0,       7,     145,     146,     147,     148,     149,
     150,     151,     152,     153,     154,      23,      27,      28,
      37,      38,      44,      46,      48,      49,      50,      51,
      55,      56,      58,      66,
};

////////////////////////////////////////////////////////////////////////////
// 
// YACCPAR.C - PCYACC LR parser driver routine for use with improved errorlib
//
//	(c) Copyright 1989, Abraxas Software, Inc.
//
//  Adaptada a C++ por F.Suárez   8/11/1993
//
//  Adaptada para el lenguaje de diccionario
////////////////////////////////////////////////////////////////////////////
//#include "errorlib.h"

#define yyerrok     errfl = 0
#define yyclearin	if (token == 0) return 1; else token = -1
#define YYFLAG -1000
#define YYACCEPT return(0)
#define YYABORT return(1)

#ifndef YYMAXDEPTH
#define YYMAXDEPTH	100
#endif
#ifdef YYTFLAG
	static int redseq[YYREDMAX];
	static int redcnt;
#endif

//YYSTYPE yyv[YYMAXDEPTH];
//int token = -1; /* input token */
//int errct = 0;  /* error count */
//int errfl = 0;  /* error flag */

 
int CLengDic::yyparse(void)
	{
	int yys[YYMAXDEPTH];
	int yyj, yym;
	YYSTYPE *yypvt;
	int yystate, *yyps, yyn;
	YYSTYPE *yypv;
	const int *yyxi;

#ifdef YYDEBUG
  YYTRACE("state 0 _PtrIni=%s\n", _PtrIni);
#endif
	yystate = 0;
	token = -1;
	errct = 0;
	errfl = 0;
	yyps = yys - 1;
	yypv = yyv - 1;

yystack:    /* put a state and value onto the stack */
  if ( ++yyps > yys + YYMAXDEPTH ) {
    yyerror( "yacc stack overflow", "" );
    return(1);
    }
  *yyps = yystate;
  ++yypv;
  *yypv = yyval;

yynewstate:
  yyn = yypact[yystate];
  YYTRACE("newstate %d _PtrIni=%s", yyn, _PtrIni);

  if ( yyn <= YYFLAG )
	  goto yydefault; /* simple state with only default action */

  if ( token < 0 ) {		/* no lookahead token? */
	  if ( (token = yylex()) < 0 )
	    token = 0;
    }

	yyn += token;
  if ( yyn < 0 || yyn >= YYLAST )	/* no entry within table? */
	  goto yydefault;	/* simple state with only default action */

  if ( yychk[yyn = yyact[yyn]] == token ) {	/* valid shift? */
	  YYTRACE("shift on [%s] to state %d\n", yydisplay(token), yyn);
	  token = -1;		/* lookahead token is read */
	  yyval = yylval;
	  
	  yystate = yyn;
	  if ( errfl > 0 )	/* still in error state? */
	     --errfl;
	  goto yystack;
    }

yydefault:	/* simple state with only default action */
  if ( (yyn = yydef[yystate]) == -2 ) {
	  if ( token < 0 ) {	/* no lookahead token? */
	    if ( (token = yylex()) < 0 )
		    token = 0;
	  }

          /* look through exception table */
  for ( yyxi = yyexca; *yyxi != -1 || yyxi[1] != yystate ; yyxi += 2 )
	       ;		/* VOID */
  while( *(yyxi += 2) >= 0 ) {
	  if ( *yyxi == token )
	    break;
    }

  if ( (yyn = yyxi[1]) < 0 ) {
#ifdef YYTFLAG
    yytfilep = fopen(yytfilen, "w");
    if (yytfilep == NULL) {
      fprintf(stderr, "Can't open t file: %s\n", yytfilen);
      return(0);          }
      for (ti=redcnt-1; ti>=0; ti--) {
        tj = svdprd[redseq[ti]];
        while (strcmp(svdnams[tj], "$EOP"))
          fprintf(yytfilep, "%s ", svdnams[tj++]);
        fprintf(yytfilep, "\n");
        }
      fclose(yytfilep);
#endif
	    YYTRACE("accept\n");
	    return 0;   /* accept */
			}
    }

  if ( yyn == 0 ) {		/* error? */
	  switch( errfl ) {
	    case 0:   /* brand new error */
        yyerror( "syntax error",yydisplay( token ));
		    if ( (yyn = yypact[yystate]) > YYFLAG && yyn < YYLAST) {
					register int x;

					for ( x = (yyn > 0) ? yyn : 0; x < YYLAST; ++x ) {
			      if ( yychk[yyact[x]] == x - yyn &&
								x - yyn != YYERRCODE )
							yyerror( "", yydisplay( x - yyn ));
						}
					}
		    yyerror( "", "");
		    ++errct;

	  case 1:
	  case 2: /* incompletely recovered error ... try again */
		    errfl = 3;

		    /* find a state where "error" is a legal shift action */
		    while ( yyps >= yys ) {
			 yyn = yypact[*yyps] + YYERRCODE;
			 if ( yyn >= 0 && yyn < YYLAST &&
					yychk[yyact[yyn]] == YYERRCODE ){
			      /* simulate a shift of "error" */
			      yystate = yyact[yyn];
			      YYTRACE("shift on [error] to state %d\n", yystate);
			      goto yystack;
			 }
		         yyn = yypact[*yyps];
			 /* the current yyps has no shift on "error",
				pop stack */
			--yyps;
			--yypv;
			YYTRACE("error: pop state %d, uncovering state %d\n", yyps[1], yyps[0]);
	    }

	    /* there is no state on the stack with an error shift ... abort */
yyabort:

#ifdef YYTFLAG
              yytfilep = fopen(yytfilen, "w");
              if (yytfilep == NULL) {
                fprintf(stderr, "Can't open t file: %s\n", yytfilen);
                return(1);              }
              for (ti=1; ti<redcnt; ti++) {
                tj = svdprd[redseq[ti]];
                while (strcmp(svdnams[tj], "$EOP"))
                  fprintf(yytfilep, "%s ", svdnams[tj++]);
                fprintf(yytfilep, "\n");
              }
              fclose(yytfilep);
#endif
		    YYTRACE("abort\n");
		    return 1;

	       case 3:  /* no shift yet; clobber input char */
		    YYTRACE("error: discard [%s]\n", yydisplay(token));
		    if ( token == 0 )	/* EOF? */
			 goto yyabort;		/* don't discard EOF, quit */
		    token = -1;
		    goto yynewstate;   /* try again in the same state */
	  }
     }

     /* reduction by production yyn */
#ifdef YYTFLAG
    if (yytflag && redcnt<YYREDMAX)
        redseq[redcnt++] = yyn;
#endif
     yyps -= yyr2[yyn];
     yypvt = yypv;
     yypv -= yyr2[yyn];
     yyval = yypv[1];

     yym = yyn;
     /* consult goto table to find next state */
     yyn = yyr1[yyn];
     yyj = yypgo[yyn] + *yyps + 1;
     if ( yyj >= YYLAST || yychk[yystate = yyact[yyj]] != -yyn )
	  yystate = yyact[yypgo[yyn]];
#ifdef YYDEBUG
    YYTRACE("reduce w/ rule %d, uncover state %d, go to state %d\n", yym, *yyps, yystate);
		m_ptrWord->Dump();
#endif
    switch (yym) {
	  
case 1:
# line 99 "LengDic.y"
{
                YYTRACE("Sentencia\n");
                if(m_SyntaxChk == FALSE)
                    {
                    m_Trad = yypvt[-0].string;
                    }
                } break;
case 2:
# line 107 "LengDic.y"
{
                YYTRACE("Directo\n");
                if(m_SyntaxChk == FALSE)
                    {
                    m_Trad = yypvt[-0].string;
                    }
                directo = TRUE;
                } break;
case 4:
# line 119 "LengDic.y"
{
                if(m_SyntaxChk == FALSE)
                    {
                    yyval.string = yypvt[-1].string;
                    m_Trad = yyval.string;
                    }
                } break;
case 5:
# line 127 "LengDic.y"
{
                if(m_SyntaxChk == FALSE)
                    {
                    if(yypvt[-4].integer)
                        yyval.string = yypvt[-2].string;
                    else
                        yyval.string = yypvt[-0].string;
                    
                    m_Trad = yyval.string;
                    }
                } break;
case 6:
# line 139 "LengDic.y"
{
                yyval.string = yypvt[-0].string;
                } break;
case 7:
# line 146 "LengDic.y"
{
                YYTRACE("Caso sustantivos");
                if(m_SyntaxChk == FALSE)
                    {
                    GetPtrWord()->m_Genero = (Genero)yypvt[-4].integer;
                    GetPtrWord()->m_Numero = (Numero)yypvt[-3].integer;
                    if(yypvt[-2].integer == 1)
                        GetPtrWord()->m_Reflexivo = (BOOL)yypvt[-2].integer;
                    yyval.string = yypvt[-5].string;                  
                    
                    if(m_LlenaAcep == TRUE)
                        {
                        CWordEsp *tmp = new CWordEsp();
                        tmp->m_Genero = (Genero)yypvt[-4].integer;
                        tmp->m_Numero = (Numero)yypvt[-3].integer;
                        tmp->m_Reflexivo = (BOOL)yypvt[-2].integer;
                        tmp->m_Esp  = "GG";
                        tmp->m_Word = yypvt[-5].string;
                        GetPtrWord()->m_AcepArray.InsertAt(0,tmp);
                        }
                    }
                } break;
case 8:
# line 169 "LengDic.y"
{
                YYTRACE("Caso sustantivos");
                if(m_SyntaxChk == FALSE)
                    {
                    GetPtrWord()->m_Genero = (Genero)yypvt[-2].integer;
                    GetPtrWord()->m_Numero = (Numero)yypvt[-1].integer;
                    if(yypvt[-0].integer == 1)
                        GetPtrWord()->m_Reflexivo = (BOOL)yypvt[-0].integer;
                    GetPtrWord()->m_Reflexivo = (BOOL)yypvt[-0].integer;
                    yyval.string = yypvt[-3].string;

                    if(m_LlenaAcep == TRUE)
                        {
                        CWordEsp *tmp = new CWordEsp();
                        tmp->m_Genero = (Genero)yypvt[-2].integer;
                        tmp->m_Numero = (Numero)yypvt[-1].integer;
                        tmp->m_Reflexivo = (BOOL)yypvt[-0].integer;
                        tmp->m_Esp  = "GG";
                        tmp->m_Word = yypvt[-3].string;
                        GetPtrWord()->m_AcepArray.InsertAt(0,tmp);
                        }
                    }
                } break;
case 9:
# line 193 "LengDic.y"
{                      /* sólo se ejecuta la función*/
                YYTRACE("caso de sufijos y prefijos ");
                } break;
case 10:
# line 199 "LengDic.y"
{ 
            yyval.integer = (GetPtrWord()->m_Femenino == TRUE)?FEMENINO:MASCULINO;
            } break;
case 11:
# line 203 "LengDic.y"
{ 
						yyval.integer = FEMENINO;
						} break;
case 12:
# line 205 "LengDic.y"
{ 
						yyval.integer = NEUTRO;
						} break;
case 13:
# line 209 "LengDic.y"
{
						yyval.integer = (GetPtrWord()->m_Plural == TRUE || GetPtrWord()->m_Numero == PLURAL)?PLURAL:SINGULAR;
						} break;
case 14:
# line 211 "LengDic.y"
{ 
						yyval.integer = PLURAL;
						} break;
case 15:
# line 215 "LengDic.y"
{
						yyval.integer = 0;
						} break;
case 16:
# line 217 "LengDic.y"
{
						yyval.integer = 1;
						} break;
case 19:
# line 226 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                GetPtrSentence()->SetWordTradMorf(GetPtrSentence()->GetXNode(_PtrNodo,1,FALSE),yypvt[-1].string,-1);
           } break;
case 20:
# line 231 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                GetPtrSentence()->SetWordTradMorf(GetPtrSentence()->GetXNode(_PtrNodo,2,FALSE),yypvt[-1].string,-1);
           } break;
case 21:
# line 236 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                GetPtrSentence()->SetWordTradMorf(GetPtrSentence()->GetXNode(_PtrNodo,1,TRUE),yypvt[-1].string,-1);
           } break;
case 22:
# line 241 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                GetPtrSentence()->SetWordTradMorf(GetPtrSentence()->GetXNode(_PtrNodo,2,TRUE),yypvt[-1].string,-1);
           } break;
case 23:
# line 246 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                 if(yypvt[-3].word != NULL)
                    GetPtrSentence()->SetWordTradMorf(yypvt[-3].word,yypvt[-1].string,-1);
           } break;
case 24:
# line 252 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                 ASSERT(GetPtrWord() !=NULL );
                }
            } break;
case 25:
# line 259 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                 ASSERT(GetPtrWord() !=NULL );
                 GetPtrWord()->m_Delete = TRUE;
                }
            } break;
case 26:
# line 267 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                 if(yypvt[-1].word != NULL)
                     yypvt[-1].word->m_Delete = TRUE;
                }
            } break;
case 27:
# line 276 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                 if(yypvt[-3].word != NULL)
                     yypvt[-3].word->m_Articulo = yypvt[-1].integer;
                }
            } break;
case 28:
# line 284 "LengDic.y"
{                        /* Adiciona nodo detrás de la palabra dada */
           if(m_SyntaxChk == FALSE)
                {
                 if(yypvt[-3].word != NULL)
                     yypvt[-3].word->m_Adiciona = yypvt[-1].integer;
                }
            } break;
case 29:
# line 292 "LengDic.y"
{                        /* Adiciona nodo detrás de la palabra dada */
           if(m_SyntaxChk == FALSE)
                {
                 ASSERT(GetPtrWord() !=NULL );
                 GetPtrWord()->m_Adiciona = yypvt[-1].integer;
                }
            } break;
case 30:
# line 300 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                GetPtrSentence()->SetWordTradMorf(GetPtrSentence()->GetFirstWord(),yypvt[-1].string,-1);
           } break;
case 31:
# line 305 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                 ASSERT(GetPtrWord() !=NULL );
                 GetPtrWord()->m_Articulo = yypvt[-1].integer;
                }
           } break;
case 32:
# line 313 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)            /* esta mal debe cambiarse por SETVERBO (numero , numero, numero)*/
                {
                ASSERT(GetPtrWord() !=NULL );
                if(yypvt[-1].string.GetLength() != 4) 
                    {
                    GetPtrWord()->m_Modo = mINFINITIVO;
                    GetPtrWord()->m_Tiempo = PRESENTE;
                    GetPtrWord()->m_Numero = SINGULAR;
                    GetPtrWord()->m_Persona = TERCERAP;
                    }
                else
                    {
                    char c = yypvt[-1].string.GetAt(0); 
                    
                    if(c >= '0' && c <= '9')
                        GetPtrWord()->m_Modo = (Modo)(c - '0');
                    c = yypvt[-1].string.GetAt(1); 
                    if(c >= '0' && c <= '9')
                        GetPtrWord()->m_Tiempo = (Tiempo)(c - '0');
                    c = yypvt[-1].string.GetAt(2); 
                    if(c >= '0' && c <= '9')
                        GetPtrWord()->m_Numero = (Numero)(c - '0');
                    c = yypvt[-1].string.GetAt(3); 
                    if(c >= '0' && c <= '9')
                        GetPtrWord()->m_Persona = (Persona)(c - '0');
                    }
                }
           } break;
case 33:
# line 343 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                ASSERT(GetPtrWord() !=NULL );
                GetPtrWord()->m_Reflexivo = TRUE;
                }
           } break;
case 34:
# line 351 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                ASSERT(GetPtrWord() !=NULL );
                GetPtrWord()->m_Case = (Caso)yypvt[-1].integer;
                }
           } break;
case 35:
# line 359 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                ASSERT(GetPtrWord() !=NULL );
                GetPtrWord()->m_LastPrepNoArt = TRUE;
                }
           } break;
case 36:
# line 368 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                 ASSERT(GetPtrWord() !=NULL );
                 GetPtrWord()->m_Negado = (BOOL)yypvt[-1].integer;
                }
           } break;
case 37:
# line 376 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                 ASSERT(yypvt[-3].word !=NULL );
                 if(yypvt[-3].word != NULL)
                    yypvt[-3].word->m_Negado = (BOOL)yypvt[-1].integer;
                }
           } break;
case 38:
# line 386 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                 ASSERT(yypvt[-3].word !=NULL );
                 if(yypvt[-3].word != NULL)
                    yypvt[-3].word->m_Modo = (Modo)yypvt[-1].integer;
                }
           } break;
case 39:
# line 396 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                 ASSERT(GetPtrWord() !=NULL );
                 GetPtrWord()->m_Modo = (Modo)yypvt[-1].integer;
                }
           } break;
case 40:
# line 405 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                 ASSERT(GetPtrWord() !=NULL );
                 GetPtrWord()->m_sPrefijo = yypvt[-1].string;
                }
           } break;
case 41:
# line 413 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                 ASSERT(GetPtrWord() !=NULL );
                 GetPtrWord()->m_sSufijo = yypvt[-1].string;
                }
           } break;
case 42:
# line 421 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                 ASSERT(GetPtrWord() !=NULL );
                 GetPtrWord()->m_sTipo2 = yypvt[-1].string;
                }
           } break;
case 43:
# line 429 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                 ASSERT(GetPtrWord() !=NULL );
                 GetPtrWord()->m_sTipo = yypvt[-1].string;
                }
           } break;
case 44:
# line 437 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                 ASSERT(yypvt[-3].word !=NULL );
                 if(yypvt[-3].word != NULL)
                    yypvt[-3].word->m_sTipo = yypvt[-1].string;
                }
           } break;
case 45:
# line 446 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                 ASSERT(GetPtrWord() !=NULL );
                 GetPtrWord()->m_Tiempo = (Tiempo)yypvt[-1].integer;
                }
           } break;
case 46:
# line 454 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                 ASSERT(yypvt[-3].word !=NULL );
                 if(yypvt[-3].word != NULL)
                    yypvt[-3].word->m_Tiempo = (Tiempo)yypvt[-1].integer;
                }
           } break;
case 47:
# line 463 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                ASSERT(GetPtrWord() !=NULL );
                GetPtrWord()->m_Patron = yypvt[-1].string[0];
                }
           } break;
case 48:
# line 471 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                 ASSERT(GetPtrWord() !=NULL );
                 CStringA stmp("^(");
                 stmp += yypvt[-3].string;
                 stmp += ")";
                 CSGrep setprefijo(stmp,yypvt[-1].string);    // compile a regexp to sgrep
                 setprefijo.Replace(GetPtrWord()->m_Key,TRUE);
                }
           } break;
case 49:
# line 483 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                 ASSERT(GetPtrWord() !=NULL );
                 CStringA stmp("(");
                 stmp += yypvt[-3].string;
                 stmp += ")$";
                 if(yypvt[-1].string.Find(',') != -1)
                    yypvt[-1].string = ',' + yypvt[-1].string;
                 CSGrep setsufijo(stmp,yypvt[-1].string);    // compile a regexp to sgrep
                 setsufijo.Replace(GetPtrWord()->m_Key,TRUE);
                }
           } break;
case 50:
# line 497 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                 ASSERT(GetPtrWord() !=NULL );
                 CStringA stmp("^(");
                 stmp += yypvt[-3].string;
                 stmp += ")";
                 CSGrep setprefijo(stmp,yypvt[-1].string);    // compile a regexp to sgrep
                 setprefijo.Replace(GetPtrWord()->m_Trad,TRUE);
                }
           } break;
case 51:
# line 509 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                 if(yypvt[-3].string.IsEmpty()==TRUE)
                    {
#if !defined(MERCADO)
                    AfxMessageBox(IDS_EMPTYSTRING,MB_ICONSTOP);
#endif
                    }
                 else
                    {   
                     ASSERT(GetPtrWord() !=NULL );
                     CStringA stmp("(");
                     stmp += yypvt[-3].string;
                     stmp += ")$";  
                     CSGrep setsufijo(stmp,yypvt[-1].string);    // compile a regexp to sgrep
                     setsufijo.Replace(GetPtrWord()->m_Trad,TRUE);
                     }
                }
           } break;
case 52:
# line 530 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)     /* pone la bandera m_Plural de word en falso*/
                {
                 ASSERT(GetPtrWord() !=NULL );
                 GetPtrWord()->m_Plural = FALSE;
                }
           } break;
case 53:
# line 539 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)     /* pone la bandera m_Plural de word */
                {
                 ASSERT(GetPtrWord() !=NULL );
                 GetPtrWord()->m_Plural = TRUE;
                }
           } break;
case 54:
# line 548 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)     /* pone la bandera m_Plural de word */
                {
                 ASSERT(GetPtrWord() !=NULL );
                 GetPtrWord()->m_Femenino = TRUE;
                 GetPtrWord()->m_Genero = FEMENINO;
                }
           } break;
case 55:
# line 558 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {    
                POSITION nodo = _PtrNodo;
                ASSERT(nodo != NULL);
                //GetPtrSentence()->m_Ora.GetNext(nodo);
				
                if(nodo != NULL)
                    {  
                    //CWord *ptrw = (LPCWORD)GetPtrSentence()->m_Ora.GetAt(nodo);
                    //CWord *ptrw = (LPCWORD)GetPtrSentence()->NextNotType(_PtrNodo,GetPtrSourceLanguageData()->GetAdverbialType());
                    CWord *ptrw = (LPCWORD)GetPtrSentence()->NextNotType(_PtrNodo,"DD,AA,AP,AI,RD,RI,SS,NP,FN,AE,AN,NN,OR,OC");
                    if(ptrw != NULL && ptrw->m_Origlwr == yypvt[-3].string)
                        {
                        ptrw->m_StrReserva1 = ptrw->m_Trad = yypvt[-1].string;
                        ptrw->m_Traducida = TRUE;
                        }
                    }
                }
           } break;
case 56:
# line 579 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {    
				GetPtrSentence()->IsLastNotDDSetByType(_PtrNodo,yypvt[-3].string,yypvt[-1].string,-1,GetPtrSourceLanguageData()->GetAdverbialType());
                }
           } break;
case 57:
# line 586 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {    
                GetPtrWord()->m_Prep = yypvt[-1].string;
                }
           } break;
case 58:
# line 593 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {    
                GetPtrSentence()->IsLastNotDDSetByWord(_PtrNodo,yypvt[-3].string,yypvt[-1].string);
                }
           } break;
case 59:
# line 600 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {                            
                GetPtrWord()->m_NotArtInOf = TRUE;
                }
           } break;
case 60:
# line 607 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {                     
                GetPtrWord()->m_OfAs = yypvt[-1].string;
                }
           } break;
case 61:
# line 615 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {                            
                GetPtrWord()->m_ArtAtFirst = TRUE;
                }                      
           } break;
case 62:
# line 622 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {
                 ASSERT(GetPtrWord() !=NULL );
                 GetPtrWord()->m_Persona = (Persona)(yypvt[-1].integer-1);
                }
           } break;
case 63:
# line 630 "LengDic.y"
{
            YYTRACE("especialidad de sustantivos o adjetivos");
           if(m_SyntaxChk == FALSE)
                {
                CWordEsp *tmp = new CWordEsp();
                tmp->m_Genero = (Genero)yypvt[-2].integer;
                tmp->m_Numero = (Numero)yypvt[-1].integer;
                tmp->m_Reflexivo = (BOOL)yypvt[-0].integer;
                tmp->m_Esp  = yypvt[-4].string;
                tmp->m_Word = yypvt[-3].string;
                GetPtrWord()->m_AcepArray.Add(tmp);
                }
           } break;
case 64:
# line 644 "LengDic.y"
{
            YYTRACE("especialidad de sustantivos o adjetivos");
           if(m_SyntaxChk == FALSE)
                {
                CWordEsp *tmp = new CWordEsp();
                tmp->m_Genero = (Genero)yypvt[-2].integer;
                tmp->m_Numero = (Numero)yypvt[-1].integer;
                tmp->m_Reflexivo = (BOOL)yypvt[-0].integer;
                tmp->m_Esp        = "GG";
                tmp->m_Word       = yypvt[-3].string;
								StringToArray(yypvt[-4].string,&tmp->m_Semantica,";,");
                //tmp->m_Semantica  = yypvt[-3].string;
                GetPtrWord()->m_AcepArray.Add(tmp);
                }
           } break;
case 65:
# line 660 "LengDic.y"
{
            YYTRACE("Borra todos los acentos de la palabra");
           if(m_SyntaxChk == FALSE)
                {
				        DelAccent(GetPtrWord()->m_Key);
                }
           } break;
case 66:
# line 668 "LengDic.y"
{
           YYTRACE("Inserta una palabra detrás con el tipo dado");
           if(m_SyntaxChk == FALSE)
                {
				GetPtrSentence()->InsertWordAfter(_PtrNodo,yypvt[-3].string,yypvt[-1].string);
                }
           } break;
case 67:
# line 678 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                {
                yyval.integer = (yypvt[-2].integer || yypvt[-0].integer);
                }
             } break;
case 68:
# line 685 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                {
                yyval.integer = (yypvt[-2].integer && yypvt[-0].integer);
                }
             } break;
case 69:
# line 692 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {                  
                 if(GetPtrWord() != NULL)
                    yyval.integer = (yypvt[-0].string.Find(GetPtrWord()->m_sTipo2) != -1);
                 else
                    yyval.integer = FALSE;
                 }
             } break;
case 70:
# line 702 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {                  
                 if(GetPtrWord() != NULL)
                    yyval.integer = (yypvt[-0].string.Find(GetPtrWord()->m_sTipo2) == -1);
                 else
                    yyval.integer = TRUE;
                 }
             } break;
case 71:
# line 712 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {                  
                 if(yypvt[-2].word != NULL)
                     yyval.integer = (yypvt[-0].string.Find(yypvt[-2].word->m_sTipo) != -1);
                 else
                    yyval.integer = FALSE;
                 }
             } break;
case 72:
# line 722 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 if(yypvt[-2].word != NULL)
                     yyval.integer = (yypvt[-0].string.Find(yypvt[-2].word->m_sTipo) == -1);
                 else
                    yyval.integer = TRUE;
                 }
             } break;
case 73:
# line 756 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer = ((yypvt[-2].word == yypvt[-0].word) && yypvt[-2].word != NULL);
                 }
             } break;
case 74:
# line 763 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer = (yypvt[-2].word != yypvt[-0].word);
                 }
             } break;
case 75:
# line 770 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 if(yypvt[-2].word != NULL)
                     //yyval.integer = (yypvt[-2].word->m_Key == yypvt[-0].string)?1:0;
                     yyval.integer = (yypvt[-2].word->m_Origlwr == yypvt[-0].string)?1:0;
                 else
                    yyval.integer = FALSE;
                 }
             } break;
case 76:
# line 782 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 if(yypvt[-2].word != NULL)
                     yyval.integer = (yypvt[-2].word->m_Origlwr != yypvt[-0].string)?1:0;
                 else
                    yyval.integer = TRUE;
                 }
             } break;
case 77:
# line 793 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 if(yypvt[-0].word != NULL)
                     yyval.integer = (yypvt[-0].word->m_Origlwr == yypvt[-2].string)?1:0;
                 else
                    yyval.integer = FALSE;
                 }
             } break;
case 78:
# line 804 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 if(yypvt[-0].word != NULL)
                     yyval.integer = (yypvt[-0].word->m_Origlwr != yypvt[-2].string)?1:0;
                 else
                    yyval.integer = TRUE;
                 }
             } break;
case 79:
# line 815 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer=(yypvt[-2].integer == yypvt[-0].integer);
                 }
             } break;
case 80:
# line 822 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer=(yypvt[-2].integer != yypvt[-0].integer);
                 }
             } break;
case 81:
# line 829 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer=(yypvt[-2].integer == yypvt[-0].integer);
                 }
             } break;
case 82:
# line 836 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer=(yypvt[-2].integer != yypvt[-0].integer);
                 }
             } break;
case 83:
# line 843 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 if(yypvt[-2].word != NULL)
                     yyval.integer = (yypvt[-2].word->m_Tiempo == (Tiempo)yypvt[-0].integer);
                 else 
                     yyval.integer = FALSE;
                 }
             } break;
case 84:
# line 853 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 if(yypvt[-2].word != NULL)
                     yyval.integer = (yypvt[-2].word->m_Tiempo != (Tiempo)yypvt[-0].integer);
                 else 
                     yyval.integer = TRUE;
                 }
             } break;
case 85:
# line 863 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 if(yypvt[-2].word != NULL)
                     yyval.integer = (yypvt[-2].word->m_Modo == (Modo)yypvt[-0].integer);
                 else 
                     yyval.integer = FALSE;
                 }
             } break;
case 86:
# line 873 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 if(yypvt[-2].word != NULL)
                     yyval.integer = (yypvt[-2].word->m_Modo != (Modo)yypvt[-0].integer);
                 else 
                     yyval.integer = TRUE;
                 }
             } break;
case 87:
# line 883 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 if(yypvt[-0].word != NULL)
                     yyval.integer = (yypvt[-0].word->m_Tiempo == (Tiempo)yypvt[-2].integer);
                 else 
                     yyval.integer = FALSE;
                 }
             } break;
case 88:
# line 893 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 if(yypvt[-0].word != NULL)
                     yyval.integer = (yypvt[-0].word->m_Tiempo != (Tiempo)yypvt[-2].integer);
                 else 
                     yyval.integer = TRUE;
                 }
             } break;
case 89:
# line 903 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 if(yypvt[-2].word != NULL)
                     yyval.integer = (yypvt[-2].word->m_Persona == (Persona)yypvt[-0].integer);
                 else 
                     yyval.integer = FALSE;
                 }
             } break;
case 90:
# line 913 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 if(yypvt[-2].word != NULL)
                     yyval.integer = (yypvt[-2].word->m_Persona != (Persona)yypvt[-0].integer);
                 else 
                     yyval.integer = TRUE;
                 }
             } break;
case 91:
# line 923 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 if(yypvt[-2].word != NULL)
                     yyval.integer = (yypvt[-2].word->m_Numero == (Numero)yypvt[-0].integer);
                 else 
                     yyval.integer = FALSE;
                 }
             } break;
case 92:
# line 933 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 if(yypvt[-2].word != NULL)
                     yyval.integer = (yypvt[-2].word->m_Numero != (Numero)yypvt[-0].integer);
                 else 
                     yyval.integer = TRUE;
                 }
             } break;
case 93:
# line 943 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 if(GetTranslationUserPtr()->GetInformalPronoun() == TRUE)
                     yyval.integer = TRUE;
                 else 
                     yyval.integer = FALSE;
                 }
			} break;
case 94:
# line 953 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer = yypvt[-0].integer;
                 }
             } break;
case 95:
# line 960 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer = yypvt[-1].integer;
                 }
             } break;
case 96:
# line 967 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer = !yypvt[-0].integer;
                 }
             } break;
case 97:
# line 976 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 yyval.word = GetPtrSentence()->GetXNode(_PtrNodo,1,TRUE);
             } break;
case 98:
# line 981 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 yyval.word = GetPtrSentence()->GetXNode(_PtrNodo,2,TRUE);
             } break;
case 99:
# line 986 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 yyval.word = GetPtrSentence()->GetXNode(_PtrNodo,3,TRUE);
             } break;
case 100:
# line 991 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 yyval.word = GetPtrSentence()->GetXNode(_PtrNodo,1,FALSE);
             } break;
case 101:
# line 996 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 yyval.word = GetPtrSentence()->GetXNode(_PtrNodo,2,FALSE);
             } break;
case 102:
# line 1001 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 yyval.word = GetPtrSentence()->GetXNode(_PtrNodo,3,FALSE);
             } break;
case 103:
# line 1006 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 ASSERT(GetPtrSentence()->GetFirstWord() !=NULL );
                 yyval.word = GetPtrSentence()->GetFirstWord();
                 ASSERT( yyval.word != NULL );
                 }
             } break;
case 104:
# line 1015 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 ASSERT(GetPtrSentence()->GetLastWord() !=NULL );
                 yyval.word = GetPtrSentence()->GetLastWord();
                 ASSERT( yyval.word != NULL );
                 }
             } break;
case 105:
# line 1024 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 ASSERT(GetPtrWord() !=NULL );
                 yyval.word = GetPtrWord();
                 ASSERT( yyval.word != NULL );
                 }
             } break;
case 106:
# line 1033 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 ASSERT(GetPtrSentence()->GetVerbo() !=NULL );
                 yyval.word = GetPtrSentence()->GetVerbo();
                 }
             } break;
case 107:
# line 1041 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 ASSERT(GetPtrSentence() !=NULL );
                 yyval.word = GetPtrSentence()->LastNotType(_PtrNodo,GetPtrSourceLanguageData()->GetAdverbialType());
                 }
             } break;
case 108:
# line 1049 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 ASSERT(GetPtrSentence() !=NULL );
                 yyval.word = GetPtrSentence()->NextNotType(_PtrNodo,GetPtrSourceLanguageData()->GetAdverbialType());
                 }
             } break;
case 109:
# line 1057 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 ASSERT(GetPtrSentence() !=NULL );
                 yyval.word = GetPtrSentence()->LastNotType(_PtrNodo,yypvt[-1].string);
                 }
             } break;
case 110:
# line 1065 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 ASSERT(GetPtrSentence() !=NULL );
                 yyval.word = GetPtrSentence()->NextNotType(_PtrNodo,yypvt[-1].string);
                 }
             } break;
case 111:
# line 1073 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 yyval.word = GetPtrSentence()->GetXNode(_PtrNodo,yypvt[-1].integer,TRUE);
             } break;
case 112:
# line 1078 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 yyval.word = GetPtrSentence()->GetXNode(_PtrNodo,yypvt[-1].integer,FALSE);
             } break;
case 113:
# line 1085 "LengDic.y"
{
            yyval.string = GetPtrSourceLanguageData()->GetAdverbialType();
            } break;
case 114:
# line 1089 "LengDic.y"
{
            yyval.string = GetPtrSourceLanguageData()->GetVerbalType();
            } break;
case 115:
# line 1093 "LengDic.y"
{
            yyval.string = GetPtrSourceLanguageData()->GetArticleType();
            } break;
case 116:
# line 1097 "LengDic.y"
{
            yyval.string = GetPtrSourceLanguageData()->GetNounType();
            } break;
case 117:
# line 1101 "LengDic.y"
{
            yyval.string = GetPtrSourceLanguageData()->GetAdjectiveType();
            } break;
case 118:
# line 1105 "LengDic.y"
{
            yyval.string = GetPtrSourceLanguageData()->GetProNounType();
            } break;
case 119:
# line 1109 "LengDic.y"
{
            yyval.string = GetPtrSourceLanguageData()->GetGerundType();
            } break;
case 120:
# line 1113 "LengDic.y"
{
            yyval.string = GetPtrSourceLanguageData()->GetParticipleType();
            } break;
case 121:
# line 1117 "LengDic.y"
{
            yyval.string = GetPtrSourceLanguageData()->GetPastOrParticipleType();
            } break;
case 122:
# line 1121 "LengDic.y"
{
            yyval.string = GetPtrSourceLanguageData()->GetPastType();
            } break;
case 123:
# line 1125 "LengDic.y"
{
            yyval.string = GetPtrSourceLanguageData()->GetPrepositionType();
            } break;
case 124:
# line 1129 "LengDic.y"
{
            yyval.string = yypvt[-0].string;
            } break;
case 125:
# line 1135 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer = GetPtrSentence()->GetLastParte(_PtrNodo);
                 }
             } break;
case 126:
# line 1142 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer=GetPtrSentence()->GetNextParte(_PtrNodo);
                 }
             } break;
case 127:
# line 1151 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer = (GetPtrWord()->m_OParte==(OParte)opCD);
                 }
             } break;
case 128:
# line 1158 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer = (GetPtrWord()->m_OParte==(OParte)opCI);
                 }
             } break;
case 129:
# line 1165 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer = (GetPtrWord()->m_OParte==(OParte)opCCL);
                 }
             } break;
case 130:
# line 1172 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer = (GetPtrWord()->m_OParte==(OParte)opCCT);
                 }
             } break;
case 131:
# line 1179 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer = (GetPtrWord()->m_OParte==(OParte)opCCM);
                 }
             } break;
case 132:
# line 1186 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer = (GetPtrWord()->m_OParte==(OParte)opSUJETO);
                 }
             } break;
case 133:
# line 1193 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer = (GetPtrWord()->m_OParte==(OParte)opVERBO);
                 }
             } break;
case 134:
# line 1200 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer = GetPtrSentence()->ExistLeft(_PtrNodo,yypvt[-1].string);
                 }
             } break;
case 135:
# line 1207 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer = GetPtrSentence()->ExistRight(_PtrNodo,yypvt[-1].string);
                 }
             } break;
case 136:
# line 1215 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer = GetPtrSentence()->ExistLeft(_PtrNodo,yypvt[-1].string,TRUE);
                 }
             } break;
case 137:
# line 1223 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer = GetPtrSentence()->ExistRight(_PtrNodo,yypvt[-1].string,TRUE);
                 }
             } break;
case 138:
# line 1231 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 int i = GetPtrWord()->m_Data.Find(yypvt[-1].string);
                 yyval.integer = (i != -1);
                 }
             } break;
case 139:
# line 1240 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer = (GetPtrWord()->m_Case == C_MIXTO)?1:0;
                 }
             } break;
case 140:
# line 1248 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer = (GetPtrWord()->m_Case == C_UPPER)?1:0;
                 }
             } break;
case 141:
# line 1255 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer = (GetPtrWord()->m_Case == C_LOWER)?1:0;
                 }
             } break;
case 142:
# line 1262 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 yyval.integer = (GetPtrWord()->m_EntreComillas == TRUE)?1:0;
                 }
            else
                yyval.integer = 0;
            } break;
case 143:
# line 1272 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {
                 //yyval.integer = (Translate->m_Acronimo == TRUE)?1:0;
                 }
             } break;
case 144:
# line 1280 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {                   
                 if(yypvt[-1].word != NULL)
                    yyval.integer = (yypvt[-1].word->m_Abreviatura == TRUE)?1:0;
                 else
                    yyval.integer = 0; 
                 }
             } break;
case 145:
# line 1291 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {                   
                 if(yypvt[-3].word != NULL && yypvt[-1].string.IsEmpty() == FALSE)
                    yyval.integer = (yypvt[-1].string.Find(yypvt[-3].word->m_sTipo) != -1)?1:0;
                 else
                    yyval.integer = 0; 
                 }
             } break;
case 146:
# line 1301 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {                   
                 if(yypvt[-3].word != NULL && yypvt[-1].string.IsEmpty() == FALSE)
                    yyval.integer = (yypvt[-1].string.Find(yypvt[-3].word->m_sTipo) != -1)?1:0;
                 else
                    yyval.integer = 0; 
                 }
             } break;
case 147:
# line 1311 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {                   
                 if(yypvt[-3].word != NULL && yypvt[-1].string.IsEmpty() == FALSE)
                    yyval.integer = (yypvt[-1].string.Find(yypvt[-3].word->m_sTipo) == -1)?1:0;
                 else
                    yyval.integer = 1; 
                 }
             } break;
case 148:
# line 1321 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {                   
                 if(yypvt[-3].word != NULL && yypvt[-1].string.IsEmpty() == FALSE)
                    yyval.integer = (yypvt[-1].string.Find(yypvt[-3].word->m_sTipo) == -1)?1:0;
                 else
                    yyval.integer = 1; 
                 }
             } break;
case 149:
# line 1331 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {                   
                 if(yypvt[-3].word != NULL && yypvt[-1].string.IsEmpty() == FALSE)
                    {                                       
                    CStringA coma(",");
                    CStringA wherefind = coma + yypvt[-1].string + coma;       // pone la cadena en que se busca entre comas
                    CStringA wfind = coma + yypvt[-3].word->m_Origlwr + coma;  // pone la palabra entre comas
                    
                    yyval.integer = (wherefind.Find(wfind) != -1)?1:0;
                    }
                 else
                    yyval.integer = 0; 
                 }
             } break;
case 150:
# line 1347 "LengDic.y"
{
             if(m_SyntaxChk == FALSE)
                 {                   
                 if(yypvt[-3].word != NULL && yypvt[-1].string.IsEmpty() == FALSE)
                    {                                       
                    CStringA coma(",");
                    CStringA wherefind = coma + yypvt[-1].string + coma;       // pone la cadena en que se busca entre comas
                    CStringA wfind = coma + yypvt[-3].word->m_Origlwr + coma;  // pone la palabra entre comas
                    
                    yyval.integer = (wherefind.Find(wfind) == -1)?1:0;
                    }
                 else
                    yyval.integer = 1; 
                 }
             } break;
case 151:
# line 1363 "LengDic.y"
{
           if(m_SyntaxChk == FALSE)
                {                        
                if(yypvt[-3].word == NULL)
                    yyval.integer = 0;    
                else
                     {    
                     ASSERT(GetPtrWord() !=NULL );
                     CStringA tmp1(yypvt[-1].string);
                     if(tmp1.GetLength() < yypvt[-3].word->m_Origlwr.GetLength())
                        {                                                    
                        CStringA tmp2 = yypvt[-3].word->m_Origlwr.Right(tmp1.GetLength());
                        if(tmp1 == tmp2) // sailer
                        //if(!strncmp(tmp1,tmp2,tmp1.GetLength()))
                           yyval.integer = 1;
                        else   
                           yyval.integer = 0;
                        }
                     else
                        yyval.integer = 0;
                     }   
                }
            } break;
case 152:
# line 1386 "LengDic.y"
{
	         if(m_SyntaxChk == FALSE)
                {                        
                if(yypvt[-3].word == NULL)
                    yyval.integer = 0;    
                else
                     {    
                     ASSERT(GetPtrWord() !=NULL );
                     CStringA tmp1(yypvt[-1].string);
                     if(tmp1.GetLength() < yypvt[-3].word->m_Key.GetLength())
                        {                                                    
                        CStringA tmp2 = yypvt[-3].word->m_Key.Right(tmp1.GetLength());
                        if (tmp1 == tmp2) // sailer
                        //if(!strncmp(tmp1,tmp2,tmp1.GetLength()))
                           yyval.integer = 1;
                        else   
                           yyval.integer = 0;
                        }
                     else
                        yyval.integer = 0;
                     }   
                }
            } break;
case 153:
# line 1409 "LengDic.y"
{
	         if(m_SyntaxChk == FALSE)
                {                        
                if(yypvt[-3].word == NULL)
                    yyval.integer = 0;    
                else
                     {    
                     ASSERT(GetPtrWord() !=NULL );
					 if(yypvt[-3].word != NULL && yypvt[-3].word->IsInMode((Modo)yypvt[-1].integer) == TRUE)
						yyval.integer = 1;
                     else
                        yyval.integer = 0;
                     }   
                }
            } break;
case 154:
# line 1425 "LengDic.y"
{
	         if(m_SyntaxChk == FALSE)
                {                        
                if(yypvt[-3].word == NULL)
                    yyval.integer = 0;    
                else
                     {    
                     ASSERT(GetPtrWord() !=NULL );
					 if(yypvt[-3].word != NULL && yypvt[-3].word->m_Tiempo ==((Tiempo)yypvt[-1].integer))
						yyval.integer = 1;
                     else
                        yyval.integer = 0;
                     }   
                }
            } break;
case 155:
# line 1455 "LengDic.y"
{yyval.integer =mINFINITIVO; } break;
case 156:
# line 1457 "LengDic.y"
{yyval.integer =mINDICATIVO; } break;
case 157:
# line 1459 "LengDic.y"
{yyval.integer =mIMPERATIVO; } break;
case 158:
# line 1461 "LengDic.y"
{yyval.integer =mPOTENCIAL; } break;
case 159:
# line 1463 "LengDic.y"
{yyval.integer =mSUBJUNTIVO; } break;
case 160:
# line 1465 "LengDic.y"
{yyval.integer =mGERUNDIO; } break;
case 161:
# line 1467 "LengDic.y"
{yyval.integer =mPARTICIPIO; } break;
case 162:
# line 1469 "LengDic.y"
{yyval.integer =mSUBJUNTIVO_IMPERATIVO; } break;
case 163:
# line 1471 "LengDic.y"
{yyval.integer =mSUBJUNTIVO_IMPERATIVO_INDICATIVO; } break;
case 164:
# line 1473 "LengDic.y"
{yyval.integer =mIMPERATIVO_PARTICIPIO_INDICATIVO; } break;
case 165:
# line 1475 "LengDic.y"
{yyval.integer =mSUBJUNTIVO_IMPERATIVO_INDICATIVO_PARTICIPIO; } break;
case 166:
# line 1477 "LengDic.y"
{yyval.integer =mPARTICIPIO_INDICATIVO; } break;
case 167:
# line 1479 "LengDic.y"
{yyval.integer =mIMPERATIVO_INDICATIVO; } break;
case 168:
# line 1481 "LengDic.y"
{yyval.integer =mIMPERATIVO_INFINITIVO; } break;
case 169:
# line 1483 "LengDic.y"
{yyval.integer =mSUBJUNTIVO_INDICATIVO; } break;
case 170:
# line 1485 "LengDic.y"
{yyval.integer =mPOTENCIAL_INDICATIVO; } break;
case 171:
# line 1487 "LengDic.y"
{yyval.integer =mSUBJUNTIVO_PARTICIPIO; } break;
case 172:
# line 1492 "LengDic.y"
{yyval.integer = PRIMERAP; } break;
case 173:
# line 1494 "LengDic.y"
{yyval.integer = SEGUNDAP; } break;
case 174:
# line 1496 "LengDic.y"
{yyval.integer = TERCERAP; } break;
case 175:
# line 1500 "LengDic.y"
{yyval.integer = /*GetPtrWord()->m_Numero.Get();*/ SINGULAR; } break;
case 176:
# line 1502 "LengDic.y"
{yyval.integer = PLURAL; } break;
case 177:
# line 1506 "LengDic.y"
{yyval.integer =opCD; } break;
case 178:
# line 1508 "LengDic.y"
{yyval.integer =opCI; } break;
case 179:
# line 1510 "LengDic.y"
{yyval.integer =opCCT; } break;
case 180:
# line 1512 "LengDic.y"
{yyval.integer =opCCM; } break;
case 181:
# line 1514 "LengDic.y"
{yyval.integer =opCCL; } break;
case 182:
# line 1516 "LengDic.y"
{yyval.integer =opSUJETO; } break;
case 183:
# line 1518 "LengDic.y"
{yyval.integer =opVERBO; } break;
case 184:
# line 1522 "LengDic.y"
{yyval.integer =PRESENTE; } break;
case 185:
# line 1524 "LengDic.y"
{yyval.integer =PASADO; } break;
case 186:
# line 1526 "LengDic.y"
{
						yyval.integer =FUTURO; 
						} break;
     }
		 
    goto yystack;  /* stack new state and value */
}

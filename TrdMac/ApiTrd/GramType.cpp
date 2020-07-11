#include "GramType.h"


_TGramType CGramType::invalidType = { 0, 0};

// lista de los tipos no definidos
CStrArray CGramType::_arrUndefTypes;

// lista de los tipos definidos (fillTypeArray sustituye las "," por un null terminator)
char CGramType::_arrTypes[] = 
				"AA,AC,AE,AF,AI,AL,AN,AO,AP,AQ,AS,AY,"
				"BD,BE,BF,BG,BH,BI,BJ,BK,BL,BM,BN,BO,BP,BR,BS,BU,BV,BX,BY,BZ,"
				"CC,CO,"
				"DA,DD,DF,DI,DN,DO,DQ,"
				"EI,"
				"FH,FI,FL,FM,FN,FO,FP,FS,FT,"
				"GA,GC,GD,GG,GI,GN,GO,GP,GR,GS,GT,GV,GW,GY,GZ,"
				"HA,HC,HD,HE,HF,HI,HJ,HS,HT,HV,"
				"IA,IF,"
				"JI,JJ,JK,JL,JT,"
				"KK,"
				"LK,"
				"ME,MG,MN,MO,MU,MV,"
				"NA,NI,NM,NN,NO,NP,NU,"
				"OA,OC,OD,OE,OF,OG,OH,OI,OL,OO,OP,OR,OS,OW,OX,"
				"PB,PI,PP,PT,PV,"
				"QA,QB,QC,QD,QE,QF,QG,QH,QI,QJ,QK,QL,QM,QN,QO,QQ,QR,QS,QT,QU,QZ,"
				"RD,RI,"
				"SE,SH,SP,SS,SV,"
				"TA,TH,TI,TO,TW,TY,"
				"UA,UB,UC,UD,UE,UF,UG,UH,UI,UJ,UK,UL,UM,UN,UO,UP,UR,UT,UV,UW,UX,UY,UZ,"
				"VA,VB,VC,VD,VE,VF,VG,VH,VI,VJ,VK,VL,VM,VN,VO,VP,VU,VQ,VR,VS,VT,VV,VW,VX,VZ,"
				"WA,WB,WC,WD,WI,WK,WL,WM,WN,WO,WP,WQ,WR,WS,WT,WV,WW,WX,WY,"
				"XA,XB,XC,XD,XE,XF,XG,XH,XI,XJ,XK,XL,XM,XO,XP,XQ,XX,XY,"
				"YA,YB,YC,YD,YE,YF,YG,YH,YI,YJ,YK,YL,YM,YN,YP,YQ,YR,YS,YT,YU,YV,YW,YX,YZ,"
				"ZB,ZF,ZG,ZH,ZI,ZJ,ZV,ZZ,";


_TGramType CGramType::arrType[NLETTERS * NLETTERS];

void CGramType::addType(int aIndex, LPCSTR aStrType)
	{
	if (aIndex >= MAX_GRAM_TYPES)
		{
		TRACE("Too many gram-types, increase MAX_GRAM_TYPES");
		return;
		}

	int index1 = -1;
	int index2 = -1;

	if (aStrType[0] != 0 && aStrType[1] != 0)
		{
		index1 = aStrType[0] - 'A';
		index2 = aStrType[1] - 'A';
		}

	if (index1 < 0 || index1 >= NLETTERS || 
		index2 < 0 || index2 >= NLETTERS || 
		aStrType[2] != 0)
		{ // tipo no valido
		LOG_ERROR("ERROR: Tipo no valido: %s", aStrType);
		ASSERT(FALSE);
		return;
		}

	_TGramType type;
	type.off = aIndex >> 3;            // >> 3   == / 8
	type.mask = 1 << (aIndex & 0x07);  // & 0x07 == % 8

	arrType[index1*NLETTERS + index2] = type;
	}

// Llenar el arreglo de tipos a partir de los tipos definidos
void CGramType::fillTypeArray()
	{
	// inicializar el arreglo de tipos en valores invalidos
	for(int i=0; i<NLETTERS * NLETTERS; i++)
		arrType[i] = invalidType;

	//return;
	LPSTR psType = _arrTypes;

	for(int i=0; *psType != 0; i++, psType += 3)
		{
		if (psType[2] == 0)
			{ // caso en que no hay una coma al final
			addType(i, psType);
			}
		else
			{
			ASSERT(psType[2] == ',');

			psType[2] = 0;
			addType(i, psType);
			}
		}
	}

void CGramType::clearTypes()
	{
	_arrUndefTypes.Close();
	}

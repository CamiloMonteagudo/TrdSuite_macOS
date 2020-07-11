#ifndef STRUTIL_H_
#define STRUTIL_H_

#include "CString.h"

#define CS2SZ(s) (s).c_str()


// Compara dos cadenas de manera rapida, solo debe ser usada cuando se puede asumir que str1 y str2
// estan alineadas a DWORD (aunque no es estrictamente requerido)
// y ambas cadenas tienen reservada una cantidad de memoria multiplo de 4 (no tiene que ver con
// la longitud real de las cadenas)
// aLen es la longitud de una de las cadenas
// retorna <0 si str1 es menor que str2
// retorna >0 si str1 es mayor que str2
// retorna 0 si str1 es igual a str2
// parece ser mas rapido que strcmp
inline int strFastCompare(LPCSTR str1, LPCSTR str2, int aLen)
	{
	DWORD dw1, dw2;
	for(int i=0; ; i += 4, aLen -= 4)
		{
		dw1 = *(DWORD*)(str1+i);
		dw2 = *(DWORD*)(str2+i);
		
		if (dw1 != dw2)
			break; // los primeros 4 bytes son distintos, ir a comparar byte a byte
			
		// los primeros 4 bytes son iguales (puede incluir el null terminator)
		
		if (aLen <= 3)
			// quedan 3 o menos caracteres, por tanto las cadenas son iguales pues el null terminator
			// tambien se comparo
			return 0;
		// si quedan 4 caracteres hay que seguir comparando porque a la otra cadena le pueden quedar
		// mas de 4 caracteres
		}
		
		// los primeros 4 bytes no son iguales, comparar byte a byte
	int ch1, ch2, diff; 

	ch1 = (BYTE)dw1;
	ch2 = (BYTE)dw2;
	if ((diff = ch1 - ch2) != 0) return diff;
	if (ch1 == 0) return 0;

	ch1 = (BYTE)(dw1 >> 8);
	ch2 = (BYTE)(dw2 >> 8);
	if ((diff = ch1 - ch2) != 0) return diff;
	if (ch1 == 0) return 0;

	ch1 = (BYTE)(dw1 >> 16);
	ch2 = (BYTE)(dw2 >> 16);
	if ((diff = ch1 - ch2) != 0) return diff;
	if (ch1 == 0) return 0;

	ch1 = (BYTE)(dw1 >> 24);
	ch2 = (BYTE)(dw2 >> 24);
	// este resultado debe ser != 0 ya que las cadenas en este punto tienen que ser distintas
	return ch1 - ch2;
	}

/*inline int strFastCompare(LPCSTR str1, LPCSTR str2)
	{
	int ch1, ch2, diff; 
	for(int i=0; ; str1 += 4, str2 += 4)
		{
		DWORD chdw1 = *(DWORD*)str1;
		DWORD chdw2 = *(DWORD*)str2;

		ch1 = chdw1 & 0xFF;
		ch2 = chdw2 & 0xFF;
		if ((diff = ch1 - ch2) != 0) return diff;
		if (ch1 == 0) return 0;

		ch1 = chdw1 & 0xFF00;
		ch2 = chdw2 & 0xFF00;
		if ((diff = ch1 - ch2) != 0) return diff;
		if (ch1 == 0) return 0;

		ch1 = chdw1 & 0xFF0000;
		ch2 = chdw2 & 0xFF0000;
		if ((diff = ch1 - ch2) != 0) return diff;
		if (ch1 == 0) return 0;

		ch1 = (BYTE)(chdw1 >> 24);
		ch2 = (BYTE)(chdw2 >> 24);
		if ((diff = ch1 - ch2) != 0) return diff;
		if (ch1 == 0) return 0;
		}
	}*/

inline int StrFindOneOf(LPCSTR pStr, LPCSTR sCharSet)
	{
	for(int i=0; true; i++)
		{
		char ch = pStr[i];
		if (ch == 0)
			break;

		for(int j=0; sCharSet[j] != 0; j++)
			{
			if (sCharSet[j] == ch)
				return i;
			}
		}

	return -1;
	}

// funciones de la stdlib

#ifndef __STDC_WANT_SECURE_LIB__

typedef int errno_t;

errno_t strncpy_s(char *strDest, size_t numberOfElements, const char *strSource, size_t count);
errno_t strcpy_s(char *strDest, size_t numberOfElements, const char *strSource);
#endif

int atoics(const CStringA& s);

#if !defined(_MSC_VER) && !defined(strnlen)
size_t strnlen(const char *str, size_t maxlen);
#endif

#endif /* STRUTIL_H_ */

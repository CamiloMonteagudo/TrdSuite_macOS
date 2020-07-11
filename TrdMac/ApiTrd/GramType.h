#pragma once

#include "WinUtil.h"

// Cantidad de letras que se pueden usar en los tipos gramaticales
#define NLETTERS ('Z' - 'A' + 1)

#define GRAMTYPE_OK        0
#define GRAMTYPE_INVALID   1
#define GRAMTYPE_UNDEFINED 2

struct _TGramType
	{
	BYTE mask;
	BYTE off;
	};

class CGramTypeSet;

class CGramType
	{
	friend class CGramTypeSet;
private:

	_TGramType m_Type;

	static _TGramType invalidType;
	static char _arrTypes[];
	static _TGramType arrType[NLETTERS * NLETTERS];
	static CStrArray _arrUndefTypes;

	static void addType(int aIndex, LPCSTR aStrType);

public:

	static void fillTypeArray();
	static CStrArray* getUndefTypes() { return &_arrUndefTypes; };
	static void clearTypes();

	CGramType(void):m_Type(invalidType)
		{
		}

	// aCheckLen indica si se debe chequear la longitud de aType (solo se hace en modo DEBUG)
	CGramType(LPCSTR aType, bool aCheckLen = true)
		{
		fromString(aType, aCheckLen);
		}

	~CGramType(void)
		{
		}

	inline LPCSTR toString() const
		{
		ASSERT(isValid());

		// calcular el indice que corresponde a este tipo
		int index = m_Type.off << 3; 

		for(BYTE mask = m_Type.mask; (mask & 1) == 0; mask >>= 1, index++);

		ASSERT(index >= 0 && index < NLETTERS*NLETTERS);

		// cada tipo ocupa dos caracteres mas el null terminator
		LPSTR psType = _arrTypes + (index*3);
		return psType;
		}

	inline bool isValid() const
		{
		return m_Type.mask != 0;
		}

	// aCheckLen indica si se debe chequear la longitud de aType (solo se hace en modo DEBUG)
	// devuelve GRAMTYPE_OK, GRAMTYPE_INVALID o GRAMTYPE_UNDEFINED
	inline int fromString(LPCSTR aType, bool aCheckLen = true)
		{
		int index1 = -1;
		int index2 = -1;

		if (aType[0] != 0 && aType[1] != 0)
			{
			index1 = aType[0] - 'A';
			index2 = aType[1] - 'A';
			}

		if (index1 < 0 || index1 >= NLETTERS || index2 < 0 || index2 >= NLETTERS)
			{
			m_Type = invalidType;
#ifdef ACTIVE_SAVE
			CStringA type(aType, 2);
			//LOG_ERROR("Tipo no valido: %s", type.c_str());
#endif
			return GRAMTYPE_INVALID;
			}

		ASSERT(index1 >= 0 && index1 < NLETTERS &&
			index2 >= 0 && index2 < NLETTERS && 
			(!aCheckLen || aType[2] == 0));

		m_Type = arrType[index1*NLETTERS + index2];

		if (!isValid())
			{
#ifdef ACTIVE_SAVE
			CStringA type(aType, 2);
			_arrUndefTypes.AddUnique(type);
#endif
			//LOG_ERROR("Tipo no definido: %s", type.c_str());
			//ASSERT(FALSE);
			return GRAMTYPE_UNDEFINED;
			}

		return GRAMTYPE_OK;
		}

	};

// Cantidad maxima de tipos gramaticales soportados (tiene que ser multiplo de 32)
#define MAX_GRAM_TYPES (9*32)

#define TYPE_SET_LEN (MAX_GRAM_TYPES / 8)

struct TTypeSet
	{
	union
		{
		BYTE  types[TYPE_SET_LEN];
		DWORD dwtypes[TYPE_SET_LEN / 4];
		};
	};

class CGramTypeSet
	{
	private:
	TTypeSet m_Types;

	public:

	CGramTypeSet()
		{
		Clear();
		}

	CGramTypeSet(const CGramTypeSet& aTypeSet)
		{
		Copy(aTypeSet);
		}

	CGramTypeSet(LPCSTR aTypes)
		{
		Copy(aTypes);
		}

	inline void Clear()
		{
		for(int i=0; i<TYPE_SET_LEN / 4; i++)
			m_Types.dwtypes[i] = 0;
		}

	inline void SetAll()
		{
		for(int i=0; i<TYPE_SET_LEN / 4; i++)
			m_Types.dwtypes[i] = 0xFFFFFFFF;
		}

	inline void Copy(const CGramTypeSet& aTypeSet)
		{
		for(int i=0; i<TYPE_SET_LEN / 4; i++)
			m_Types.dwtypes[i] = aTypeSet.m_Types.dwtypes[i];
		}

	inline void Copy(LPCSTR aTypes)
		{
		Clear();
		Add(aTypes);
		}

	// ojo: esta funcion es lenta, solo usar para debuguear
	CStringA toString() const
		{
		CStringA res;
		for(int i=0; i<MAX_GRAM_TYPES; i++)
			{
			CGramType type;
			type.m_Type.off = i / 8;
			type.m_Type.mask = 1 << (i % 8);

			if (ContainType(type))
				res += CStringA(type.toString()) + ',';
			}

		return res;
		}

	// suma una lista de tipos a esta (union)
	inline void Add(const CGramTypeSet& aTypeSet)
		{
		for(int i=0; i<TYPE_SET_LEN / 4; i++)
			m_Types.dwtypes[i] |= aTypeSet.m_Types.dwtypes[i];
		}

	// intersecta esta lista con otra lista de tipos
	inline void Intersect(const CGramTypeSet& aTypeSet)
		{
		for(int i=0; i<TYPE_SET_LEN / 4; i++)
			m_Types.dwtypes[i] &= aTypeSet.m_Types.dwtypes[i];
		}

	// elimina de esta lista los tipos que esten en otra lista
	inline void Remove(const CGramTypeSet& aTypeSet)
		{
		for(int i=0; i<TYPE_SET_LEN / 4; i++)
			m_Types.dwtypes[i] &= ~aTypeSet.m_Types.dwtypes[i];
		}

	// hace que esta lista tenga los tipos que no se repiten entre ella y otra lista
	// (es lo opuesto a la interseccion)
	inline void Difference(const CGramTypeSet& aTypeSet)
		{
		for(int i=0; i<TYPE_SET_LEN / 4; i++)
			m_Types.dwtypes[i] ^= aTypeSet.m_Types.dwtypes[i];
		}

	// indica si esta lista contiene a otra lista
	inline bool ContainSet(const CGramTypeSet& aTypeSet) const
		{
		for(int i=0; i<TYPE_SET_LEN / 4; i++)
			if ((m_Types.dwtypes[i] & aTypeSet.m_Types.dwtypes[i]) != aTypeSet.m_Types.dwtypes[i])
				return false;

		return true;
		}

	// indica si la lista tiene elementos en comun con otra
	inline bool HasCommonTypes(const CGramTypeSet& aTypeSet) const
		{
		DWORD dwOrSum = 0;
		for(int i=0; i<TYPE_SET_LEN / 4; i++)
			dwOrSum |= m_Types.dwtypes[i] & aTypeSet.m_Types.dwtypes[i];
		return (dwOrSum != 0);
		}

	// invierte los tipos que contiene esta lista
	inline void Invert()
		{
		for(int i=0; i<TYPE_SET_LEN / 4; i++)
			m_Types.dwtypes[i] = ~m_Types.dwtypes[i];
		}

	// indica si la lista esta vacia
	inline bool IsEmpty() const
		{
		DWORD dwOrSum = m_Types.dwtypes[0];
		for(int i=1; i<TYPE_SET_LEN / 4; i++)
			dwOrSum |= m_Types.dwtypes[i];
		return (dwOrSum == 0);
		}

	// indica si la lista tiene todos los tipos
	inline bool IsFull() const
		{
		DWORD dwAndSum = m_Types.dwtypes[0];
		for(int i=1; i<TYPE_SET_LEN / 4; i++)
			dwAndSum &= m_Types.dwtypes[i];
		return (dwAndSum == 0xFFFFFFFF);
		}

	// Adicionar una lista de tipos separados por coma (puede ser un solo tipo tambien)
	// aTypes puede o no tener una coma al final
	inline void Add(LPCSTR aTypes)
		{
		LPCSTR sTypes = aTypes;
		for(; *sTypes != 0; sTypes += 3)
			{
			//ASSERT(aTypes[2] == ',' || aTypes[2] == 0);

			if (sTypes[1] == 0 || (sTypes[2] != ',' && sTypes[2] != 0))
				{ // caso de cadena mal formada
#if defined(ACTIVE_SAVE) && defined(REPORT_ERRORS)
				CStringA s(sTypes, 2);
				LOG_ERROR("Tipo no valido (%s) en lista de tipos: %s", s.c_str(), aTypes);
				break;
#else
				// ignorarlo todo hasta la proxima coma
				for(; *sTypes != 0 && *sTypes != ','; sTypes++);
				if (*sTypes == 0) return;
				sTypes -= 2;
				continue;
#endif
				}

			CGramType type;
			int res = type.fromString(sTypes, false);
			if (res == GRAMTYPE_OK)
				AddType(type);
			else if (res == GRAMTYPE_INVALID)
				{
#if defined(ACTIVE_SAVE) && defined(REPORT_ERRORS)
				CStringA s(sTypes, 2);
				LOG_ERROR("Tipo no valido (%s) en lista de tipos: %s", s.c_str(), aTypes);
				break;
#else
				continue;
#endif
				}
			else if (res == GRAMTYPE_UNDEFINED)
				{
#if defined(ACTIVE_SAVE) && defined(REPORT_ERRORS)
				CStringA s(sTypes, 2);
				LOG_ERROR("Tipo no definido (%s) en lista de tipos: %s", s.c_str(), aTypes);
				break;
#else
				continue;
#endif
				}

			if (sTypes[2] == 0)
				break;  // caso en que no hay una coma al final
			}
		}

	// Adiciona un tipo a esta lista (funciona con tipos no validos)
	inline void AddType(const CGramType& aType)
		{
		m_Types.types[aType.m_Type.off] |= aType.m_Type.mask;
		}

	// Elimina un tipo de esta lista (funciona con tipos no validos)
	inline void RemoveType(const CGramType& aType)
		{
		m_Types.types[aType.m_Type.off] &= ~aType.m_Type.mask;
		}

	// Indica si esta lista contiene un tipo en particular (funciona con tipos no validos)
	inline bool ContainType(const CGramType& aType) const
		{
		return (m_Types.types[aType.m_Type.off] & aType.m_Type.mask) != 0;
		}
	};


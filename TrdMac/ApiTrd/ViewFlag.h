//////////////////////////////////////////////////////////////////////
// Define que parte de la traducción ver durante el 
// proceso de traducción
//////////////////////////////////////////////////////////////////////


#if !defined(_CVIEWFLAG_)
#define _CVIEWFLAG_

#if !defined(MERCADO)
#define VIEWFLAG_SIZE 20
extern const char *StringArrayStepInTranslation[];

class CViewFlag 
	{
	private:
		CWordArray m_ViewFlag;

public:
	void Mark(int iStep)
		{m_ViewFlag.Add(iStep);}

	void UnMark(int iStep)
		{
		for(int i = 0; i < m_ViewFlag.GetSize(); i++)
			if(m_ViewFlag.GetAt(i) == iStep)
				m_ViewFlag.RemoveAt(i);
		}
	bool ExistMark(int iStep)
		{
		for(int i = 0; i < m_ViewFlag.GetSize(); i++)
			{
			if(m_ViewFlag.GetAt(i) == iStep)
				return true;
			}
		return false;
		}
	void ClearArray()
		{m_ViewFlag.RemoveAll();}
	};
#endif
#endif

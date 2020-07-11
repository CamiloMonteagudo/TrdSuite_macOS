/////////////////////////////////////////////////////////////////////
// Clase para manipular los comodines de la forma "Cmdkey","CmdData","CmdData"
/////////////////////////////////////////////////////////////////////

class CComodinItem: public CMFCObject
	{
	private:

	CComodinItemKey  m_Definicion;   // m_FComodin
	CComodinItemData m_Determinacion;// m_TComodin
	CComodinItemData m_Traduccion;	 // m_CComodin

	POSITION m_Nodo;
	CWord *m_Word;
	COracion *m_Ora;

	public:

	CComodinItem(COracion * ora, POSITION nodo)
		{
		m_Nodo = NULL;
		m_Word = NULL;
		ASSERT(nodo != NULL);
		if(nodo != NULL)
			{
			m_Nodo = nodo;
		    CWord *m_Word = (CWord *)ora.GetAt(m_Nodo);
			}
		}
	void Execute();
	void Proccess(const char *s);
	};


/////////////////////////////////////////////////////////////////////

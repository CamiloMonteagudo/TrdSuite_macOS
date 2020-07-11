////////////////////////////////////////////////////////////////////////////
//   Class CModo                                                          //
//   enum  Modo                                                           //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         7/10/93                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//	Se define e implementa class y enum para el tratamiento del Modo de   // 
//  los verbos                                            				  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#if !defined(__MODO__)
#define __MODO__

//enum Mood{meINFINITIVO=mINFINITIVO,mINDICATIVO,mIMPERATIVO,mPOTENCIAL,
//		  mSUBJUNTIVO,mGERUNDIO,mPARTICIPIO,mSU};

class CModo
	{   
	protected:
	
	Modo m_Modo;
	
	public:
	
	CModo()                  // Inicia Modo con mINDICATIVO como implícito
		{
		m_Modo = mINFINITIVO;
		}
		
	CModo(Modo m)  
		{
	 	ASSERT( m >= mINFINITIVO && m <= mMODO_FINAL);
		m_Modo = m;
		}
		
	void Set(Modo g)         // Pone Modo
	 	{
	 	ASSERT( g >= mINFINITIVO && g <= mMODO_FINAL);
	 	m_Modo = g;
	 	}
	 	
	void Set(CModo  g)         // Pone Modo
	 	{
	 	m_Modo = g.m_Modo;
	 	}
                    
	CModo& operator=(Modo g)	
		{            
		Set(g);
		return *this;
		}
		
	CModo& operator=(CModo  g)	
		{            
		Set(g);
		return *this;
		}
                    
	Modo Get()               // Retorna el Modo
		{
	 	ASSERT( m_Modo >= mINFINITIVO && m_Modo <= mMODO_FINAL);
		return m_Modo;
		}

	BOOL operator == (Modo g)	
		{            
	 	ASSERT( m_Modo >= mINFINITIVO && m_Modo <= mMODO_FINAL);
		return (m_Modo == g)?TRUE:FALSE;
		}
		
	BOOL operator ==(CModo &g)	
		{            
		return (m_Modo == g.m_Modo)?TRUE:FALSE;
		}

	BOOL operator !=(Modo g)	
		{            
	 	ASSERT( m_Modo >= mINFINITIVO && m_Modo <= mMODO_FINAL);
		return (m_Modo != g)?TRUE:FALSE;
		}
		
	BOOL operator !=(CModo &g)	
		{            
		return (m_Modo != g.m_Modo)?TRUE:FALSE;
		}
	};
#endif 

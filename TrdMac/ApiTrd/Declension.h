////////////////////////////////////////////////////////////////////////////
//   Class CDeclination                                                    //
//   enum  Declination                                                     //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Su�rez         12/12/99                        //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//	Se define e implementa class y enum para el tratamiento de declinaci�n// 
//  Se refiere a Caso dativo, genitivo, vocativo, nominativo y acusativo. //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#if !defined(__DECLENSION__)
#define __DECLENSION__

//enum Declination{NODECLINE,NOMINATIVE,ACCUSATIVE,DATIVE,GENITIVE,VOCATIVE};

class CDeclination
	{   
	protected:
	
	Declination m_Declination;
	
	public:
	
	CDeclination()                  // Inicia Declination con C_OTHER como impl�cito
		{
		m_Declination = NODECLINE;
		}
		
	CDeclination(Declination c)        
		{
		m_Declination = c;
		}
		
	void Set(Declination g)         // Pone Declinaci�n
	 	{
	 	ASSERT( g >= NODECLINE && g <= VOCATIVE);
	 	m_Declination = g;
	 	}
	 	
	void Set(CDeclination g)         // Pone Declinaci�n
	 	{
	 	m_Declination = g.m_Declination;
	 	}
	 	
	CDeclination& operator=(Declination g)	
		{            
		Set(g);
		return *this;
		}
		
	CDeclination& operator=(CDeclination g)	
		{            
		Set(g);
		return *this;
		}

	Declination Get()               // Retorna el Declination
		{
	 	ASSERT( m_Declination >= NODECLINE && m_Declination <= VOCATIVE);
		return m_Declination;
		}

    bool operator==(Declination g)  
        {            
	 	ASSERT( m_Declination >= NODECLINE && m_Declination <= VOCATIVE);
        return (m_Declination == g);
        }
        
    bool operator!=(Declination g)  
        {            
	 	ASSERT( m_Declination >= NODECLINE && m_Declination <= VOCATIVE);
        return (m_Declination != g);
        }

    bool operator==(CDeclination g) 
        {            
        return (m_Declination == g.Get());
        }
        
    bool operator!=(CDeclination g) 
        {            
        return (m_Declination != g.Get());
        }

	};
#endif 

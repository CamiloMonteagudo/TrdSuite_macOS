////////////////////////////////////////////////////////////////////////////
//   Class CCaso                                                          //
//   enum  Caso                                                           //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         7/10/93                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//	Se define e implementa class y enum para el tratamiento del Caso de   // 
//  las palabras en la oración                                            //
//  Se refiere a Mayúsculas, Minúsculas o Mixto no a CASO dativo,         //
//  genitivo, etc.														  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#if !defined(__CASO__)
#define __CASO__

enum Caso{C_MIXTO,C_UPPER,C_LOWER,C_OTHER};

class CCaso
	{   
	protected:
	
	Caso m_Caso;
	
	public:
	
	CCaso()                  // Inicia Caso con C_OTHER como implícito
		{
		m_Caso = C_OTHER;
		}
		
	CCaso(Caso c)        
		{
		m_Caso = c;
		}
		
	void Set(Caso g)         // Pone Caso
	 	{
	 	ASSERT( g >= C_MIXTO && g <= C_OTHER);
	 	m_Caso = g;
	 	}
	 	
	void Set(CCaso g)         // Pone Caso
	 	{
	 	m_Caso = g.m_Caso;
	 	}
	 	
	CCaso& operator=(Caso g)	
		{            
		Set(g);
		return *this;
		}
		
	CCaso& operator=(CCaso g)	
		{            
		Set(g);
		return *this;
		}

	Caso Get()               // Retorna el Caso
		{
	 	ASSERT( m_Caso >= C_MIXTO && m_Caso <= C_OTHER);
		return m_Caso;
		}

    BOOL operator==(Caso g)  
        {            
	 	ASSERT( m_Caso >= C_MIXTO && m_Caso <= C_OTHER);
        return (m_Caso == g);
        }
        
    BOOL operator!=(Caso g)  
        {            
	 	ASSERT( m_Caso >= C_MIXTO && m_Caso <= C_OTHER);
        return (m_Caso != g);
        }

    BOOL operator==(CCaso g) 
        {            
        return (m_Caso == g.Get());
        }
        
    BOOL operator!=(CCaso g) 
        {            
        return (m_Caso != g.Get());
        }

	};
#else	
	#pragma message ("Fichero <<CCaso.h>> incluido en más de una ocasión")
#endif 

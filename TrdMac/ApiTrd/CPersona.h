////////////////////////////////////////////////////////////////////////////
//   Class CPersona                                                       //
//   enum  Persona                                                        //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         7/10/93                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//	Se define e implementa class y enum para el tratamiento de Persona de // 
//  las palabras en la oración                                            //
//                                                                        //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#if !defined(__PERSONA__)
#define __PERSONA__

//enum Persona{PRIMERAP,SEGUNDAP,TERCERAP};

class CPersona
	{   
	protected:
	Persona m_Per;
	
	public:
	CPersona()                  // Inicia Persona con TERCERAP como implícito
		{
		m_Per = TERCERAP;
		}
		
	CPersona(Persona g)     
		{
	 	ASSERT( g >= PRIMERAP && g <= TERCERAP);
		m_Per = g;
		}
		
	void Set(Persona g)         // Pone la Persona
	 	{
	 	ASSERT( g >= PRIMERAP && g <= TERCERAP);
	 	m_Per = g;
	 	}
	 	
	void Set(CPersona  g)         // Pone la Persona
	 	{
	 	m_Per = g.m_Per;
	 	}
	 	
	CPersona& operator=(Persona g)	
		{            
		Set(g);
		return *this;
		}
		
	CPersona& operator=(CPersona  g)	
		{            
		Set(g);
		return *this;
		}

	BOOL operator==(Persona g)	
		{            
	 	ASSERT( g >= PRIMERAP && g <= TERCERAP);
		return (m_Per == g);
		}
	BOOL operator==(CPersona g)	
		{            
		return (m_Per == g.m_Per);
		}
	BOOL operator!=(Persona g)	
		{            
	 	ASSERT( g >= PRIMERAP && g <= TERCERAP);
		return (m_Per != g);
		}
	BOOL operator!=(CPersona g)	
		{            
		return (m_Per != g.m_Per);
		}
	Persona Get()               // Retorna la Persona
		{
	 	ASSERT( m_Per >= PRIMERAP && m_Per <= TERCERAP);
		return m_Per;
		}
	};
#endif 

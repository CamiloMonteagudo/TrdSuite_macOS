////////////////////////////////////////////////////////////////////////////
//   Class CTiempo                                                        //
//   enum  Tiempo                                                         //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         7/10/93                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//	Se define e implementa class y enum para el tratamiento del Tiempo de // 
//  las palabras en la oración                                            //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#if !defined(__TIEMPO__)
#define __TIEMPO__

//enum Tiempo{PRESENTE,PASADO,FUTURO,PPASADO/* participio pasado*/};

class CTiempo
	{   
	protected:
	
	Tiempo m_Tiempo;
	
	public:
	
	CTiempo()                  // Inicia Tiempo con PRESENTE como implícito
		{
		m_Tiempo = PRESENTE;
		}
		
	CTiempo(Tiempo g)                 
		{
	 	ASSERT( g >= PRESENTE && g <= PASADOIMP);
		m_Tiempo = g;
		}
		
	void Set(Tiempo g)         // Pone Tiempo
	 	{
	 	ASSERT( g >= PRESENTE && g <= PASADOIMP);
	 	m_Tiempo = g;
	 	}
	 	
	void Set(CTiempo  g)         // Pone Tiempo
	 	{
	 	m_Tiempo = g.m_Tiempo;
	 	}
	 	
	CTiempo& operator=(Tiempo g)	
		{            
		Set(g);
		return *this;
		}
		
	CTiempo& operator=(CTiempo  g)	
		{            
		Set(g);
		return *this;
		}
	BOOL operator==(CTiempo  g)	
		{            
		return (m_Tiempo == g.m_Tiempo);
		}
	BOOL operator==(Tiempo  g)	
		{            
	 	ASSERT( g >= PRESENTE && g <= PASADOIMP);
		return (m_Tiempo == g);
		}
	BOOL operator!=(CTiempo  g)	
		{            
		return (m_Tiempo != g.m_Tiempo);
		}
	BOOL operator!=(Tiempo  g)	
		{            
	 	ASSERT( g >= PRESENTE && g <= PASADOIMP);
		return (m_Tiempo != g);
		}

	Tiempo Get()               // Retorna el Tiempo
		{
	 	ASSERT( m_Tiempo >= PRESENTE && m_Tiempo <= PASADOIMP);
		return m_Tiempo;
		}
	};
#endif 

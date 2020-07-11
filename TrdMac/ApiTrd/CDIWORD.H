////////////////////////////////////////////////////////////////////////////
//   Class CDiWord                                                        //
//   enum  DiWord                                                         //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         7/10/93                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//  Se define e implementa class y enum para el tratamiento del tipo de   // 
//  palabras que reconoce el parsing de palabras                          //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#if !defined(__DiWord__)
#define __DiWord__

enum DiWord
        {
        DW_BLANCOS,
        DW_DOSPUNTOS,
        DW_PUNTOYCOMA,
        DW_PUNTOSUSPENSIVOS,
        DW_PUNTO,
        DW_COMA,
        DW_FININTERROGA,
        DW_FINADMIRACION,
        DW_ORDINAL,               // DW_ORDINAL_ST
        DW_ORDINAL_ND,            // se pueden borrar
        DW_ORDINAL_RD,            // se pueden borrar
        DW_ORDINAL_TH,            // se pueden borrar
        DW_DECADA_CS,
        DW_DECADA_S,
        DW_NUMERAL,
        DW_DINERO,
        DW_LETRAGRIEGA,
        DW_ABREVIATURA3,
        DW_FILENAME,
        DW_ABREVIATURA2,
        DW_ABREVIATURA,
        DW_POSESIVO_CS,
        DW_POSESIVO_SC,
        DW_WORDENTRECOMILLAS,
        DW_WORD,
        DW_MENOS,
        DW_RAYA,
        DW_AMPERSAN,
        DW_LLAVEA,
        DW_LLAVEC,
        DW_CORCHETEA,
        DW_CORCHETEC,
        DW_MENORQ,
        DW_MAYORQ,
        DW_PARENTA,
        DW_PARENTC,
        DW_COMILLAD,
        DW_COMILLAS,
        DW_BARRA,
        DW_NOTRADUCIR,
        DW_PORCIENTO,
        DW_NUMERO,
        DW_CUALQUIERA,
        DW_NOMBRE_PROPIO,
		DW_ENTREPARENTESIS,	//44
		DW_ENTRECORCHETES,	
		DW_ENTRECOMILLAS,
		DW_ENTRECOMILLASS,
        DW_MARCADOR,                   // sólo para el diccionario
		DW_RTF_COMMAND,
		DW_FRASE_GUION,
        DW_FIN_DIV_WORD
        };

class CDiWord
    {   
    protected:
    
    DiWord m_DiWord;
    
    public:
    
    CDiWord()                  // Inicia Caso con C_MIXTO como implícito
        {
        m_DiWord = DW_BLANCOS;
        }
        
    CDiWord(DiWord d)       
        {
        m_DiWord = d;
        }
        
    void Set(DiWord g)         // Pone Caso
        {
        ASSERT( g >= DW_BLANCOS && g <= DW_FIN_DIV_WORD);
        m_DiWord = g;
        }
        
    void Set(CDiWord g)         // Pone Caso
        {
        m_DiWord = g.m_DiWord;
        }
        
    CDiWord& operator=(DiWord g)    
        {            
        Set(g);
        return *this;
        }
        
    CDiWord& operator=(CDiWord g)   
        {            
        Set(g);
        return *this;
        }

    DiWord Get()               // Retorna el Caso
        {
        ASSERT( m_DiWord >= DW_BLANCOS && m_DiWord <= DW_FIN_DIV_WORD);
        return m_DiWord;
        }

    };
#endif 

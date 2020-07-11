////////////////////////////////////////////////////////////////////////////
//   Class CDicType                                                       //
//   enum  DicType                                                        //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         7/10/93                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//  Se define e implementa class y enum para el tratamiento del tipo de   // 
//  la palabras en según el diccionario                                   //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#if !defined(__DicType__)
#define __DicType__


///////////////////////////////////////////////////////////////////////////
//    MUY IMPORTANTE                                                     //
//                                                                       //
//  Para agregar una palabra a la lista de palabras claves es necesario  //
//  agregarla también en el file tokenfun.h y en CWord.cpp en la lista   //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

enum DicType
    {     
    dtERROR = -1,                             /*ER*/
    dtNULL = 0,                               /*EN*/
    IT = 257,
    YOU,
    THOU,
    I,
    HE,
    SHE,
    WE,
    THEY,
    MYSELF,
    YOURSELF,
    HIMSELF,
    HERSELF,
    ITSELF,
    OURSELVES,
    YOURSELVES,
    THEMSELVES,
    MY,
    HIS,
    HERS,
    OUR,
    OURS,
    THEIR,
    THEIRS,
    YOUR,
    YOURS,
    MINE,
    ITS,
    ME,
    THEE,
    HIM,
    HER,
    US,
    THEM,
    STREET,
    LATER,
    PENNY,
    NICKEL,
    DIME,
    CENTS,
    DOLLAR,
    COINS,
    _THIS_,		//K THIS error
    THAT,
    THESE,
    THOSE,
    EVERY,
    HALF,
    QUARTER,
    ABOUT,
    AFTER,
    AGAINST,
    AROUND,
    AS,
    AT,
    BECAUSE,
    BEFORE,
    BEHIND,
    BETWEEN,
    BY,
    DURING,
    EXCEPT,
    FOR,
    FROM,
    HERETO,
    _IN_,	  //K IN error
    INTO,
    _NEAR_,
    OF,
    ON,
    OVER,
    PER,
    SINCE,
    dtPC_TO,  // palabra clave to es una de las que queda ya que marca infinitivo
    THROUGH,
    UNDER,
    UNTIL,
    WITH,
    WITHIN,
    WITHOUT,
            //  Adverbios de frecuencia    
    ALWAYS,
    USUALLY,
    OFTEN,
    SELDOM,
    SOMETIMES,
    RARELY,
    FREQUENTLY,
    OCCASIONALLY,
    NEVER,
    EVER,                              
           
    AND,
    OR,
    BUT,
    AM,
    ARE,
    BE,
    BEEN,
    BEING,
    IS,
    WAS,
    WAST,
    WERE,
    WILL,
    SHALL,
    HAS,
    HAVE,
    HAD,
    HAVING,
    DO,
    DOES,
    DID,
    DOING,
    HERE,
    THERE,
    NOT,
    NO,
    NONE,
    YES,
    WHO,
    WHOM,
    WHOSE,
    WHAT,
    WHERE,
    WHEN,
    CLOCK,
    FALL,
    PAST,
    PM,  
    VERY,
    NEXT,
    LAST,
    MAY,
    CAN, 
    MIGHT,
    COULD,
    SHOULD,
    WOULD,
    MUST,
    OUGHT,
    dtAADJETIVO,           /*ADJETIVOS*/      /*AA*/
    dtAABUNDANCIA,                            /*AB*/
    dtACALIFICATIVO,                          /*AC*/
    dtADETERMINATIVO,                         /*AD*/
    dtADEMOSTRATIVO,                          /*AE*/
    dtAINDEFINIDO,                            /*AF*/
    dtAGENTILICIO,                            /*AG*/
    dtACUANTITATIVO,                          /*AH*/
    dtAINAMOVIBLE,                            /*AI*/
    dtADECANTIDAD,                            /*AK*/
    dtACOMPARATIVO,                           /*AM*/
    dtANUMERAL,                               /*AN*/
    dtADEPOSICION,                            /*AO*/
    dtAPOSESIVO,                              /*AP*/
    dtAINTERROGATIVO,                         /*AR*/
    dtADISTRIBUTIVO,                          /*AS*/
    dtAATRIBUTIVO,                            /*AT*/
    dtAPREDICATIVO,                           /*AV*/
    dtAADVERBIAL,                             /*AY*/
    dtANOCOMPARATIVO,    /*ADJETIVOS*/        /*AZ*/
    
    dtDAFIRMACION,        /* ADVERBIOS */     /*DA*/
    dtDPROBABILIDAD,                          /*DB*/
    dtDCANTIDAD,                              /*DC*/
    dtDADVERBIO,                              /*DD*/
    dtDFRECUENCIA,                            /*DF*/
    dtDGRADO,                                 /*DG*/
    dtDDIMINUTIVO,                            /*DI*/
    dtDCONCESION,                             /*DK*/
    dtDLUGAR,                                 /*DL*/
    dtDMODO,                                  /*DM*/
    dtDNEGACION,                              /*DN*/
    dtDORDEN,                                 /*DO*/
    dtDCOMPARACION,                           /*DP*/
    dtDINTERROGACION,                         /*DR*/
    dtDSUPERLATIVO,                           /*DS*/ 
    dtDTIEMPO,                                /*DT*/
    dtDDUDA,                                  /*DU*/
    dtDRESULTADO,                             /*DV*/
    dtDRAZONOCAUSA,                           /*DW*/
    dtDCONDICIONOSUPOSICION,                  /*DX*/
    dtDFINALIDAD,                             /*DY*/
    dtDNOCOMPARATIVO,    /* ADVERBIOS */      /*DZ*/
         
    dtSUSTANTIVO,          /* SUSTANTIVOS */  /*SS*/
    dtPARTEANIO,                              /*SA*/
    dtCONTABLE,                               /*SB*/
    dtPARTEDIA,                               /*SD*/
    dtPARTESEMANA,                            /*SE*/
    dtHORADELDIA,                             /*SH*/
    dtMES,                                    /*SM*/
    dtDIASEMANA,                              /*SN*/
    dtNOMBREPROPIO,                           /*NP*/
    dtSUSTANTIVOTIEMPO,                       /*ST*/ /*Day,year,moth,week*/
    dtNOMBRETIEMPO,                           /*SU*/ 
    dtPALABRAEXTRANGERA,                      /*SW*/ 
    dtUNIDADDEMEDICION,                       /*SX*/ 
    dtESTACIONES,                             /*SY*/
    dtDIADELANIO,                             /*SZ*/
         
                 /* DEBEN SER BORRADOS  */
    dtSUSTANTIVOOBJETO,                       /*SO*/
    dtNOMBRECIUDAD,                           /*NC*/
    dtNOMBRECONTINENTE,                       /*NO*/
    dtTITULO,                                 /*ST*/
    dtPROFESION,                              /*SP*/
                /* Hasta aquí */
                        
	/*HA*/  //verbo haver auxiliar
    dtVTRANINTRAN,         /* VERBOS */       /*VV*/
    dtVAUXILIAR,                              /*VA*/
    dtVPPASADO,   // verbo pasado o ppasado   /*VC*/
    dtVPASADO,                                /*VD*/
    dtVRECIPROCO,                             /*VE*/
    dtVINFINITIVO,                            /*VF*/
    dtVGERUNDIO,                              /*VG*/
    dtVINTRANSITIVO,                          /*VI*/
    dtVIMPERSONAL,                            /*VM*/
    dtVPARTICIPIO,                            /*VP*/
    dtVPARTICIPIOA,                           /*VW*/
    dtVREFLEXIVO,                             /*VR*/
    dtVTERCERAPERSONA,                        /*VS*/
    dtVTRANSITIVO,                            /*VT*/
    dtVGERUNDIOREFLEXIVO,                     /*RG*/
    dtVTRASITIVOREFLEXIVO,                    /*TR*/
    dtVINTRASITIVOREFLEXIVO,                  /*IR*/
                          /* VERBOS */
    
    dtPREPOSICION,         /* OTROS  */       /*PP*/
    dtCONJUNCION,                             /*CC*/
    dtINTERJECCION,                           /*JJ*/
    dtARTICULOI,                              /*RI*/
    dtARTICULOD,                              /*RD*/
         
    dtPALABRACLAVE,                           /*PC*/
                  /* Pronombres */
    dtPRONOMBRE,                              /*OO*/
    dtPRONOMBRECD,                            /*OC*/   /* pronombre CDirecto*/
    dtPRONOMBREDEMOSTRATIVO,                  /*OD*/
    dtPRONOMBREENFATICO,                      /*OE*/
    dtPC_OF,                                  /*OF*/
    dtPRONOMBREINDEFINIDO,                    /*OG*/
    dtPRONOMBREINTERROGATIVO,                 /*OI*/
    dtPRONOMBREPERSONAL,                      /*OP*/
    dtPRONOMBREREFLEXIVO,                     /*OR*/
    dtPRONOMBREPOSESIVO,                      /*PS*/
                  /* Pronombres */
                  /* Numerales  */
    dtNUMERO,                                 /*NN*/
    dtCARDINAL,                               /*NC*/
    dtORDINAL,                                /*NO*/
    dtORDINALN,                               /*NU*/
         
                 /* combinados*/
    dtVTISUSTADJADV,                          /*XA*/ // S o V O ADJ O ADV
    dtVTISUSTADJ,                             /*XB*/
    dtVTISUSTADV,                             /*XC*/
    dtVTIADJADV,                              /*XD*/
    dtVTIADJ,                                 /*XE*/
    dtVTIADV,                                 /*XF*/
    dtVTISUST,                                /*XG*/
    
    dtSUSTADJADV,                             /*XH*/
    dtSUSTADJ,                                /*XJ*/
    dtSUSTADV,                                /*XK*/
    
    dtADJADV,                                 /*XM*/
    dtADJIADV,                                /*XY*/    // Adj inmovil o adv
    
    dtVSSUSTADJ,                              /*XN*/ // S o V 3ra Per o ADJ
    dtVSSUST,                                 /*XP*/
    dtVSADJ,                                  /*XQ*/
    
    dtVGSUSTADJ,                              /*XR*/ // S o G  o ADJ
    dtVGSUST,                                 /*XS*/
    dtVGADJ,                                  /*XT*/
    dtVGPACTIVO,                              /*XW*/  // Gerundio o participio activo
                                              /*XX*/  // pasado o ppasado o sust 
                                                    // para verbos reflexivos
    dtVRSUSTADJADV,                           /*YA*/ // S o V O ADJ O ADV
    dtVRSUSTADJ,                              /*YB*/
    dtVRSUSTADV,                              /*YC*/
    dtVRADJADV,                               /*YD*/
    dtVRADJ,                                  /*YE*/
    dtVRADV,                                  /*YF*/
    dtVRSUST,                                 /*YG*/

                           /* sólo para el parsing */
    dtRAYA,                                  /*GR*/
    dtGUION,                                 /*GG*/
    dtCOMILLAS,                              /*GC*/
    dtCOMILLAD,                              /*GD*/
    dtPARENTA,                               /*GP*/
    dtPARENTC,                               /*GP*/
    dtPORCIENTO,                             /*GO*/
    dtROMANO,                                /*GM*/
    dtADMIRACION,                            /*GA*/
    dtINTERROGACION,                         /*GI*/
    dtMENORQ,                                /*GP*/
    dtMAYORQ,                                /*GP*/
    dtDINERO,                                /*GN*/
    dtDOSPUNTOS,                             /*GT*/
    dtOTROSCHAR,                             /*GT*/
    dtPOSESIVO,                              /*GX*/
    dtDECADA,                                /*GY*/
    dtFILENAME,                              /*GF*/
    dtCOMA,                                  /*GZ*/
    dtPOSESIVONP,                            /*GV*/
    dtPOSESIVOSUST,                          /*GW*/
    dtPOSESIVOTIEMPO,                        /*GB*/
         
    dtFINDETIPO             // último tipo no mover es sólo una marca
    };          /* las palabras claves empezaran en 600 */

class CDicType
    {   
    protected:
    
    DicType m_DicType;
    
    public:
    
    CDicType()         // Inicia DicType con dtNOMBREPERSONA como implícito
        {
        m_DicType = (DicType)0; //K no encuentra este enum dtNOMBREPROPIO;
        }
        
    CDicType(DicType c)        
        {
        m_DicType = c;
//K error        ASSERT( m_DicType >= dtERROR && m_DicType < dtFINDETIPO);
        }
        
    void Set(DicType g)         // Pone DicType
        {
        m_DicType = g;
        ASSERT( m_DicType >= dtERROR && m_DicType < dtFINDETIPO);
        }
        
    void Set(CDicType g)         // Pone DicType
        {
        m_DicType = g.m_DicType;
        }
        
    CDicType& operator=(DicType g)  
        {            
        Set(g);
        return *this;
        }
        
        
    CDicType& operator=(CDicType g) 
        {            
        Set(g);
        return *this;
        }

    DicType Get()               // Retorna el DicType
        {
        ASSERT( m_DicType >= dtERROR && m_DicType < dtFINDETIPO);
        return m_DicType;
        }
        
    BOOL operator==(DicType g)  
        {            
        ASSERT( g >= dtERROR && g < dtFINDETIPO);
        return (m_DicType == g);
        }
        
    BOOL operator!=(DicType g)  
        {            
        ASSERT( g >= dtERROR && g < dtFINDETIPO);
        return (m_DicType != g);
        }
    BOOL operator==(CDicType g) 
        {            
        return (m_DicType == g.Get());
        }
        
    BOOL operator!=(CDicType g) 
        {            
        return (m_DicType != g.Get());
        }
    };
#else   
    #pragma message ("Fichero <<CDicType.h>> incluido en más de una ocasión")
#endif 

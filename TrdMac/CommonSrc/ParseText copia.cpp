
#include <string.h> 
#include "stdafx.h"
#include "parsetext.h"


static BYTE Keys[] =  {//   Ini,End,Num,Up,Low
                      PACK( 0 , 1 , 0 , 0, 0 ),// 0  NUL
                      PACK( 0 , 1 , 0 , 0, 0 ),// 1  SOH
                      PACK( 0 , 1 , 0 , 0, 0 ),// 2  STX
                      PACK( 0 , 1 , 0 , 0, 0 ),// 3  ETX
                      PACK( 0 , 1 , 0 , 0, 0 ),// 4  EOT
                      PACK( 0 , 1 , 0 , 0, 0 ),// 5  ENQ
                      PACK( 0 , 1 , 0 , 0, 0 ),// 6  ACK
                      PACK( 0 , 1 , 0 , 0, 0 ),// 7  BEL
                      PACK( 0 , 1 , 0 , 0, 0 ),// 8  BS
                      PACK( 0 , 1 , 0 , 0, 0 ),// 9  HT
                      PACK( 0 , 1 , 0 , 0, 0 ),// 10 LF
                      PACK( 0 , 1 , 0 , 0, 0 ),// 11 VT
                      PACK( 0 , 1 , 0 , 0, 0 ),// 12 FF
                      PACK( 0 , 1 , 0 , 0, 0 ),// 13 CR
                      PACK( 0 , 1 , 0 , 0, 0 ),// 14 SO
                      PACK( 0 , 1 , 0 , 0, 0 ),// 15 SI
                      PACK( 0 , 1 , 0 , 0, 0 ),// 16 SLE
                      PACK( 0 , 1 , 0 , 0, 0 ),// 17 CS1
                      PACK( 0 , 1 , 0 , 0, 0 ),// 18 DC2
                      PACK( 0 , 1 , 0 , 0, 0 ),// 19 DC3
                      PACK( 0 , 1 , 0 , 0, 0 ),// 20 DC4
                      PACK( 0 , 1 , 0 , 0, 0 ),// 21 NAK
                      PACK( 0 , 1 , 0 , 0, 0 ),// 22 SYN
                      PACK( 0 , 1 , 0 , 0, 0 ),// 23 ETB
                      PACK( 0 , 1 , 0 , 0, 0 ),// 24 CAN
                      PACK( 0 , 1 , 0 , 0, 0 ),// 25 EM
                      PACK( 0 , 1 , 0 , 0, 0 ),// 26 SIB
                      PACK( 0 , 1 , 0 , 0, 0 ),// 27 ESC
                      PACK( 0 , 1 , 0 , 0, 0 ),// 28 FS
                      PACK( 0 , 1 , 0 , 0, 0 ),// 29 GS
                      PACK( 0 , 1 , 0 , 0, 0 ),// 30 RS
                      PACK( 0 , 1 , 0 , 0, 0 ),// 31 US
                      PACK( 0 , 0 , 0 , 0, 0 ),// 32 (space)
                      PACK( 1 , 1 , 0 , 0, 0 ),// 33 !
                      PACK( 1 , 1 , 0 , 0, 0 ),// 34 "
                      PACK( 0 , 0 , 0 , 0, 0 ),// 35 #
                      PACK( 0 , 0 , 0 , 0, 0 ),// 36 $
                      PACK( 0 , 0 , 0 , 0, 0 ),// 37 %
                      PACK( 0 , 0 , 0 , 0, 0 ),// 38 &
                      PACK( 1 , 0 , 0 , 0, 0 ),// 39 '
                      PACK( 1 , 0 , 0 , 0, 0 ),// 40 (
                      PACK( 0 , 0 , 0 , 0, 0 ),// 41 )
                      PACK( 0 , 0 , 0 , 0, 0 ),// 42 *
                      PACK( 0 , 0 , 0 , 0, 0 ),// 43 +
                      PACK( 0 , 0 , 0 , 0, 0 ),// 44 ,
                      PACK( 0 , 0 , 0 , 0, 0 ),// 45 -
                      PACK( 0 , 1 , 0 , 0, 0 ),// 46 .
                      PACK( 0 , 0 , 0 , 0, 0 ),// 47 /
                      PACK( 1 , 0 , 1 , 0, 0 ),// 48 0
                      PACK( 1 , 0 , 1 , 0, 0 ),// 49 1
                      PACK( 1 , 0 , 1 , 0, 0 ),// 50 2
                      PACK( 1 , 0 , 1 , 0, 0 ),// 51 3
                      PACK( 1 , 0 , 1 , 0, 0 ),// 52 4
                      PACK( 1 , 0 , 1 , 0, 0 ),// 53 5
                      PACK( 1 , 0 , 1 , 0, 0 ),// 54 6
                      PACK( 1 , 0 , 1 , 0, 0 ),// 55 7
                      PACK( 1 , 0 , 1 , 0, 0 ),// 56 8
                      PACK( 1 , 0 , 1 , 0, 0 ),// 57 9
                      PACK( 0 , 1 , 0 , 0, 0 ),// 58 :
                      PACK( 0 , 1 , 0 , 0, 0 ),// 59 ;
                      PACK( 0 , 0 , 0 , 0, 0 ),// 60 <
                      PACK( 0 , 0 , 0 , 0, 0 ),// 61 =
                      PACK( 0 , 0 , 0 , 0, 0 ),// 62 >
                      PACK( 0 , 1 , 0 , 0, 0 ),// 63 ?
                      PACK( 0 , 0 , 0 , 0, 0 ),// 64 @
                      PACK( 1 , 0 , 0 , 1, 0 ),// 65 A
                      PACK( 1 , 0 , 0 , 1, 0 ),// 66 B
                      PACK( 1 , 0 , 0 , 1, 0 ),// 67 C
                      PACK( 1 , 0 , 0 , 1, 0 ),// 68 D
                      PACK( 1 , 0 , 0 , 1, 0 ),// 69 E
                      PACK( 1 , 0 , 0 , 1, 0 ),// 70 F
                      PACK( 1 , 0 , 0 , 1, 0 ),// 71 G
                      PACK( 1 , 0 , 0 , 1, 0 ),// 72 H
                      PACK( 1 , 0 , 0 , 1, 0 ),// 73 I
                      PACK( 1 , 0 , 0 , 1, 0 ),// 74 J
                      PACK( 1 , 0 , 0 , 1, 0 ),// 75 K
                      PACK( 1 , 0 , 0 , 1, 0 ),// 76 L
                      PACK( 1 , 0 , 0 , 1, 0 ),// 77 M
                      PACK( 1 , 0 , 0 , 1, 0 ),// 78 N
                      PACK( 1 , 0 , 0 , 1, 0 ),// 79 O
                      PACK( 1 , 0 , 0 , 1, 0 ),// 80 P
                      PACK( 1 , 0 , 0 , 1, 0 ),// 81 Q
                      PACK( 1 , 0 , 0 , 1, 0 ),// 82 R
                      PACK( 1 , 0 , 0 , 1, 0 ),// 83 S
                      PACK( 1 , 0 , 0 , 1, 0 ),// 84 T
                      PACK( 1 , 0 , 0 , 1, 0 ),// 85 U
                      PACK( 1 , 0 , 0 , 1, 0 ),// 86 V
                      PACK( 1 , 0 , 0 , 1, 0 ),// 87 W
                      PACK( 1 , 0 , 0 , 1, 0 ),// 88 X
                      PACK( 1 , 0 , 0 , 1, 0 ),// 89 Y
                      PACK( 1 , 0 , 0 , 1, 0 ),// 90 Z
                      PACK( 0 , 1 , 0 , 0, 0 ),// 91 [
                      PACK( 0 , 1 , 0 , 0, 0 ),// 92 backslat
                      PACK( 0 , 1 , 0 , 0, 0 ),// 93 ]
                      PACK( 0 , 1 , 0 , 0, 0 ),// 94 ^
                      PACK( 0 , 0 , 0 , 0, 0 ),// 95 _
                      PACK( 0 , 0 , 0 , 0, 0 ),// 96 `
                      PACK( 1 , 0 , 0 , 0, 1 ),// 97 a
                      PACK( 1 , 0 , 0 , 0, 1 ),// 98 b
                      PACK( 1 , 0 , 0 , 0, 1 ),// 99 c
                      PACK( 1 , 0 , 0 , 0, 1 ),// 100 d
                      PACK( 1 , 0 , 0 , 0, 1 ),// 101 e
                      PACK( 1 , 0 , 0 , 0, 1 ),// 102 f
                      PACK( 1 , 0 , 0 , 0, 1 ),// 103 g
                      PACK( 1 , 0 , 0 , 0, 1 ),// 104 h
                      PACK( 1 , 0 , 0 , 0, 1 ),// 105 i
                      PACK( 1 , 0 , 0 , 0, 1 ),// 106 j
                      PACK( 1 , 0 , 0 , 0, 1 ),// 107 k
                      PACK( 1 , 0 , 0 , 0, 1 ),// 108 l
                      PACK( 1 , 0 , 0 , 0, 1 ),// 109 m
                      PACK( 1 , 0 , 0 , 0, 1 ),// 110 n
                      PACK( 1 , 0 , 0 , 0, 1 ),// 111 o
                      PACK( 1 , 0 , 0 , 0, 1 ),// 112 p
                      PACK( 1 , 0 , 0 , 0, 1 ),// 113 q
                      PACK( 1 , 0 , 0 , 0, 1 ),// 114 r
                      PACK( 1 , 0 , 0 , 0, 1 ),// 115 s
                      PACK( 1 , 0 , 0 , 0, 1 ),// 116 t
                      PACK( 1 , 0 , 0 , 0, 1 ),// 117 u
                      PACK( 1 , 0 , 0 , 0, 1 ),// 118 v
                      PACK( 1 , 0 , 0 , 0, 1 ),// 119 w
                      PACK( 1 , 0 , 0 , 0, 1 ),// 120 x
                      PACK( 1 , 0 , 0 , 0, 1 ),// 121 y
                      PACK( 1 , 0 , 0 , 0, 1 ),// 122 z
                      PACK( 0 , 1 , 0 , 0, 0 ),// 123 {
                      PACK( 0 , 1 , 0 , 0, 0 ),// 124 |
                      PACK( 0 , 1 , 0 , 0, 0 ),// 125 }
                      PACK( 0 , 1 , 0 , 0, 0 ),// 126 ~
                      PACK( 0 , 0 , 0 , 0, 0 ),// 127
                      PACK( 1 , 0 , 0 , 1, 0 ),// 128 
                      PACK( 0 , 0 , 0 , 0, 0 ),// 129
                      PACK( 0 , 1 , 0 , 0, 0 ),// 130 
                      PACK( 0 , 0 , 0 , 0, 0 ),// 131 
                      PACK( 0 , 1 , 0 , 0, 0 ),// 132 
                      PACK( 0 , 1 , 0 , 0, 0 ),// 133 

                      PACK( 0 , 1 , 0 , 0, 0 ),// 134 
                      PACK( 0 , 1 , 0 , 0, 0 ),// 135 
                      PACK( 0 , 0 , 0 , 0, 0 ),// 136
                      PACK( 0 , 1 , 0 , 0, 0 ),// 137 
                      PACK( 1 , 0 , 0 , 1, 0 ),// 138 
                      PACK( 1 , 0 , 0 , 0, 0 ),// 139 
                      PACK( 1 , 0 , 0 , 1, 0 ),// 140 S
                      PACK( 1 , 0 , 0 , 1, 0 ),// 141 T
                      PACK( 1 , 0 , 0 , 1, 0 ),// 142 
                      PACK( 1 , 0 , 0 , 1, 0 ),// 143 Z
                      PACK( 0 , 0 , 0 , 0, 0 ),// 144 
                      PACK( 0 , 0 , 0 , 0, 0 ),// 145 
                      PACK( 0 , 0 , 0 , 0, 0 ),// 146 
                      PACK( 0 , 0 , 0 , 0, 0 ),// 147 
                      PACK( 0 , 1 , 0 , 0, 0 ),// 148 
                      PACK( 0 , 0 , 0 , 0, 0 ),// 149 
                      PACK( 0 , 0 , 0 , 0, 0 ),// 150 
                      PACK( 0 , 0 , 0 , 0, 0 ),// 151  
                      PACK( 0 , 0 , 0 , 0, 0 ),// 152
                      PACK( 0 , 1 , 0 , 0, 0 ),// 153 
                      PACK( 1 , 0 , 0 , 0, 1 ),// 154 
                      PACK( 0 , 0 , 0 , 0, 0 ),// 155 
                      PACK( 1 , 0 , 0 , 0, 1 ),// 156 s
                      PACK( 1 , 0 , 0 , 0, 1 ),// 157 t
                      PACK( 1 , 0 , 0 , 0, 1 ),// 158 
                      PACK( 1 , 0 , 0 , 0, 1 ),// 159 z
                      PACK( 0 , 0 , 0 , 0, 0 ),// 160   
                      PACK( 1 , 0 , 0 , 0, 0 ),// 161 ¡
                      PACK( 0 , 0 , 0 , 0, 0 ),// 162 ¢
                      PACK( 0 , 0 , 0 , 0, 0 ),// 163 £
                      PACK( 0 , 0 , 0 , 0, 0 ),// 164 ¤
                      PACK( 0 , 0 , 0 , 0, 0 ),// 165 ¥
                      PACK( 0 , 1 , 0 , 0, 0 ),// 166 ¦
                      PACK( 0 , 0 , 0 , 0, 0 ),// 167 §
                      PACK( 0 , 1 , 0 , 0, 0 ),// 168 ¨
                      PACK( 0 , 0 , 0 , 0, 0 ),// 169 ©
                      PACK( 0 , 1 , 0 , 0, 0 ),// 170 ª
                      PACK( 1 , 0 , 0 , 0, 0 ),// 171 «
                      PACK( 0 , 1 , 0 , 0, 0 ),// 172 ¬
                      PACK( 0 , 1 , 0 , 0, 0 ),// 173 ­
                      PACK( 0 , 0 , 0 , 0, 0 ),// 174 ®
                      PACK( 0 , 1 , 0 , 0, 0 ),// 175 ¯
                      PACK( 0 , 0 , 0 , 0, 0 ),// 176 °
                      PACK( 0 , 0 , 0 , 0, 0 ),// 177 ±
                      PACK( 0 , 0 , 0 , 0, 0 ),// 178 ²
                      PACK( 0 , 0 , 0 , 0, 0 ),// 179 ³
                      PACK( 0 , 0 , 0 , 0, 0 ),// 180 ´
                      PACK( 0 , 0 , 0 , 0, 0 ),// 181 µ
                      PACK( 0 , 0 , 0 , 0, 0 ),// 182 ¶
                      PACK( 0 , 1 , 0 , 0, 0 ),// 183 ·
                      PACK( 0 , 1 , 0 , 0, 0 ),// 184 ¸
                      PACK( 0 , 1 , 0 , 0, 0 ),// 185 ¹
                      PACK( 0 , 0 , 0 , 0, 0 ),// 186 º
                      PACK( 0 , 0 , 0 , 0, 0 ),// 187 »
                      PACK( 1 , 0 , 0 , 0, 0 ),// 188 ¼
                      PACK( 1 , 0 , 0 , 0, 0 ),// 189 ½
                      PACK( 1 , 0 , 0 , 0, 0 ),// 190 ¾
                      PACK( 1 , 1 , 0 , 0, 0 ),// 191 ¿
                      PACK( 1 , 0 , 0 , 1, 0 ),// 192 À
                      PACK( 1 , 0 , 0 , 1, 0 ),// 193 Á
                      PACK( 1 , 0 , 0 , 1, 0 ),// 194 Â
                      PACK( 1 , 0 , 0 , 1, 0 ),// 195 Ã
                      PACK( 1 , 0 , 0 , 1, 0 ),// 196 Ä
                      PACK( 1 , 0 , 0 , 1, 0 ),// 197 �

                      PACK( 0 , 0 , 0 , 0, 0 ),// 198 Æ
                      PACK( 0 , 0 , 0 , 1, 0 ),// 199 Ç
                      PACK( 1 , 0 , 0 , 1, 0 ),// 200 È
                      PACK( 1 , 0 , 0 , 1, 0 ),// 201 É
                      PACK( 1 , 0 , 0 , 1, 0 ),// 202 Ê
                      PACK( 1 , 0 , 0 , 1, 0 ),// 203 Ë
                      PACK( 1 , 0 , 0 , 1, 0 ),// 204 Ì
                      PACK( 1 , 0 , 0 , 1, 0 ),// 205 Í
                      PACK( 1 , 0 , 0 , 1, 0 ),// 206 Î
                      PACK( 1 , 0 , 0 , 1, 0 ),// 207 Ï
                      PACK( 1 , 0 , 0 , 1, 0 ),// 208 Ð
                      PACK( 1 , 0 , 0 , 1, 0 ),// 209 Ñ
                      PACK( 1 , 0 , 0 , 1, 0 ),// 210 Ò
                      PACK( 1 , 0 , 0 , 1, 0 ),// 211 Ó
                      PACK( 1 , 0 , 0 , 1, 0 ),// 212 Ô
                      PACK( 1 , 0 , 0 , 1, 0 ),// 213 Õ
                      PACK( 1 , 0 , 0 , 1, 0 ),// 214 Ö
                      PACK( 0 , 0 , 0 , 0, 0 ),// 215 ×
                      PACK( 0 , 0 , 0 , 1, 0 ),// 216 Ø
                      PACK( 1 , 0 , 0 , 1, 0 ),// 217 Ù
                      PACK( 1 , 0 , 0 , 1, 0 ),// 218 Ú
                      PACK( 1 , 0 , 0 , 1, 0 ),// 219 Û
                      PACK( 1 , 0 , 0 , 1, 0 ),// 220 Ü
                      PACK( 1 , 0 , 0 , 1, 0 ),// 221 Ý
                      PACK( 1 , 0 , 0 , 0, 0 ),// 222 Þ
                      PACK( 1 , 0 , 0 , 0, 0 ),// 223 ß
                      PACK( 1 , 0 , 0 , 0, 1 ),// 224 à
                      PACK( 1 , 0 , 0 , 0, 1 ),// 225 á
                      PACK( 1 , 0 , 0 , 0, 1 ),// 226 â
                      PACK( 1 , 0 , 0 , 0, 1 ),// 227 ã
                      PACK( 1 , 0 , 0 , 0, 1 ),// 228 ä
                      PACK( 1 , 0 , 0 , 0, 1 ),// 229 å
                      PACK( 1 , 0 , 0 , 0, 1 ),// 230 æ
                      PACK( 1 , 0 , 0 , 0, 1 ),// 231 ç
                      PACK( 1 , 0 , 0 , 0, 1 ),// 232 è
                      PACK( 1 , 0 , 0 , 0, 1 ),// 233 é
                      PACK( 1 , 0 , 0 , 0, 1 ),// 234 ê
                      PACK( 1 , 0 , 0 , 0, 1 ),// 235 ë
                      PACK( 1 , 0 , 0 , 0, 1 ),// 236 ì
                      PACK( 1 , 0 , 0 , 0, 1 ),// 237 í
                      PACK( 1 , 0 , 0 , 0, 1 ),// 238 î
                      PACK( 1 , 0 , 0 , 0, 1 ),// 239 ï
                      PACK( 1 , 0 , 0 , 0, 1 ),// 240 ð
                      PACK( 1 , 0 , 0 , 0, 1 ),// 241 ñ
                      PACK( 1 , 0 , 0 , 0, 1 ),// 242 ò
                      PACK( 1 , 0 , 0 , 0, 1 ),// 243 ó
                      PACK( 1 , 0 , 0 , 0, 1 ),// 244 ô
                      PACK( 1 , 0 , 0 , 0, 1 ),// 245 õ
                      PACK( 1 , 0 , 0 , 0, 1 ),// 246 ö
                      PACK( 0 , 0 , 0 , 0, 0 ),// 247 ÷
                      PACK( 0 , 0 , 0 , 0, 1 ),// 248 ø
                      PACK( 1 , 0 , 0 , 0, 1 ),// 249 ù
                      PACK( 1 , 0 , 0 , 0, 1 ),// 250 ú
                      PACK( 1 , 0 , 0 , 0, 1 ),// 251 û
                      PACK( 1 , 0 , 0 , 0, 1 ),// 252 ü
                      PACK( 1 , 0 , 0 , 0, 1 ),// 253 ý
                      PACK( 1 , 0 , 0 , 0, 1 ),// 254 þ
                      PACK( 1 , 0 , 0 , 0, 1 ),// 255 ÿ
                      };                  
/*------------------------------------------------------------------------------------*/
// Constructor, pone todos los valores por defecto
//<!----------------------------------------------------------------------------------->
CParseText::CParseText(void)
  {
	m_isRtf = false;
  m_Txt   = "";
  m_len   = 0;
  m_NTIni = "[";
  m_NTEnd = "]";
  m_nNoTrd = 0;
  }

/*------------------------------------------------------------------------------------*/
// Destructor, Libera todos los Items
//<!----------------------------------------------------------------------------------->
CParseText::~CParseText(void)
  {
  ClearSetting();
  }

/*------------------------------------------------------------------------------------*/
// Libera todos los Items analizados hasta ese momento
//<!----------------------------------------------------------------------------------->
void CParseText::ClearSetting(void)
  {
  for( int i=0; i<m_Items.GetSize(); ++i )  // Libera todos los Items
    delete (CItem *)m_Items[i];

  m_Items.RemoveAll();                      // Limpia el arreglo de Items
  }

/*------------------------------------------------------------------------------------*/
// Establece el texto que se va a analizar
//<!----------------------------------------------------------------------------------->
void CParseText::SetText(const CStringA& Text, bool aIsRtf)
  {
	m_isRtf = aIsRtf;
  if( Text.Length() == 0 ) return;
  
  m_Txt = Text.c_str();                    // Guarda puntero al texto
  m_len = Text.Length();                   // Longitud del texto

  /*
	// Determina el formato del texto
  int i=0;
  while( Text[i]<=' ' ) ++i;                  // Salta los espacios iniciales

	if (i >= m_len)														// Se llego al final del texto (nunca debiera ocurrir)
    m_Formato = F_TEXT;                     // Asumir como texto
	else if( m_Txt[i]=='<' )                  // Es un texto HTML
		m_Formato = F_HTML;
	else if( _strnicmp(m_Txt+i,"{\\rtf",5) == 0 )   // Es un texto RTF
		m_Formato = F_RTF;
	else
		{
		if (m_Txt[i+1] == '<')									// chequear el siguiente caracter
			m_Formato = F_HTML;
		else
			m_Formato = F_TEXT;                   // Es texto (Si no HTML ni RTF)
		}*/

  ClearSetting();                           // Libera todos los items que habia
  }

/*------------------------------------------------------------------------------------*/
//Obtiene el texto traducido completo.
//<!----------------------------------------------------------------------------------->
CStringA CParseText::GetTrdText(TAddItemMark aAddMark)
{
  CStringA  Text;                                     // Texto traducido completo
  CStrArray Sust;                                     // Cadenas para sustitución
  CStrArray Atrb;                                     // Cadenas de sust. atributos

  for( int i=0; i<m_Items.GetSize(); ++i )            // Recorre todos los items
    {
    CItem *Item = (CItem*)(m_Items.GetAt(i));         // Toma el Item actual

    if( Item->m_Type == 'c' || Item->m_Type == 's' )    // Si el item no se traduce
      {
      CString Tmp = Item->m_Text;                       // Obtiene la traduccion
      CString Txt;                                      // Obtiene la traduccion

      for( int j=0; j<Atrb.GetSize(); ++j)              // Para todas las sustituciones
        {
        int iFind = Tmp.Find(SUST_MARK);                // Busca marca de sustitucion
        if( iFind == -1 )                               // No encontro la marca 
          break;

        Txt += Tmp.Left(iFind);                         // Adiciona 1ra mitad al texto
        Txt += Atrb[j];                                 // Sustituye la cadena

        Tmp = Tmp.Mid(iFind+1);                         // Continua con 2da mitad
        }

      Txt += Tmp;                                       // Adiciona el resto al texto
      Atrb.RemoveAll();                                 // Limpia arreglo de sustitución

      if( Item->m_Type == 'c' )                         // Es un item de cascara
        Text += Txt;                                    // Adiciona item al texto
      else
        Sust.Add(Txt);                                  // Lo adiciona al arreglo

      continue;                                         // Coje el proximo item
      }

    if( Item->m_Type == 'T' )                           // Es un item de sustitución
      {
      Atrb.Add(Item->m_Trd);                            // Lo adiciona al arreglo
      continue;                                         // Coje el proximo item
      }

    if( Item->m_Type == 't' )                           // Si es un item que se traduce
      {
      CString Tmp = Item->m_Trd;                        // Obtiene la traduccion

      if( Tmp.GetLength() == 0 )                        // Si no hay texto traducido
        Tmp = Item->m_Text;                             // Coje el texto original

      for( int j=0; j<Sust.GetSize(); ++j)              // Para todas las sustituciones
        {
        int iFind = Tmp.Find(SUST_MARK);                // Busca marca de sustitucion
        if( iFind == -1 )                               // No encontro la marca 
          {
          Tmp += Sust[j];                               // La adiciona al final de la oración
          continue;                                     // Continua con la proxima marca
          }

        CheckEndOfRtfTab( Text );                       // Asegura que el texto no se mezcle con un tab RTF
        Text += Tmp.Left(iFind  );                      // Adiciona 1ra mitad al texto
        Text += Sust[j];                                // Sustituye la cadena

        Tmp = Tmp.Mid(iFind+1);                         // Continua con 2da mitad
        }

      CheckEndOfRtfTab( Text );                         // Asegura que el texto no se mezcle con un tab RTF
      Text += Tmp;                                      // Adiciona el resto al texto

      if (aAddMark == ADD_ITEM_MARK)
        Text += ITEM_MARK;
      Sust.RemoveAll();                                 // Limpia arreglo de sustitución
      }
    }

  return Text;                                        // Retorna texto completo
}

/*------------------------------------------------------------------------------------*/
// Analiza el texto establecido anteriormente y lo separa en oraciones
//<!----------------------------------------------------------------------------------->
bool CParseText::Parse()          
  {
  ClearSetting();                             // Libera todos los items que habia

  for( int i=0; i<m_len; )                   // Recorre todos los caracteres del texto
    {
    SkipNoText( i );                          // Obtiene todo hasta principio de oracion

    int _i = i;                               // Caracter donde se inicia la busqueda
    GetTextOra( i );                          // Obtiene todo hasta fin de oración

    if( i<m_len && _i == i )                  // No se puedo obtener texto
      AddItem( 'c', CStringA(m_Txt[i++]) );   // Pone el caracter en la cascara para que 
                                              // no caiga en un ciclo infinito
    }
                                     
  return true;
  }

/*------------------------------------------------------------------------------------*/
// Analiza el texto 'Text' a partir del caracter 'i', obteniendo todos los caracteres
// no texto y avanzando 'i' hasta el comienzo de la proxima oración. Si 'i' lleva hasta
// 'len' retorna false y termina.
//<!----------------------------------------------------------------------------------->
bool CParseText::SkipNoText( int& _i)
  {
  CStringA Cascara;
  int i;
  for( i=_i; i<m_len; ++i )                 // Para todos los caracters i    
    {
    //CheckNoTrdMark( i, NULL);               // Marca de no Traducción, la salta

    /*if( m_Txt[i]=='<' )                     // Inicio de comando HTML
      {
      CString Tag;
      long j = i;                           // Prserva le apuntador
      if( GetHtmlTag(j, Tag ))              // Obtiene un TagHTML
        {
        if( m_TagName == "IDX" )            // Es un comando XML para el traductor
          break;                            // Inicia la oración

        //if( IsHtml() )                      // Si el texto es HTML
          {
          Cascara += Tag;                   // Incorpora Tag a la cascara
          i = j;                            // Avanza el apuntador
          continue;                         // Continua buscando inicio de oración
          }
        }
      }*/

    char c = getChar(i);                      // Obtine el caracter i 

    if( (c=='\\' || c=='{' )   &&           // Caracter de inicio de comando RTF
        isRtf() && isCharText() )           // El texto es RTF y c no caracter especial 
      {
      if (getRtfTag(i, Cascara))            // Obtiene un TagRTF
        continue;                           // Continua con el resto de la oración
      }

    if( isc_ini(c)  )                       // Si el caracter puede inicial oración
      {
      if( IsBullet( m_ic, i, Cascara) )        // Si empieza con bullet
        continue;                           // Contunua analizando
      else
				{
        i = m_ic;                           // Idx donde comienza el caracter actual
        break;                              // Termina el analasis
				}
      }


    if (isCharText())                       // Es un caracter normal
      Cascara += c;                         // Guarda el caracter
    else                                    // Es un caracter especial
      Cascara += m_sChar;                   // Lo guarda como estaba
    }

  // Encontro el inicio de oración
  AddItem( 'c', Cascara );                  // Adiciona los caracteres a la lista
  _i = i;                                   // Pone el puntero al inicio de oracion

  return true;
  }

/*------------------------------------------------------------------------------------*/
// Chequea si a partir del caracter 'i' viene un marca de no traducción
//<!----------------------------------------------------------------------------------->
bool CParseText::CheckNoTrdMark( int& i, CString* Ora)
  {
  // Chequea si viene una marca de inicio de no traducción
  if( strncmp( m_Txt+i, m_NTIni.c_str(), m_NTIni.GetLength() ) == 0 )
    {
    i += m_NTIni.GetLength();             // La salta la marca

    if(Ora)                               // Si existe oración
      (*Ora) += m_NTIni;                  // Incorpora la marca al texto

    if( m_nNoTrd>0 && m_NTIni==m_NTEnd )  // Si son iguales las marcas y ya hay una
      --m_nNoTrd;                         // La toma como final (Decrementa contador)
    else
      ++m_nNoTrd;                         // Incrementa contador

//    return true;
    }

  // Chequea si viene una marca de final de no traducción
  if( strncmp( m_Txt+i, m_NTEnd.c_str(), m_NTEnd.GetLength() ) == 0 )
    {
    if( m_nNoTrd>0 )                // Si no esta macheada la ignora
      {
      i += m_NTEnd.GetLength();     // La salta

      if(Ora)                       // Si existe oración
        (*Ora) += m_NTEnd;          // Incorpora la marca al texto

      --m_nNoTrd;                   // Decrementa contador
      }

    return true;
    }

  return false;
  }

/*------------------------------------------------------------------------------------*/
// Analiza el caracter i y determina según el formato que se este usando si es un 
// caracter especial o no, si es un caracter normal devuelve el caracter tal como es, 
// si es un caracter especial lo decodifica y devuelve su equivalente ANSI ademas 
// adelanta el apuntador _i hasta el final de la definicion del caracter.
// Nota: Esta función tambien actualiza las siguientes varibles.
//        m_TypeChar - Tipo de caracter 0 normal, 1 - Especial RTF, 2- Especial HTML
//        m_ic       - Apuntador donde comenzo la definición del caracter
//        m_sChar    - Definición original del caracter
//
//<!----------------------------------------------------------------------------------->
char CParseText::getChar(int& _i)
  {
  m_typeChar = C_ANSI;                // Por defecto el caracter es ANSI
  m_ic       = _i;                    // Solo un caracter
  char     c =  m_Txt[_i];            // Toma caracter a analizar

  /*if (isHtml() && c=='&')             // Texto HTML e inicio de definición de caracter
    c = getCharHtml(_i);              // Procesa caracteres HTML especiales
  else*/
    {
    if (isRtf() && c=='\\')           // Texto RTF e inicio de definición de caracter
      {
      int j = _i;
      c = getCharRtf(_i);             // Procesa caracteres RTF especiales
      if( _i > j)
        {
        m_sChar = CString( m_Txt+j, _i-j+1 );   // Obtiene cadena que representa al caracter
        m_typeChar = C_RTF;                     // Declara tipo de caracter
        }
      }
    }

	if (c == SUST_MARK) c = 'x';							// Para evitar que hayan caracteres de sustitucion en el texto
	if (c == '<' || c == '>') c = ' ';        // Los caracteres <> confunden al traductor

  if( c!=' ' ) m_nSp = 0;             // Resetea contador de &nbsp; seguidos

  return c;
  }

/*------------------------------------------------------------------------------------*/
// Obtiene el codigo ANSI de un caracter definido según sintaxis de RTF, '_i' apunta
// al ultimo caracter de la definición del caracter.
//<!----------------------------------------------------------------------------------->
char CParseText::getCharRtf(int& _i)
  {
  if( m_Txt[_i]=='\\' )                 // Definición de un caracter especial
    {
    /*if( m_Txt[_i+1]=='{'  ||            // Para { -> '\{'
        m_Txt[_i+1]=='}'  ||            // Para } -> '\}'
        m_Txt[_i+1]=='\\' )             // Para \ -> '\\'
      {
      ++_i;
      return m_Txt[_i];
      }*/

    // Busca caracteres especiales en RTF
    LPCSTR cmdName  = "\\rquote   \\lquote   \\rdblquote\\ldblquote\\emdash   \\endash   \\~        \\emspace  \\enspace  \\-        \\_        \\";
    char  cmdChar[]= {'\''        ,'\''      ,'\"'      ,'\"'      ,'-'       ,'-'       ,' '       ,' '       ,' '       ,'\0'      ,'\0'      ,};

    // Obtine comando RTF
    int i = _i + 1;                           // Salta backslat

    // Comandos especiales de un solo caracter
    if( m_Txt[i]=='~' || m_Txt[i]=='-' || m_Txt[i]=='_')
      ++i;
    else
      while( (BYTE)m_Txt[i]>' ' && m_Txt[i]!='\\' ) // Salta caracteres del comando
        ++i;

    if (i > i+1)
      {
      CString cmd( m_Txt+_i, i-_i );            // Obtiene comando

      const char *ptr = strstr( cmdName, cmd.c_str() );       // Busca si es nombre de caracter
      if( ptr != NULL )                         // Si es un nombre de caracter
        {
        if( m_Txt[i]==' ' ) ++i;                // Salta espacio al final
        _i = i-1;                               // Retorna ultimo caracter leido

        int Idx =  ((int)(ptr-cmdName))/10;     // Indice del comando
        return cmdChar[Idx];                    // Retorna caracter equivalente
        }
      }

    if( m_Txt[_i+1]=='\'' )                   // Codigo del caracter en hexagesimal
      {
      char hNum[] = "  ";
      hNum[0] = m_Txt[_i+2];                  // Toma primer digito hexa    
      hNum[1] = m_Txt[_i+3];                  // Toma segundo digito hexa
      long n = strtol( hNum, NULL, 16 );      // Lo convierte a numero decimal
      if( n>32 && n<256 )                     // El número esta en rango
        {
        _i += 3;                              // Apunta al final
        return (char)n;                       // Devuelve el codigo del caracter
        }
      }
    }

  return m_Txt[_i];                           // Retorna el caracter actual
  }

/*------------------------------------------------------------------------------------*/
// Analiza el texto 'Text' a partir del caracter 'i', obteniendo todos los caracteres
// que forman parte de una oración y avanzando 'i' hasta el final de la oración. 
// Si 'i' lleva hasta 'len' retorna false y termina.
//<!----------------------------------------------------------------------------------->
bool CParseText::GetTextOra( int& _i)
  {
  CString    Ora;                             // Contenido de la oración
  CInnerTags ITags;                           // Arreglo de TAGs internos a la oracion  

  bool NtIni = ( m_nNoTrd > 0 );              // Si esta abierta la marca de no traducción
  m_nSp = 0;                                  // Resetea contador de &nbsp; seguidos

  int i=_i;
  for( ; i<m_len; ++i )                       // Para todos los caracters i    
    {
    CheckNoTrdMark(i, &Ora);                  // Verifica marcas para no traducir
    
    /*if( m_Txt[i]=='<' )                       // Inicio de comando HTML
      {
      if( IsHtml()  )                         // El texto es HTML y no caracter especial
        {
        CString sTag;    
        int n, ii = i;                        // Posicion donde comienza el Tag
        if( GetInnerHtmlTags( i, sTag, n ) )  // Todos los tag internos a la oración
          {
          int Len = Ora.GetLength();          // Posicion donde se va a poner la marca
          ITags.Add( Ora, sTag, Len, ii );    // Adiciona una marca de sustitución y guarda información
          continue;                           // Continua con el resto de la oración
          }
        else
          {
          if( n>0 ) goto Termina;             // Encontro un TAG terminador
          }
        }

      CString s; long k=i;
      if( GetHtmlTag(k, s ) && m_TagName=="IDX" ) // Es un TAG interno de a la oración
        {
        Ora += s;                             // Adiciona el TAG a la oración
        i = k;                                // Actualiza la posición 
        continue;                             // Continua con el resto de la oración
        }
      }*/

    char c = getChar(i);                      // Obtine el caracter i 

    if( c=='\0' && m_typeChar != C_ANSI)      // Hay que ignorar el caracter
      continue;

    if( m_nSp>1 )                             // Mas de un &nbsp; seguido
      {
      i = m_iSp;                              // Restaura la posicion del primer &nbsp;
      goto Termina;                           // Corta la oracion
      }

		if (m_typeChar == C_HTML_UNICODE)
			{
      int Len = Ora.GetLength();                                    // Posicion de la marca
      ITags.Add( Ora, m_sChar, Len, i - m_sChar.GetLength() + 1);   // Adiciona una marca de sustitución y guarda información
			continue;
			}

    if( isc_end(c)  )                                               // Si el caracter puede ser fin de oración
      {
      switch( c )
        {
        case '\r': case '\n':                                       // Cambio de linea.
          if( isText() )                                       // Si es formato de texto
            {
            int j = i+1;                                            // Indice temporal
            if( c=='\r' && m_Txt[j]=='\n' )                         // Car return, seguido de New line
              ++j;                                                  // Salta el new line

            if( c=='\n' && m_Txt[j]=='\r' )                         // New line, seguido de Car return
              ++j;                                                  // Salta el Car return
          
            for(; m_Txt[j] && m_Txt[j]<=' '; ++j )                  // Espacios y caracteres especiales
              {
              if( m_Txt[j]=='\n' || m_Txt[j]=='\r' )                // Linea vacia
                goto Termina;                                       // Corta
              }

            if( isc_up(m_Txt[j]) )                                  // Proxima letra mayuscula
              goto Termina;                                         // Corta

            CString s;
            if( IsBullet(j, j, s) )                                 // Es un Bullet
              goto Termina;                                         // Corta

            s = CString( m_Txt+i, j-i );                            // Coje la cascara
            Ora += ' ';                                             // Espacio para separar palabras

            int Len = Ora.GetLength();                              // Posicion de la marca
            ITags.Add( Ora, s, Len, i );                            // Adiciona una marca de sustitución y guarda información

            i = j-1;                                                // Actaliza pocición actual
            continue;                                               // Continua con resto de la oración
            }
          else                                   
            {
            if( isHtml() )                                          // Si es formato HTML
              {
              int len = Ora.GetLength();                            // Longitud de la cadena
              if( len>0 && Ora[len-1] != ' ' )                      // Si el ultimo no es espacio
                Ora += ' ';                                         // Agreaga un espacio
              }
            continue;                                               // Los ignora
            }
                                                                    
        case '\t':                                                  // Tab
          {
          if( isText() )                                            // Si es formato de texto
            goto Termina;                                           // Siempre corta la oración

          if( isHtml() )                                            // Si es formato de Html
            break;                                                  // Siempre lo toma (Html lo ignora)

          long j=i+1;                                               // Salta el tab
          while( j<m_len && m_Txt[j]<=' ')                          // Salta espacios tercos
            ++j;  

          if( isc_up(m_Txt[j]) )                                    // Sigue un caracter en mayusculas
            goto Termina;                                           // Corta la oración
          }
          break;                                                    // Lo toma

        case ':':
          if( isFile( i, Ora) )                                     // Nombre de fichero o URL
            continue ;                                              // Lo toma
          goto Termina;                                             // Corta

        case '.':
          {
          if( isc_up(m_Txt[i-1]) &&                                 // Antecedido de mayuscula
              isc_up(m_Txt[i+1])  )                                 // Seguido de mayuscula
            break;                                                  // Lo toma       

          if( IsExt(i+1) || IsAbr(i-1) )                            // Si es una extension o abreviatura
            break;                                                  // Lo toma

          long j = i+1 ;
          for( ; m_Txt[j]==' '; ++j );                              // Salta espacios

          if( isc_up(m_Txt[j])     ||                               // Le sigue mayuscula
             !isc_alfanum(m_Txt[j]) )                               // Le sigue no alfanumerico
            goto Termina;                                           // Rompe.

          break;                                                    // En otro caso lo toma
          }

        case '!':
          if( Ora.GetLength()==0  )                                 // Es el primer caracter
            break;                                                  // Siempre lo toma
                                                                    // Sigue para abajo
        case '?':
          Ora += m_Txt[i++];                                        // Lo toma y despues termina
          goto Termina;                       

        case '�':          
          if( Ora.GetLength()==0  )                                 // Es el primer caracter
            break;                                                  // Lo toma

          i = m_ic;                                                 // Indice del ultimo caracter analizado
          goto Termina;                                             // En otro caso, corta
          
        case '�':
        case '"':
          {
          if( Ora.GetLength()==0  )                                 // Es el primer caracter
            break;                                                  // Siempre lo toma

          int j = i+1;
          while( j<m_len && m_Txt[j] == ' ') ++j;                   // Salta espacios
          if( !isc_alfanum(m_Txt[j]) )                              // Seguido de no alfanumerico
            {
            Ora += m_Txt[i++];                                      // Lo toma y despues termina
            goto Termina;                                           
            }
          break;                                                    // Lo toma y sigue
          }

        case '`':
          c = '\'';                                                 // Sustituye el tipo de comilla
          break;                                                    // Y la toma siempre

        case '\\':                                                  // Inicio de comando RTF
        case '{':                                                   // Inicio de bloque de comandos RTF
        case '}':                                                   // Fin de bloque de comandos RTF
          if( isRtf() && isCharText() )                             // Texto RTF y no caracter especial?
            {
            CString sTag;       
            int ii = i;                                             // Posicion donde comienza el Tag
            if( getInnerRtfCmd( i, sTag )>0 )                       // Todos los tag internos a la oración
              {
              int Len = Ora.GetLength();                            // Posicion donde se va a poner la marca
              ITags.Add( Ora, sTag, Len, ii );                      // Adiciona una marca de sustitución y guarda información

              --i;                                                  // Compensa incremento del for(...)
              continue;                                             // Continua con el resto de la oración
              }
            }
          goto Termina;                                             // Corta
        default:                                                    // Siempre es fin de oración
          i = m_ic;                                                 // Indice del ultimo caracter analizado
          goto Termina;                                             // Corta
        } // end switch
      } // end if

    Ora += c;                                                       // Agrega caracter a la oración
    } // end for

Termina:;

  FixApostrofes(Ora, ITags);                                        // Arregla los apotrofes

  bool NtEnd = (m_nNoTrd > 0);                                      // Continua abierta la marca de no traducción

  if( NtIni || NtEnd  )                                             // Inicio, final o toda la oracion no se traducen
    {
    SplitByNoTrdMark(Ora, NtIni, NtEnd, ITags );                     // Pica la oración según el caso
    }
  else
    {
    i = DelEndSustMark( i, Ora, ITags);                             // Borra Marcas al final de la oración

    if( ITags.Len() && isRtf() )                                    // Si hay tags internos y es RTF
      GetBullet2( Ora, ITags);                                      // Obtiene bullet al inicio de la oración 

    DelIniSustMarks( Ora, ITags );                                  // Quita marcas del inicio
    JoinSustMarks( Ora, ITags );                                    // Une las marcas que estan cosecutivas

    for(int j=0; j<ITags.Len(); ++j )                               // Guarda cascara interior de la oracion
      AddItem( 's', ITags.GetTxt(j) );                              // Adiciona cadena de sustitución

    AddItem( 't', Ora );                                            // Adiciona oración a la lista de items
    }

  _i = i;                                                           // Pone el puntero al final de la oracion
  return true;
  }

/*------------------------------------------------------------------------------------*/
// Quita caracteres especiales delante de los apotrofes, para garantizar palabras con
// contracción
//<!----------------------------------------------------------------------------------->
void CParseText::FixApostrofes( CString& Ora, CInnerTags& ITags )
  {
  for(int i=0; i<ITags.Len(); ++i )                     // Busca por todas las marcas
    {
    int j = ITags.GetiOra(i);                           // Posición de la marca

    if( j==0 || j>=Ora.GetLength()-1 )                  // La marca esta en un extremo
      continue;                                         // No hace nada

    if( (isc_alfa(Ora[j-1]) || Ora[j-1]=='\'' ) &&      // Si la marca esta entre
        (isc_low(Ora[j+1])  || Ora[j+1]=='\'' ) )       // letras o apostrofe
      {
      int k;

      // Busca el inicio de la palabra y corre los caracteres     
      for( k=j-1; k>=0 && (isc_alfa(Ora[k]) || Ora[k]=='\''); --k ) 
        Ora.SetAt( k+1 , Ora[k] );                      

      Ora.SetAt( k+1, SUST_MARK );                      // Pone la marca el principio

      ITags.SetiOra( i, k+1 );                          // Rectifica posicion de la marca
      }


/*
    if( j>0                 && isc_alfa(Ora[j-1]) &&    // Tiene delante una letra
        j<Ora.GetLength()-1 && Ora[j+1]=='\''       )   // Detras un apotrofe
      {
      Ora.SetAt( j  , '\'' );                           // Intercambia caracteres
      Ora.SetAt( j+1, SUST_MARK );

      ITags.SetiOra( i, j+1 );                          // Rectifica posicion de la marca
      }
*/
    }
  }

/*------------------------------------------------------------------------------------*/
// Analiza la oración obtenida para ver si todavia permanece comenzando con un bullet,
// tiene en cuanta los siguientes formatos:
//    Romano  - Ej: I- II- III- ó I. II. III. ó I) II) III) ó I)- II)- III)- 
//              Ej: i- ii- iii- ó i. ii. iii. ó i) ii) iii) ó i)- ii)- iii)- 
//    Numero  - Ej: 1- 2- 3- ó 1. 2. 3. ó 1) 2) 3) ó 1)- 2)- 3)- 
//    Letra   - Ej: a- b- c- ó a. b. c. ó a) b) c) ó a)- b)- c)- 
//<!----------------------------------------------------------------------------------->
bool CParseText::GetBullet2( CString& Ora, CInnerTags& ITags )
  {
  bool num = false;
  int    j = 0;

  while( Ora[j]=='I' || Ora[j]=='V' || Ora[j]=='X' )      // Romanos mayusculas
    ++j;

  if( j == 0 )
    while( Ora[j]=='i' || Ora[j]=='v' || Ora[j]=='x' )    // Romanos minusculas
      ++j;

  if( j == 0 )
    while( isc_num(Ora[j]) )                              // Numeros 
      { 
      ++j; 
      if( Ora[j]=='.' && isc_num(Ora[j+1]) )              // Si 2.1 ...
        { j+=2; num = true; }                             // Salta el punto y num.
      }

  if( j == 0 && isc_low(Ora[0]))                          // Un caracter en minuscula                         
    j = 1;                                                // Lo toma
  
  if( j &&  (Ora[j]=='-' || Ora=='.' ||             // Seguido de - ó .
      num || Ora[j]==')' || Ora==':') )             // Seguido de ) ó :
    {
    if( !num ) ++j;                                 // Salta caracter adicional

    if( Ora[j]=='-' ) ++j;                          // Opcionalmente - Ej a)- i.- a:-  

    int nMark = 0;
    bool sp   = false;                              // Bandera para espacio

    for(;;)                                         // Busca caracteres que le siguen
      {
      if( Ora[j] == SUST_MARK )                     // Marca de sustitución
        ++nMark;
      else if( Ora[j] == ' ' || Ora[j] == '\t')     // Espacio o tabulador
        sp = true;
      else
        break;                                      // Termina la busqueda

      ++j;                                          // Salta el caracter
      }

    if( sp )                                        // Encontro espacio, es un bullet
      {
      CString sIni( Ora.Left(j) );                  // Toma el bullet mas la cascara
      Ora = Ora.Mid(j);                             // Lo quita de la oración

      for( int n = nMark-1; n>=0; --n )             // Busca todas las marcas que habia
        {
        int i = ITags.GetiOra(n);                   // Posición de la marca en la oración

        sIni = sIni.Left(i) + ITags.GetTxt(n) + sIni.Mid(i+1);  // Sustituye la marca

        ITags.DeleteAt(n);                          // Borra la marca
        }

      AddItem( 'c', sIni );                         // Guarda la cascara

      return true;                                  // Retorna OK
      }
    }

  return false;                                     // No complio con el formato
  }

/*------------------------------------------------------------------------------------*/
// Quita caracteres especiales que aparecen al inicio de la oración
//<!----------------------------------------------------------------------------------->
void CParseText::DelIniSustMarks( CString& Ora, CInnerTags& ITags )
  {
  int nMark = 0;

  int j=0;
  for(;j<Ora.GetLength(); ++j )                   // Busca caracteres que le siguen
    {
    if( Ora[j] == SUST_MARK )                     // Marca de sustitución
      ++nMark;
    else if( (BYTE)Ora[j] > ' ')                  // Si no espacio o caracter de control
      break;                                      // Termina la busqueda
    }

  if( j>0 )                                       // Encontro espacio, es un bullet
    {
    CString sIni( Ora.Left(j) );                  // Toma el bullet mas la cascara
    Ora = Ora.Mid(j);                             // Lo quita de la oración

    for( int n = nMark-1; n>=0; --n )             // Busca todas las marcas que habia
      {
      int i = ITags.GetiOra(n);                   // Posición de la marca en la oración

      sIni = sIni.Left(i) + ITags.GetTxt(n) + sIni.Mid(i+1);  // Sustituye la marca

      ITags.DeleteAt(n);                          // Borra la marca
      }

    AddItem( 'c', sIni );                         // Guarda la cascara
    }
  }

/*------------------------------------------------------------------------------------*/
// Une las marcas de sustitución que vienen juntas o separadas por espacio dentro de
// dentro de la oración la oracion
//<!----------------------------------------------------------------------------------->
void CParseText::JoinSustMarks( CString& Ora, CInnerTags& ITags )
  {
  for(int i=0; i<ITags.Len(); ++i )                             // Busca por todas las marcas
    {
    int nMark = 0;
    int j = ITags.GetiOra(i);                                   // Posición de la marca

    for( int k=j+1; ; ++k )                                     // Busca marcas seguidas 
      {
      if( k >= Ora.GetLength() ) break;                         // Llego al final de la oración, termina
      if( Ora[k] == SUST_MARK )                                 // Es un caracter marca de sustitución
        ++nMark;                                                // Incrementa contador de marcas seguidas
      else if( (BYTE)Ora[k] > ' ' ) break;                      // No es espacio o caracter de control, termina
      }

    if( nMark > 0 )                                             // Encontro una o mas marcas seguidas
      {
      CString s = ITags.GetTxt(i);                              // Coje texto de la primera marca
      for( int n=0; n<nMark; ++n )                              // Para todas las marcas consecutivas
        {
        s += ITags.GetTxt(i+1);                                 // Adiciona texto de marca actual a la anterior

        int p = ITags.GetiOra(i+1) - n;                         // Posición de la marca actual en la oración
        Ora.Delete( p );                                        // Borra caracter de la oración

        ITags.DeleteAt(i+1);                                    // Borra la marca de lista de marcas
        }

      ITags.SetTxt( i, s );                                     // Incorpora texto de toda las marcas a la inicial

      for(int k=i+1; k<ITags.Len(); ++k )                       // Para todas las marcas posteriores a la actual
        ITags.SetiOra( k, ITags.GetiOra(k) - nMark );           // Arregla posición de acuerdo a las marcas borradas
      }
    }
  }

/*------------------------------------------------------------------------------------*/
// Determina si al final de la oración hay una marca de sustitución que se puede quitar
//<!----------------------------------------------------------------------------------->
int CParseText::DelEndSustMark(int& i, CString& Ora, CInnerTags& ITags)
  {
  int n = ITags.Len()-1;              // Indice al ultimo TAG interno
  int k = Ora.GetLength()-1;          // Indice al ultimo caracter de 'Ora'

  for( ;k>=0 && n>=0; --k )           // Analiza 'Ora' desde atras
    {
    if( (BYTE)Ora[k]<=' ' )           // Caracteres sin importancia al final
      continue;                       // Continua analizando

    if( Ora[k] == SUST_MARK )         // Encuentra marca de sustitución
      {
      i = ITags.GetiTxt(n--);         // Obtiene inicio del TAG en el texto
      continue;                       // Continua analizando
      }

    break;                            // Termina el analisis
    }

  if( n < ITags.Len()-1 )             // Si encontro alguna marca de sustitución
    {
    Ora = Ora.Left(k+1);              // Quita caracteres analizados (del final)
    ITags.SetLen( n+1 );              // Quita TAGs analizados del arreglo
    }

  return i;                           // Retorna caracter para seguir analisis
  }

/*------------------------------------------------------------------------------------*/
// Obtiene todos los tags consecutivos que son internos a una oración
//<!----------------------------------------------------------------------------------->
int CParseText::getInnerRtfCmd(int& _i, CString& iCmd)
  {
  CString Cmds;
  bool    tab = false;                      // Bandera para si hay tabuladores
  int i = _i;                              // Guarda posicion temporalmente
  m_icmd = _i;                              // Indice donde termina el comando

  for(;;)                                   // Para todos los comandos consecutivos
    {     
    if( m_Txt[i]=='}' )                     // Fin de bloque
      {
      Cmds += m_Txt[i++];                   // Guarda la llave y salta

      while( i<m_len && (BYTE)m_Txt[i]<' ' )// Salta posibles tarecos 
        Cmds += m_Txt[i++];                 // Guarda los tarecos
      }

    int k = i;                             // Apuntador temporar a caracter leido
    getCharRtf(k);                          // Procesa caracteres RTF especiales
    if( k>i )                               // Es una caracter especial
      {
      int len = Cmds.GetLength();           // Longitud de los comandos
      if( len && Cmds[len-1] != ' ')        // El comando no termina en espacio
        Cmds += ' ';                        // Se lo agrega

      break;                                // Termina..
      }

    if( !getRtfTag(i, Cmds ) )              // Obtiene un comando RTF
      break;                                // No puedo obternelo termina

    m_icmd = ++i;                           // Indice donde termina el comando
    if( m_TagName=="tab" )                  // Encontro un TAB
      tab = true;                           // No decide hasta el final
    else
      if( !isInnerParagraf(m_TagName.c_str(), m_TagValue.c_str()) )     // Si no puede estar interno a la oración
        return -1;                          // No todos son internos

    while( i<m_len && (BYTE)m_Txt[i]< ' ' ) // Salta posibles tarecos 
      Cmds += m_Txt[i++];                   // Guarda los tarecos
    }

  int len = Cmds.GetLength();               // Longitud de los cmd analizados
  if( len >0 )                              // Obtuvo algun Comando?
    {
    if( tab )                               // Si hubo un tab intermedio
      {
      int j=i;                              // Utiliza indice temporar
      while( j<m_len && m_Txt[j]<=' ' )     // Salta posibles tarecos y espacio
        ++j;

      if( isc_up(m_Txt[j]) )                // Sigue un caracter en mayusculas
        return -1;                          // El tab termina la oración
      }

    _i = i;                                 // Actuliza ultima pocicion analizada
    iCmd = Cmds;                            // Lista de comandos obtenidos
    }

  return len;                               // Longitud de los comandos internos
  }

/*------------------------------------------------------------------------------------*/
// Determina si el TAG 'Tag' puede estar dentro de un parrafo o no.
//<!----------------------------------------------------------------------------------->
bool CParseText::isInnerParagraf(LPCSTR Tag, LPCSTR Value)
  {
  // Lista de comandos HTML que pueden estar dentro de una oración
  static LPCSTR InHTM = "<A><ADRESS><AREA><B><BASE><BASEFONT><BGSOUND><BIG><CITE>"
                       "<EM><FONT><I><KBD><LINK><LISTING><NOBR><S><SAMP><SMALL>"
                       "<STRIKE><STRONG><SUB><SUP><U><VAR><XMP><SPAN>";         

  // Lista de comandos RTF que seaparan las oraciones
  static LPCSTR InRTF = "<par><cell><row><tab><nestcell><nestrow><sect><pard><tcelld>"
                       "<trowd><line>";

  // los comandos \u8232 y \u8233 son cambio de linea y de parrafo
  if (Tag[0] == 'u' && Tag[1] == 0)
    {
    if (strcmp(Value, "8232") == 0 || strcmp(Value, "8233") == 0)
      return false;
    return true;
    }
      
  CString sTag = '<'+(CString)Tag+'>';    // Pone seperadores para buscar

  bool ret; 
  if( isHtml() )                          // Si el texto es HTML
    ret=( strstr(InHTM, sTag.c_str())!=NULL );    // Busca en lista de comandos HTML
  else                                    // Si el texto es RTF
    ret=( strstr(InRTF, sTag.c_str())==NULL);     // Busca en lista de comandos RTF

  return ret;                             // Si lo encuentra retorna true
  }

/*------------------------------------------------------------------------------------*/
// Pica la oracion según la pocision de la marca de no traducción, tambien se tienen en
// cuenta las marcas de sustitución que estan en la parte que no se va ha traducir.
//<!----------------------------------------------------------------------------------->
void CParseText::SplitByNoTrdMark(CString& Ora, bool NtIni, bool NtEnd, CInnerTags& ITags )
  {
  int SpIni = -1;
  int SpEnd = Ora.GetLength();
  if( NtIni )                             // Comienza con una marca
    {
    SpIni = Ora.Find( m_NTEnd );          // Busca la marca final
    if( SpIni == -1 )                     // No la encuentra (Toda la oración)
      SpIni = Ora.GetLength();            // Logitud de la oracion
    }

  if( NtEnd )                             // Termina con una marca
    {
    SpEnd = 0;                            // Asume toda la oración
    for( int i=0;;)
      {
      int iFind = Ora.Find( m_NTIni, i ); // Busca la marca final
      if( iFind == -1 )                   // No la encuentra (Toda la oración)
        break;

      SpEnd = iFind;                      // Ultima marca encontrada
      i     = iFind + m_NTIni.GetLength();
      }
    }

  int iNew = 0;                           // Nuevo indice en el arreglo
  int dt = 0;                             // Incremento del tamaño de la oración
  CString s1,s2;
  for( int i=0; i<ITags.Len(); ++i )      // Recorre todas las marcas
    {
    int Idx = ITags.GetiOra(i) + dt;      // Recalcula el indice a la marca
    if( Idx<SpIni || Idx>=SpEnd )         // Marca dentro del segmento inicial o final
      {                                   // Sustitulye la marca
      s1 = Ora.Left(Idx);                 // Obtiene primera mitad
      s2 = Ora.Mid (Idx+1);               // Obtiene segunda mitad

      Ora = s1 + ITags.GetTxt(i) + s2;    // Inserta el texto donde esta la marca
      int len = ITags.GetTxt(i).GetLength()-1;  // Numero que se incrementa la oración

      dt += len;                          // Suma lo que crece la oración

      if( Idx<SpIni ) SpIni += len;       // Recalcula segmento inicial 
      if( Idx<SpEnd ) SpEnd += len;       // Recalcula segmento final      
      }
    else
      ITags.Copy( i, iNew++ );            // Indices no sustituidos
    }

  ITags.SetLen(iNew);                     // Redimensiona arreglo de TAGs internos

  if( SpIni >= SpEnd )                    // Toda la oración no se traduce
    {
    AddItem( 'c', Ora );                  // Adiciona oración a la lista de items
    return;  
    }
  
  // Analiza Segmento inicial si esxiste
  if( SpIni > 0 )                         // Procesa parte inicial
    {
    s1 = Ora.Left(SpIni);                 // Separa primer segmento
    AddItem( 'c', s1 );                   // Adiciona segmento a la lista de items
    }

  // Analiza parte intermedia
  if( SpIni<0 )
    SpIni = 0;
  else
    SpIni += m_NTEnd.GetLength();         // Salta la marca

  s1 = Ora.Mid( SpIni, SpEnd-SpIni );     // Toma parte intermedia

  for( int i=0; i<ITags.Len(); ++i )      // Recorre items no sutituidos
    AddItem( 's', ITags.GetTxt(i) );      // Adiciona texto a la lista de items

  AddItem( 't', s1 );                     // Adiciona oración a la lista de items

  SpEnd += m_NTIni.GetLength();           // Salta la marca     

  // Analiza segmento final si existe
  if( SpEnd < Ora.GetLength() )           // Procesa parte final ?
    {
    s1 = Ora.Mid(SpEnd);                  // Separa segmento final
    AddItem( 'c', s1 );                   // Adiciona segmento a la lista de items
    }
  }

/*------------------------------------------------------------------------------------*/
// Determina si la palabra que sigue al punto es una de las exteniones mas conocidas
// para nombres de ficheros
//<!----------------------------------------------------------------------------------->
bool CParseText::IsExt(int i)
  {
  int j=i;
  for(; isc_alfa(m_Txt[j]); ++j ) {};                   // Salta caracteres
  if( j==i )                                            // No encontor ninguno
    return false;                                       // Retorna no extension

  static const char *Exts = "|net|com|gob|doc|txt|es|mx|htm|html|exe|dll|xml|rtf|bmp|jpe|zip|"
                      "|psd|pdf|rar|reg|mp|avi|dat|ttf|hlp|gid|sys|cfg|gif|pnp|wmf|wmv|"
                      "|wma|asf|mid|dwg|pdf|mht|ini|bad|log|tmp|drv|ocx|inf|";

  CStringA sExt = '|' + CStringA(m_Txt+i, j-i) + '|';   // Forma palabar para buscar
  sExt.MakeLower();
  //_strlwr( (LPSTR)(LPCSTR)sExt );
  
  if( strstr(Exts, CS2SZ(sExt)) )                              // Busca en lista de extensiones
    return true;                                        // Retorna no extension
 
  return false;                                         // Retorna no extension
  }

/*------------------------------------------------------------------------------------*/
// Trata de determinar si la palabra que antecede al punto es una abreviatura, o no.
//<!----------------------------------------------------------------------------------->
bool CParseText::IsAbr(int i)
  {
  int j=i;
  for( ; j>0 && isc_alfa(m_Txt[j]); --j ){};          // Salta caracteres hacia atras
  if( j==i )                                          // No encontro nada
    return false;                                     // Retorna no abreviatura

  if( j==0 )                                          // Es una sola palabra
    return true;                                      // No corta

  if( j+1 == i && m_Txt[j]=='.' )                     // Si es X.X.X. ó x.x.x
    return true;                                      // Retorna no extension

  if( i-j < 5 && isc_up(m_Txt[j+1]) )                 // Cortica y comienza en mayuscula
    {                                                 // Ej: Xxx.
    for( int k=j+2; k<i; ++k )                        // Para los demas caracteres
      if( isc_up(m_Txt[k]) )                          // Es mayuscula
        return false;                                 // No es abreviatura

    return true;                                      // Es abreviatura
    }

  while( j>0 && m_Txt[j] <= ' ')                     // Salta tarecos hacia atras
    --j;

  if( m_Txt[j]=='(' || m_Txt[j]=='{'  || m_Txt[j]=='[' || // Caracter de agrupamiento
      m_Txt[j]=='"' || m_Txt[j]=='�'  )                   // Ej: ( xxxx. ó "xxxxx.
    return true;                                          // Retorna si abreviatura

  return false;                                           // Retorna no extension
  }

/*------------------------------------------------------------------------------------*/
// Analiza el texto 'Text' a partir del caracter 'i', determina si es un numerado con
// uno de los siguientes formatos:
//    Romano  - Ej: I- II- III- ó I. II. III. ó I) II) III) ó I)- II)- III)- 
//              Ej: i- ii- iii- ó i. ii. iii. ó i) ii) iii) ó i)- ii)- iii)- 
//    Numero  - Ej: 1- 2- 3- ó 1. 2. 3. ó 1) 2) 3) ó 1)- 2)- 3)- 
//    Letra   - Ej: a- b- c- ó a. b. c. ó a) b) c) ó a)- b)- c)- 
//
// Si es este el caso, pone los caracteres en la cadena 'Cascara', avanza i y retorna
// true, en otro caso retorna false sin hacer nada.
//<!----------------------------------------------------------------------------------->
bool CParseText::IsBullet(int ini, int& i, CStringA& Cascara)
  {
  bool num = false;
  int   j = ini;

  while( m_Txt[j]=='I' || m_Txt[j]=='V' || m_Txt[j]=='X' )      // Romanos mayusculas
    ++j;

  if( j == ini)                                                 // No ha encontrado nada
    while( m_Txt[j]=='i' || m_Txt[j]=='v' || m_Txt[j]=='x' )    // Romanos minusculas
      ++j;

  if( j == ini)                                     // No ha encontrado nada
    while( isc_num(m_Txt[j]) )                      // Numeros 
      { 
      ++j; 
      if( m_Txt[j]=='.' && isc_num(m_Txt[j+1]) )    // Si 2.1 ...
        { j+=2; num = true; }                       // Salta el punto y num.
      }

  if( j == ini)                                     // No ha encontrado nada        
    j = i + 1;                                      // Salta caracter actual
  
  int skip = 0;
  CStringA sTag;       

  if( !skip && (m_Txt[j]=='-' || m_Txt[j]=='.' ||   // Seguido de - ó .
                m_Txt[j]==')' || m_Txt[j]==':') )   // Seguido de ) ó :
    {
    ++j;

    if( m_Txt[j]=='-' ) ++j;                        // Opcionalmente - Ej a)- i.- a:-    

    if( m_Txt[j]==' ' )                             // Seguido de espacio
      skip = 1;
    }

  if( !skip && num && m_Txt[j]==' ' )               // Numero seguido de espacio
    skip = 1;

  if( !skip && m_Txt[j]=='\t' )                     // Seguido de tab normal
    skip = 1;

  if( skip )                                        // Es un bullet o numerando
    {
    j += skip;
    Cascara += CStringA( m_Txt+ini, j-ini);          // Lo mete en la cascara

    i = j-1;                                        // Actualiza el puntero
    return true;
    }

  return false;                                     // No complio con el formato
  }

/*------------------------------------------------------------------------------------*/
// Adiciona un item a la lista
//<!----------------------------------------------------------------------------------->
bool CParseText::AddItem(BYTE Type, const CStringA& Text)
  {
  if (Text.GetLength() == 0)
  //if( !Text || Text[0]==NULL )
    return false;

  CStringA Txt = Text;

  m_Items.Add( new CItem(Txt,Type) );

  return true;
  }

#define mytolower(ch)  (((ch) >= 'a' && (ch) <= 'z') ? (ch) : (ch) - ('a' - 'A'))  
TInt _strnicmp_0(const char* s1, const char* s2, TInt len);

/*------------------------------------------------------------------------------------*/
// Funcion parecida a la _strnicmp, que se implementa aqui porque no existe en Symbian
//<!----------------------------------------------------------------------------------->
TInt _strnicmp_0(const char* s1, const char* s2, TInt len)
	{
	for(TInt i=0; i<len; i++)
		{
		char ch1 = mytolower(s1[i]);
		char ch2 = mytolower(s2[i]);
		if (ch1 == 0 || ch2 == 0 || ch1 != ch2)
			return 1;
		}
	
	return 0;
	}

/*------------------------------------------------------------------------------------*/
// Determina si es un nombre de fichero o un URL
//<!----------------------------------------------------------------------------------->
bool CParseText::isFile( int& _i, CStringA& Ora)
  {
  int i = _i;
  if( (i>2 && !isc_alfanum(m_Txt[i-2]) && isc_alfa(m_Txt[i-1]) ) || // A:\ ...
      (m_len>4 && _strnicmp_0(m_Txt+i-4, "file", 4) == 0  )      ||      // file: ...
      (m_len>4 && _strnicmp_0(m_Txt+i-4, "http", 4) == 0  )      ||      // http: ...
      (m_len>3 && _strnicmp_0(m_Txt+i-3, "ftp" , 3) == 0  )      )       //  ftp: ...
    {
    ++i;                         // Salta el : 

    // Solo seguido de numero, letra o slat o backslat
    if( !isc_alfanum(m_Txt[i]) && m_Txt[i]!='\\' && m_Txt[i]!='/' )
      return false;
  
    // Toma el resto del nombre del fichero
    for( ;i<m_len; ++i )
      {
      if( isc_alfanum(m_Txt[i]) )
        continue;

      if( m_Txt[i]=='\\' || m_Txt[i]=='/' )
        continue;

      // El punto solo si esta entre letras o numeros
      if( m_Txt[i]=='.' && isc_alfanum(m_Txt[i-1]) && isc_alfanum(m_Txt[i+1]) )
        continue;

      break;
      }

    Ora += CStringA( m_Txt+_i, i-_i);    // Coje el nombre de fichero
    _i = i-1;
    return true;
    }

  return false;
  }

/*------------------------------------------------------------------------------------*/
//**************** INICIO DE LAS FUNCIONES ESPECIFICAS PARA RTF **********************//
/*------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------*/
// Obtiene un tag (comando) RTF, y lo pone en la cadena 'Txt', para que esta función 
// trabaje 'i' debe apuntar a un caracter '{' o '\', si se puede obtener el TAG devuelve
// 'true' e 'i' apunta al ultimo caracter del Tag, en otro caso devuelve 'false' y no se 
// modifica ni 'i' ni 'Txt'.
// Nota: Esta función tambien actualiza las siguientes variables.
//        m_TagName  - Nombre del tag (comando) analizado.
//        m_TagpAll  - Si se salto toda la información que contenia el comando.
//<!----------------------------------------------------------------------------------->
bool CParseText::getRtfTag(int& _i, CString& Text)
  {
  static LPCSTR Skip = "<pict><info><fonttbl><stylesheet>"
                      "<colortbl><objdata><objclass><generator><xe>";

  int i = _i;                              // Peserva donde se inicia el analisis
  if( m_Txt[i]!='{' && m_Txt[i]!='\\' )     // Solo si se esta al inicio del TAG
    return false;

  if( i>0 && m_Txt[i-1]=='\\' )             /* Pecedidos de '\' no son TAG RTF        */
    return false;

  int key = -1;                             // Marcador de incio de llave abierta (grupo)
  if( m_Txt[i]=='{' )                       // Salta la llave
    {
    key = i++;                              // Indice de inicio de llave
    while( i<m_len && m_Txt[i]<=' ')        // Salta tarecos
      ++i;

    if( m_Txt[i]!='\\' )                    // Despues de la llave tiene que venir '\'
      return false;
    }

  m_SkipAll = false;                        // Inicializa bandera de saltar comando
  ++i;                                      // Salta el '\'                           
  if( m_Txt[i]=='*' )                       // \*\ indica que el tag se puede ignorar 
    {
    ++i;                                    // Salta el * 
    while( i<m_len && m_Txt[i]<=' ')        // Salta posibles tarecos
      ++i;

    if( m_Txt[i]!='\\' )                    // Tiene que ser '\*\'       
      return false;

    ++i;                                    // Salta el '\'                           
    m_SkipAll = (key!=-1);                  // Pone bandera solo si es un grupo
    }        

  int j=i; 
  for(; isc_alfa(m_Txt[j]); ++j );          // Salta nombre del comando (letras)
  if( j==i )                                // Nombre de comando vacio ?
    return false;                           // Retorna

  m_TagName = CString(m_Txt+i, j-i);        // Copia el nombre del comando
  i = j;                                    // Apunta al final del nombre

  if( m_Txt[i]=='-' && isc_num(m_Txt[i+1]) )// Signo negativo del parametro
    ++i;                                    // lo salta

  while( isc_num(m_Txt[i]) ) ++i;           // Salta los numeros (Parametro)
  
  m_TagValue = CStringA(m_Txt + j, i-j);    // Copia el valor del comando
  
  if( m_Txt[i]==' ' )                       // Termino con un espacio
    ++i;                                    // Salta el espacio

  if( !m_SkipAll && key!=-1 )               // Hay que determinar si se salta al final?
    {
    CString fTag = '<'+m_TagName+'>';       // Pone un separador para buscar

    m_SkipAll = (strstr(Skip,fTag.c_str())!=NULL);  // Verdadero si esta en la lista
    }

  if( m_SkipAll  )                          // Hay que saltar hasta final del grupo?
    FindMatch( i, '{', '}' );               // Busca fin del grupo
    
  CString sTag( m_Txt+_i, i-_i );           // Coje el contenido completo del TAG

  _i = i-1;                                 // Actualiza el puntero al ultimo caracter

  Text += sTag;                             // Adicina el TAG a la informacion acumulada

  return true;
  }

/*------------------------------------------------------------------------------------*/
// Encuentra el caracter 'cEnd' que corresponde un caracter 'cIni' que esta delante de
// 'i', teniendo en cuenta que la pareja de caracteres pueden estar anidados
//<!----------------------------------------------------------------------------------->
bool CParseText::FindMatch(int& _i, char cIni, char cEnd)
  {
  int Match = 1;                          // Supone que ya se paso por un 'cIni'
  for( int i=_i; i<m_len; ++i )           // Recorre todos los caracteres despues de _i
    {
    if( m_Txt[i] == '\\'  )               // Si es una palabra de control
      {
      ++i;
      if( m_Txt[i]=='b' || m_Txt[i]=='i' || m_Txt[i]=='n' )  // Es bin
        {
        CString sNum;
        for( i+=3; isc_num(m_Txt[i]); ++i )   // Obtine el numero de bytes
          sNum += m_Txt[i];

        i += atoi(sNum.c_str());                  // Salta todos los bytes
        }
      }

    if( m_Txt[i] == cIni )                // Caracter incial
      ++Match;                            // Aumenta contador

    if( m_Txt[i] == cEnd )                // Caracter final
      --Match;                            // Disminulle contador

    if( Match==0 )                        // Igual la cantidad de cIni y cEnd
      {
      _i = i+1;                           // Actualiza apuntador
      return true;                        // Retorna OK
      }
    }

  return false;                           // No encontro macheo  
  }

/*------------------------------------------------------------------------------------*/
// Chequea que los tabs RTF queden bien deliminitados cuando se adiciona texto a Txt
//<!----------------------------------------------------------------------------------->
void CParseText::CheckEndOfRtfTab( CString &Txt )
  {
  if (!isRtf()) return;                // Solo cuando el texto es RTF

  int k=Txt.GetLength()-1;                              // Se pone al final de la cadena
  if (k<0 || Txt[k] == '\\') return;                    // Si es backslat no hace nada

  for( ;k>0 && Txt[k]>='a' && Txt[k]<='z'; --k  );      // Salta hacia atras todos los caracteres
  if( Txt[k]=='\\' )                                    // Es el inicio de un tab rtf
    Txt += ' ';                                         // Pone un espacio, para separalo del texto
  }

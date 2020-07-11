#ifndef RTF_H
#define RTF_H

#include "gtypes.h"
#include "GString.h"

#define Round(x) ((int)(x + 0.5))

#define PointsToTwips(x) (Round((x)*20))

#define NEW_LINE "\x0d\x0a"
#define SPACE " "

#define RTF_HEADER "{\\rtf1\\ansi\\ansicpg1252\\deff0\\deflang1033" << NEW_LINE
#define RTF_HEADER2 "\\viewkind1\\uc1\\pard " << NEW_LINE
#define RTF_END "}" << NEW_LINE

#define RTF_LINE "\\line"

#define RTF_NEW_PARAGRAPH "\\par\\pard" << NEW_LINE

#define RTF_NEW_PAGE "\\pagebb"

#define RTF_BOLD_START    "\\b "
#define RTF_BOLD_END      "\\b0 "

#define RTF_ITALIC_START  "\\i "
#define RTF_ITALIC_END    "\\i0 "

#define RTF_UNICODE(x) "\\uc0\\u" << x << SPACE

#define RTF_TAB(x)    "\\tx" << PointsToTwips(x) << "\\tab "

#define RTF_PAR "\\par" << SPACE
#define RTF_PARD "\\pard" << NEW_LINE

// margenes de los parrafos
#define RTF_FI(x) "\\fi" << PointsToTwips(x)
#define RTF_LI(x) "\\li" << PointsToTwips(x)
#define RTF_RI(x) "\\ri" << PointsToTwips(x)
#define RTF_SB(x) "\\sb" << PointsToTwips(x)
#define RTF_SB_VIRTUAL(x) SpaceBeforeWithNewLines(x)
//#define RTF_SB_VIRTUAL(x) "\\sb" << PointsToTwips(x)
#define SB_VIRTUAL_START "{\\sbstartx"
#define SB_VIRTUAL_END "\\sbendx"
#define RTF_SA(x) "\\sa" << PointsToTwips(x)

// alienacion de los parrafos
#define RTF_QL "\\ql"
#define RTF_QR "\\qr"
#define RTF_QC "\\qc"
#define RTF_QJ "\\qj" //"justified" (solo funciona en el Word)

// secciones
#define RTF_SECT "\\sect\\sectd\\sbknone\\headery300" << NEW_LINE
#define RTF_SECT_BREAK "\\sect\\sectd\\sbkpage\\headery300" << NEW_LINE

// headers y footers
#define RTF_HEADER_START  "{\\header "
#define RTF_HEADER_END		"}" << NEW_LINE
#define RTF_HEADER_EMPTY  "{\\header "
#define RTF_FOOTER_START  "{\\footer "
#define RTF_FOOTER_END		"}" << NEW_LINE

// fuentes
#define RTF_FONT_DEF(x,s) "{\\f" << x << "\\fnil\\fcharset0 " << s << ";}" << NEW_LINE;
#define RTF_FONT(x) "\\f" << (x) << SPACE
#define RTF_FONT_SIZE(x) "\\fs" << (Round(x))*2 << SPACE

// colores
#define RTF_COLOR_DEF(x) "\\red" << Round(255*x.r) << "\\green" << Round(255*x.g) \
                         << "\\blue" << Round(255*x.b) << ";"
#define RTF_COLOR(x)     "\\cf" << (x)+1 << SPACE

// usado en subindices y superindices
#define RTF_UP_NORMAL RTF_UP(0)
#define RTF_UP(x) "\\up" << PointsToTwips((x)/10) << SPACE

// para convertir de entero a hexadecimal
#define HEX8(x)  IntToHex8(x)
#define HEX16(x) IntToHex8(x) << IntToHex8((x) >> 8)
#define HEX32(x) HEX16(x) << HEX16((x) >> 16)

// imagenes

//WMF (Windows Meta File)
//w,h     : ancho y alto de la imagen
//wb      : cantidad de bytes que se ocupa cada fila de la imagen
//w1,h1   : ancho y alto deseado de la imagen
//bpp     : bits por pixel de la imagen
//nColors : cantidad de colores que hay en la paleta de la imagen
#define RTF_WMF_HEADER(w,h,wb,w1,h1,bpp,nColors) "{\\pict\\wmetafile8" \
    << "\\picw" << PointsToTwips((w1)*1.7639) \
    << "\\pich" << PointsToTwips((h1)*1.7639) << NEW_LINE \
    << "010009000003" << HEX32(0x46 + (nColors)*2 + (wb)*(h)/2) \
    << "0000" << HEX32(0x22 + (nColors)*2 + (wb)*(h)/2) \
    << "00000400000003010800050000000B0200000000050000000C02" \
    << HEX16(h) << HEX16(w) << "05000000070104000000" << HEX32(0x22 + (nColors)*2 + (wb)*(h)/2) \
    << "430F2000CC000000" << HEX16(h) << HEX16(w) << "00000000" \
    << HEX16(h) << HEX16(w) << "0000000028000000" << HEX32(w) \
    << HEX32(h) << "0100" << HEX16(bpp) << "00000000" << HEX32((wb)*(h)) \
    << "C40E0000C40E0000" << HEX32(nColors) <<  HEX32(nColors) << NEW_LINE

#define RTF_WMF_COLOR(x) HEX8(Round(255*x.b)) << HEX8(Round(255*x.g)) << \
		HEX8(Round(255*x.r))

#define RTF_WMF_PALETTE(r,g,b) HEX8(b) << HEX8(g) << HEX8(r) << "00"

#define RTF_WMF_END "05000000070101000000030000000000}" << NEW_LINE

//JPG
#define RTF_JPG_HEADER(wZoom,hZoom) "{\\pict\\jpegblip\\picscalex" << wZoom << \
		"\\picscaley" << hZoom << NEW_LINE

#define RTF_JPG_END "}" << NEW_LINE

//PNG
#define RTF_PNG_HEADER(wZoom,hZoom) "{\\pict\\pngblip\\picscalex" << wZoom << \
		"\\picscaley" << hZoom << NEW_LINE

#define RTF_PNG_END "}" << NEW_LINE

#define RTF_BIN(x) "\\bin" << (x) << SPACE

// tablas
#define RTF_TABLE_TOP(x) "{\\pard" << RTF_SB_VIRTUAL(x) << "\\fs10\\par}"
#define RTF_TABLE_TOP_NESTED(x) "{\\pard" << RTF_SB_VIRTUAL(x) << "\\fs10\\intbl\\par}"
//#define RTF_ROW_HEADER(x) "\\trowd\\trgaph70\\trpaddfb3\\trpaddb" << PointsToTwips(x)
//#define RTF_ROW_HEADER(x) "\\trowd\\trautofit1\\trgaph50\\trpaddfb3\\trpaddb" << PointsToTwips(x)
#define RTF_ROW_HEADER(x) "\\trowd\\trautofit1\\trgaph0\\trpaddfb3\\trpaddb" << PointsToTwips(x)

#define RTF_TABLE_LEFT(x) "\\trleft" << PointsToTwips(x)
#define RTF_CELL "\\cell "
#define RTF_NEST_CELL "\\nestcell "
#define RTF_CELL_POS(x) "\\cellx" << PointsToTwips(x)

#define RTF_ROW "\\row\\pard" << NEW_LINE
#define RTF_NEST_ROW(sRowDef) "{\\*\\nesttableprops" << sRowDef << "\\nestrow" \
				<< "}" << "\\pard\\intbl" << NEW_LINE
#define RTF_TABLE_PAR "\\intbl" << SPACE
#define RTF_TABLE_PAR_NEST(x) "\\intbl" << "\\itap" << x << SPACE

// frames
#define RTF_FRAME(x,y,w) "\\phmrg\\pvpara" << "\\posx" << PointsToTwips(x) \
		<< "\\posy" <<  PointsToTwips(y) << "\\absw" <<  PointsToTwips(w) \
		<< "\\overlay" << SPACE

// tamaño de la hoja
#define RTF_PAPER_WIDTH(x)  "\\paperw" << PointsToTwips(x)
#define RTF_PAPER_HEIGHT(x) "\\paperh" << PointsToTwips(x)

// margenes
#define RTF_MARGIN_TOP(x)    "\\margt" << PointsToTwips(x)
#define RTF_MARGIN_BOTTOM(x) "\\margb" << PointsToTwips(x)
#define RTF_MARGIN_LEFT(x)   "\\margl" << PointsToTwips(x)
#define RTF_MARGIN_RIGHT(x)  "\\margr" << PointsToTwips(x)

#define MIN_MARG_TOP    25
#define MIN_MARG_BOTTOM 25
#define MAX_MARG_TOP    120
#define MAX_MARG_BOTTOM 120

#define TIMES_NEW_ROMAN "Times New Roman"
#define COURIER_NEW     "Courier New"
#define DEFAULT_FONT    TIMES_NEW_ROMAN

void EncodeRtfChars(GString *s, char c);

char* IntToHex8(int x);

// elimina de s la primera ocurrencia del tag SB
GString* removeFirstSB(GString* s, long* delPos = NULL);

// ajusta el valor de la primera ocurrencia del tag SB 
GString* adjustFirstSB(GString* s, double newValue);

GString* replaceFirstTag(GString* s, char* tag, char* value);

GString SpaceBeforeWithNewLines(double points);

#endif
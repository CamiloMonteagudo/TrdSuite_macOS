
#include "ImgLib.h"
#include "png.h"
#include "rtf.h"
#include "jinclude.h"
#include "jpeglib.h"

void img_write_hex_data(png_structp png_ptr, png_bytep data, png_size_t length);
void img_write_bin_data(png_structp png_ptr, png_bytep data, png_size_t length);
void skip_input_data (j_decompress_ptr cinfo, long num_bytes);
boolean empty_output_buffer (j_compress_ptr cinfo);
void init_destination (j_compress_ptr cinfo);
void term_destination (j_compress_ptr cinfo);
void term_source (j_decompress_ptr cinfo);
void init_source (j_decompress_ptr cinfo);
boolean fill_input_buffer (j_decompress_ptr cinfo);


// funciones call-back que llenan el contenido de la imagen resultante.
// llenar la imagen en hexadecimal
void img_write_hex_data(png_structp png_ptr,
		png_bytep data, png_size_t length)
{
	GString* sImage = (GString*)png_get_io_ptr(png_ptr);
	// la optimizacion de esta parte no parece ser significativa
	for(int i=0; i<(int)length; i++)
		*sImage << HEX8(data[i]);
}

// llenar la imagen en binario
void img_write_bin_data(png_structp png_ptr,
		png_bytep data, png_size_t length)
{
	GString* sImage = (GString*)png_get_io_ptr(png_ptr);
	sImage->append((char*)data, length);
}

int	CreatePNG(int width,int height,int bpp,Guchar** rows,GString* sImage, GBool dumpBinaryImg)
{
	png_structp png_ptr;
	png_infop info_ptr;
	png_infop end_info_ptr;

  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
     NULL, NULL, NULL);

  info_ptr = png_create_info_struct(png_ptr);
  end_info_ptr = png_create_info_struct(png_ptr);


	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_info_struct(png_ptr, &info_ptr);
		png_destroy_write_struct(&png_ptr, &end_info_ptr);
		return (-1);
	}

	//crear el buffer con un tamaño inicial igual a 1/8 del tamaño de la imagen
	int total_bytes = ((width*height*bpp)/8)/8;
	if (!dumpBinaryImg)
		total_bytes = total_bytes * 2;

	sImage->reservSize(total_bytes);

	// establecer la funcion de escritura y el parametro
	if (dumpBinaryImg)
		png_set_write_fn(png_ptr, (void *)sImage, img_write_bin_data, NULL);
	else
		png_set_write_fn(png_ptr, (void *)sImage, img_write_hex_data, NULL);

	if (bpp == 24)
		png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB,
				PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	else if (bpp == 1)
		png_set_IHDR(png_ptr, info_ptr, width, height, 1, PNG_COLOR_TYPE_GRAY,
				PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	else
		return (-1);

	png_write_info(png_ptr, info_ptr);

	png_write_image(png_ptr, rows);

	png_write_end(png_ptr, end_info_ptr);


  png_destroy_info_struct(png_ptr, &end_info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);

	return 1;
}


// *****************************************************************
// *************** Funciones para tratar los JPG *******************
// *****************************************************************


struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  //(*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

// callbacks usados para leer un jpg que esta en memoria

typedef struct {
  struct jpeg_source_mgr pub;	/* public fields */

  GString *srcImage;		/* source stream */
  boolean start_of_file;	/* have we gotten any data yet? */

} my_source_mgr;

typedef my_source_mgr * my_src_ptr;


void init_source (j_decompress_ptr cinfo)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;

  src->start_of_file = TRUE;
}

boolean fill_input_buffer (j_decompress_ptr cinfo)
{
	const JOCTET FAKE_EOI[] = {0xFF,JPEG_EOI};
  my_src_ptr src = (my_src_ptr) cinfo->src;

  if (src->start_of_file == FALSE) {
    /* Insert a fake EOI marker */
		src->pub.next_input_byte = FAKE_EOI;
		src->pub.bytes_in_buffer = 2;
  } else {
		src->pub.next_input_byte = (JOCTET*)src->srcImage->getCString();
		src->pub.bytes_in_buffer = src->srcImage->getLength();
		src->start_of_file = FALSE;
	}

  return TRUE;
}

// como toda la imagen esta en memoria, esta funcion no se debe llamar
void skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;
  if (num_bytes > 0) {
    while (num_bytes > (long) src->pub.bytes_in_buffer) {
      num_bytes -= (long) src->pub.bytes_in_buffer;
      (void) fill_input_buffer(cinfo);
    }
    src->pub.next_input_byte += (size_t) num_bytes;
    src->pub.bytes_in_buffer -= (size_t) num_bytes;
  }
}

void term_source (j_decompress_ptr cinfo)
{
  /* nada que hacer aqui */
}

// callbacks usados para escribir el jpg hacia memoria

#define OUTPUT_BUF_SIZE  4096

typedef struct {
  struct jpeg_destination_mgr pub;

  GString *dstImage;
  JOCTET *buffer;
} my_destination_mgr;

typedef my_destination_mgr *my_dest_ptr;

void init_destination (j_compress_ptr cinfo)
{
  my_dest_ptr dest = (my_dest_ptr) cinfo->dest;

  /* Allocate the output buffer --- it will be released when done with image */
  dest->buffer = (JOCTET *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				  OUTPUT_BUF_SIZE * SIZEOF(JOCTET));

  dest->pub.next_output_byte = dest->buffer;
  dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}


boolean empty_output_buffer (j_compress_ptr cinfo)
{
  my_dest_ptr dest = (my_dest_ptr) cinfo->dest;

	dest->dstImage->append((char*)dest->buffer, OUTPUT_BUF_SIZE);

  dest->pub.next_output_byte = dest->buffer;
  dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;

  return TRUE;
}

void term_destination (j_compress_ptr cinfo)
{
  my_dest_ptr dest = (my_dest_ptr) cinfo->dest;
  size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

  /* Write any data remaining in the buffer */
  if (datacount > 0) {
		dest->dstImage->append((char*)dest->buffer, datacount);
  }
}

int flipJPEG(GString *srcImage, GString* dstImage)
{
  struct jpeg_decompress_struct srcinfo;
  struct jpeg_compress_struct dstinfo;
	my_source_mgr src;
	my_destination_mgr dst;
  struct my_error_mgr jerr;
  JSAMPARRAY buffer;		/* Output row buffer */
  int row_withB;		/* ancho de una fila en bytes */
	char* imgBuff = NULL;

  /* We set up the normal JPEG error routines, then override error_exit. */
  srcinfo.err = jpeg_std_error(&jerr.pub);
  dstinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;

  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    jpeg_destroy_decompress(&srcinfo);
		jpeg_create_compress(&dstinfo);

		if (imgBuff)
			delete imgBuff;

    return 0;
  }

  jpeg_create_decompress(&srcinfo);
  jpeg_create_compress(&dstinfo);

	// llenar los datos de la estructura que provee los datos
	src.srcImage = srcImage;
	src.pub.bytes_in_buffer = 0;
	src.pub.init_source = init_source;
	src.pub.fill_input_buffer = fill_input_buffer;
	src.pub.skip_input_data = skip_input_data;
	src.pub.term_source = term_source;

	srcinfo.src = (jpeg_source_mgr*)&src;

	// llenar los datos de la estructura que provee los datos
	
	dst.dstImage = dstImage;
	dst.pub.init_destination = init_destination;
	dst.pub.empty_output_buffer = empty_output_buffer;
	dst.pub.term_destination = term_destination;

	dstinfo.dest = (jpeg_destination_mgr*)&dst;

  jpeg_read_header(&srcinfo, TRUE);

  jpeg_start_decompress(&srcinfo);

  row_withB = srcinfo.output_width * srcinfo.output_components;

	imgBuff = new char[row_withB * srcinfo.output_height];

  buffer = (*srcinfo.mem->alloc_sarray)
		((j_common_ptr) &srcinfo, JPOOL_IMAGE, row_withB, 1);

	// hacer el ciclo para descomprimir la imagen y guardarla invertida en imgBuff
  while (srcinfo.output_scanline < srcinfo.output_height) {
    jpeg_read_scanlines(&srcinfo, buffer, 1);

		memcpy(imgBuff+(srcinfo.output_height-srcinfo.output_scanline) * row_withB, 
				buffer[0], row_withB);
  }

	// copiar parametros de la imagen original a la de destino
	dstinfo.image_width = srcinfo.output_width;
  dstinfo.image_height = srcinfo.output_height;
  dstinfo.input_components = srcinfo.output_components;
  dstinfo.in_color_space = srcinfo.out_color_space;
  jpeg_set_defaults(&dstinfo);
  jpeg_set_quality(&dstinfo, 90, TRUE);

  jpeg_start_compress(&dstinfo, TRUE);

	// hacer el ciclo para volver a comprimir la imagen
	JSAMPROW row_pointer[1];

  while (dstinfo.next_scanline < dstinfo.image_height) {
    row_pointer[0] = (JSAMPROW)&imgBuff[dstinfo.next_scanline * row_withB];
    jpeg_write_scanlines(&dstinfo, row_pointer, 1);
  }

  jpeg_finish_compress(&dstinfo);
  jpeg_destroy_compress(&dstinfo);

  jpeg_finish_decompress(&srcinfo);
  jpeg_destroy_decompress(&srcinfo);

	if (imgBuff)
		delete[] imgBuff;

  return 1;
}

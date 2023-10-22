/**
 * @file bitmapUtils.c
 * @author simone maschio (simonemaschio01@gmail.com)
 * @brief  These functions are used to convert images from JPG to BMP and to create the BMP header.
 *         Others functions are used to convert images from RGB565 to RGB888 and to create the BMP header.
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */
// ============================================= CODE ==============================================

#include <bitmapUtils.h>
#include <esp_jpg_decode.h>
#include <string.h>

//========================================= == IMPORTANT ===========================================
/*
To properly use the malloc function, you need to configure the memory allocation settings
This will allow the malloc function to allocate memory in the PSRAM instead of the DRAM.
To do this, you need to follow this steps:
1) Open the menuconfig (SDK configuration tool) 
2) Go to ESP PSRAM
3) Enable Support for external, SPI-connected RAM
4) Set Spi Ram access method to : "Make ram allocatable using malloc() as well"

This is how th sdkconfig file should look like: (this is only the relevant part)
#
# SPI RAM config
#
CONFIG_SPIRAM_MODE_QUAD=y
CONFIG_SPIRAM_TYPE_AUTO=y
# CONFIG_SPIRAM_TYPE_ESPPSRAM16 is not set
# CONFIG_SPIRAM_TYPE_ESPPSRAM32 is not set
# CONFIG_SPIRAM_TYPE_ESPPSRAM64 is not set
# CONFIG_SPIRAM_SPEED_40M is not set
CONFIG_SPIRAM_SPEED_80M=y
CONFIG_SPIRAM_SPEED=80
CONFIG_SPIRAM_BOOT_INIT=y
# CONFIG_SPIRAM_IGNORE_NOTFOUND is not set
# CONFIG_SPIRAM_USE_MEMMAP is not set
# CONFIG_SPIRAM_USE_CAPS_ALLOC is not set
CONFIG_SPIRAM_USE_MALLOC=y
CONFIG_SPIRAM_MEMTEST=y
CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL=16384
# CONFIG_SPIRAM_TRY_ALLOCATE_WIFI_LWIP is not set
CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL=32768
# CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY is not set
# CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY is not set
CONFIG_SPIRAM_CACHE_WORKAROUND=y


Extra: Consider that some malloc operate on pointers that are returned as a result of the function.
So when you use a function that calls a malloc, but not a free, you need to free the pointer returned
by the function. For example:
uint8_t * ptr = NULL;
function(&ptr);
--- do something with ptr ---
free(ptr);

*/
//==================================================================================================

// tag used for ESP_LOGx functions
static const char *TAG = "bitmapUtils";

// BMP header length
static const int BMP_HEADER_LEN = 54;

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief struct used to create the BMP header for the image.
 */
typedef struct {
	uint32_t filesize; // file size in bytes
	uint32_t reserved; // 0
	uint32_t fileoffset_to_pixelarray; // 54
	uint32_t dibheadersize; // 40
	int32_t width; // width in pixels
	int32_t height; // height in pixels
	uint16_t planes; // 1 (color planes) 
	uint16_t bitsperpixel; // bits per pixel 16 for rgb565 24 for rgb888
	uint32_t compression; // 0 (uncompressed)
	uint32_t imagesize; // size of pixel data
	uint32_t ypixelpermeter; // 0x0B13
	uint32_t xpixelpermeter; // 0x0B13
	uint32_t numcolorspallette; // 0 or 256
	uint32_t mostimpcolor; // 0
} bmp_header;

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief struct used to decode the JPG image.
 */
typedef struct {
	uint16_t width; // image width
	uint16_t height; // image height
	uint16_t data_offset; // data offset
	const uint8_t *input; // input data
	uint8_t *output; // output data
} rgb_jpg_decoder;

/*------------------------------------------------------------------------------------------------*/
// static function used to read the JPG image.
static unsigned int _jpg_read(void * arg, size_t index, uint8_t *buf, size_t len)
{
	// create a pointer to the decoder struct 
	rgb_jpg_decoder * jpeg = (rgb_jpg_decoder *)arg;
	if(buf) 
	{
		// copy the data to the buffer if the buffer is not null
		memcpy(buf, jpeg->input + index, len);
	}
	return len;
}

/*------------------------------------------------------------------------------------------------*/
// static function used to write the RGB image.
static bool _rgb_write(void * arg, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data)
{
	// create a pointer to the decoder struct
	rgb_jpg_decoder * jpeg = (rgb_jpg_decoder *)arg;
	// if data is null, this is the start or end of the image
	if(!data){
		// if x and y are 0, this is the start of the image
		if(x == 0 && y == 0)
		{
			// write start
			jpeg->width = w;
			jpeg->height = h;
			// if output is null, this is BMP
			if(!jpeg->output)
			{
				// allocate memory for the output image (RGB) 
				jpeg->output = (uint8_t *)malloc((w*h*3)+jpeg->data_offset);
				if(!jpeg->output)
				{
					return false;
				}
			}
		} 
		else 
		{
			//write end
		}
		return true;
	}

	// calculate the image width in bytes (3 bytes per pixel)
	size_t jw = jpeg->width*3;
	// calculate start and end of the image 
	size_t t = y * jw;
	size_t b = t + (h * jw);
	size_t l = x * 3;
	// create a pointer to the output image
	uint8_t *out = jpeg->output+jpeg->data_offset;
	uint8_t *o = out;
	size_t iy, ix;
	w = w * 3;

	// loop through the image and copy the data to the output image
	for(iy=t; iy<b; iy+=jw) 
	{
		o = out+iy+l;
		for(ix=0; ix<w; ix+= 3) 
		{
			o[ix] = data[ix+2];
			o[ix+1] = data[ix+1];
			o[ix+2] = data[ix];
		}
		data+=w;
	}
	return true;
}

/*------------------------------------------------------------------------------------------------*/
// static function used to write the RGB565 image.
static bool _rgb565_write(void * arg, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data)
{
	// create a pointer to the decoder struct
	rgb_jpg_decoder * jpeg = (rgb_jpg_decoder *)arg;
	// if data is null, this is the start or end of the image
	if(!data){
		//if x and y are 0, this is the start of the image
		if(x == 0 && y == 0)
		{
			//write start
			jpeg->width = w;
			jpeg->height = h;
			//if output is null, this is BMP
			if(!jpeg->output)
			{
				//allocate memory for the output image (RGB565)
				jpeg->output = (uint8_t *)malloc((w*h*3)+jpeg->data_offset);
				if(!jpeg->output)
				{
					return false;
				}
			}
		} 
		else 
		{
			//write end
		}
		return true;
	}

	size_t jw = jpeg->width*3;
	size_t jw2 = jpeg->width*2;
	size_t t = y * jw;
	size_t t2 = y * jw2;
	size_t b = t + (h * jw);
	size_t l = x * 2;
	// create a pointer to the output image
	uint8_t *out = jpeg->output+jpeg->data_offset;
	uint8_t *o = out;
	size_t iy, iy2, ix, ix2;

	w = w * 3;

	// loop through the image and copy the data to the output image
	for(iy=t, iy2=t2; iy<b; iy+=jw, iy2+=jw2) 
	{
		o = out+iy2+l;
		for(ix2=ix=0; ix<w; ix+= 3, ix2 +=2) 
		{
			uint16_t r = data[ix];
			uint16_t g = data[ix+1];
			uint16_t b = data[ix+2];
			uint16_t c = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
			o[ix2+1] = c>>8;
			o[ix2] = c&0xff;
		}
		data+=w;
	}
	return true;
}

/*------------------------------------------------------------------------------------------------*/

bool jpg2rgb_565(const uint8_t *src, size_t src_len, uint8_t ** out, size_t * out_len, size_t * out_width, size_t * out_height, jpg_scale_t scale)
{
	// create a struct to decode the JPG image and set the values
	rgb_jpg_decoder jpeg;
	jpeg.width = 0;
	jpeg.height = 0;
	jpeg.input = src;
	jpeg.output = *out;
	jpeg.data_offset = 0;

	// decode the JPG image and return false if it fails
	if(esp_jpg_decode(src_len, scale, _jpg_read, _rgb565_write, (void*)&jpeg) != ESP_OK)
	{
		return false;
	}

	// calculate the output size
	size_t output_size = jpeg.width*jpeg.height*2;
	*out_len = output_size;
	// set the output width and height
	*out_width = jpeg.width;
	*out_height = jpeg.height;
	return true;
}

/*------------------------------------------------------------------------------------------------*/

bool jpg2rgb_888(const uint8_t *src, size_t src_len, uint8_t ** out, size_t * out_len, size_t * out_width, size_t * out_height, jpg_scale_t scale)
{
	// create a struct to decode the JPG image and set the values
	rgb_jpg_decoder jpeg;
	jpeg.width = 0;
	jpeg.height = 0;
	jpeg.input = src;
	jpeg.output = *out;
	jpeg.data_offset = 0;

	// decode the JPG image and return false if it fails
	if(esp_jpg_decode(src_len, scale, _jpg_read, _rgb_write, (void*)&jpeg) != ESP_OK)
	{
		return false;
	}

	// calculate the output size
	size_t output_size = jpeg.width*jpeg.height*3;
	*out_len = output_size;
	// set the output width and height
	*out_width = jpeg.width;
	*out_height = jpeg.height;
	return true;
}

/*------------------------------------------------------------------------------------------------*/

bool jpg2bmp(const uint8_t *src, size_t src_len, uint8_t ** out, size_t * out_len)
{
	// create a struct to decode the JPG image and set the values
	rgb_jpg_decoder jpeg;
	jpeg.width = 0;
	jpeg.height = 0;
	jpeg.input = src;
	jpeg.output = NULL;
	jpeg.data_offset = BMP_HEADER_LEN;

	// decode the JPG image and return false if it fails
	if(esp_jpg_decode(src_len, JPG_SCALE_NONE, _jpg_read, _rgb_write, (void*)&jpeg) != ESP_OK)
	{
		return false;
	}

	// calculate the output size
	size_t output_size = jpeg.width*jpeg.height*3;

	// Instantiate the BMP header and set the values
	jpeg.output[0] = 'B';
	jpeg.output[1] = 'M';
	bmp_header * bitmap  = (bmp_header*)&jpeg.output[2];
	bitmap->reserved = 0;
	bitmap->filesize = output_size+BMP_HEADER_LEN;
	bitmap->fileoffset_to_pixelarray = BMP_HEADER_LEN;
	bitmap->dibheadersize = 40;
	bitmap->width = jpeg.width;
	bitmap->height = -jpeg.height;//set negative for top to bottom
	bitmap->planes = 1;
	bitmap->bitsperpixel = 24;
	bitmap->compression = 0;
	bitmap->imagesize = output_size;
	bitmap->ypixelpermeter = 0x0B13 ; //2835 , 72 DPI
	bitmap->xpixelpermeter = 0x0B13 ; //2835 , 72 DPI
	bitmap->numcolorspallette = 0;
	bitmap->mostimpcolor = 0;

	// set the output and output length
	*out = jpeg.output;
	*out_len = output_size+BMP_HEADER_LEN;

	return true;
}

/*------------------------------------------------------------------------------------------------*/

bool frm2bmp(uint8_t *src, size_t src_len, uint16_t width, uint16_t height, pixformat_t format, uint8_t ** out, size_t * out_len)
{
	// if the format is JPG, use the jpg2bmp function to convert the image to BMP
	if(format == PIXFORMAT_JPEG) 
	{
		return jpg2bmp(src, src_len, out, out_len);
	}
	
	// initialize the output and output length
	*out = NULL;
	*out_len = 0;

	int pix_count = width*height;

	// With BMP, 8-bit greyscale requires a palette.
	// For a 640x480 image though, that's a savings
	// over going RGB-24.
	int bpp = (format == PIXFORMAT_GRAYSCALE) ? 1 : 3;
	int palette_size = (format == PIXFORMAT_GRAYSCALE) ? 4 * 256 : 0;

	// Calculate the output size and allocate memory for the output
	size_t out_size = (pix_count * bpp) + BMP_HEADER_LEN + palette_size;
	uint8_t * out_buf = (uint8_t *)malloc(out_size);
	// if the memory allocation fails, return false
	if(!out_buf) 
	{
		ESP_LOGE(TAG, "malloc failed! %u", out_size);
		return false;
	}

  // Instantiate the BMP header and set the values
	out_buf[0] = 'B';
	out_buf[1] = 'M';
	bmp_header * bitmap  = (bmp_header*)&out_buf[2];
	bitmap->reserved = 0;
	bitmap->filesize = out_size;
	bitmap->fileoffset_to_pixelarray = BMP_HEADER_LEN + palette_size;
	bitmap->dibheadersize = 40;
	bitmap->width = width;
	bitmap->height = -height;//set negative for top to bottom
	bitmap->planes = 1;
	bitmap->bitsperpixel = bpp * 8;
	bitmap->compression = 0;
	bitmap->imagesize = pix_count * bpp;
	bitmap->ypixelpermeter = 0x0B13 ; //2835 , 72 DPI
	bitmap->xpixelpermeter = 0x0B13 ; //2835 , 72 DPI
	bitmap->numcolorspallette = 0;
	bitmap->mostimpcolor = 0;

	// Create the palette and pixel buffers
	uint8_t * palette_buf = out_buf + BMP_HEADER_LEN;
	uint8_t * pix_buf = palette_buf + palette_size;
	uint8_t * src_buf = src;

	// Create the palette if needed
	if (palette_size > 0) {
		// Grayscale palette
		for (int i = 0; i < 256; ++i) 
		{
			for (int j = 0; j < 3; ++j) 
			{
				*palette_buf = i;
				palette_buf++;
			}
			// Reserved / alpha channel.
			*palette_buf = 0;
			palette_buf++;
		}
	}

	// if the format is RGB888, copy the data to the pixel buffer
	if(format == PIXFORMAT_RGB888) 
	{
		memcpy(pix_buf, src_buf, pix_count*3);
	} 

	// if the format is RGB565, convert the data to RGB888 and copy it to the pixel buffer
	else if(format == PIXFORMAT_RGB565) 
	{
		int i;
		uint8_t hb, lb;
		for(i=0; i<pix_count; i++) 
		{
			hb = *src_buf++;
			lb = *src_buf++;
			*pix_buf++ = (lb & 0x1F) << 3;
			*pix_buf++ = (hb & 0x07) << 5 | (lb & 0xE0) >> 3;
			*pix_buf++ = hb & 0xF8;
		}
	}
	// if the format is GRAYSCALE, copy the data to the pixel buffer
	else if(format == PIXFORMAT_GRAYSCALE) 
	{
		memcpy(pix_buf, src_buf, pix_count);
	} 
	// YUV422 is not supported yet so if the format is YUV422, return false
	else if(format == PIXFORMAT_YUV422) 
	{
		ESP_LOGE(TAG, "YUV422 not supported yet!");
		free(out_buf);
		return false;
	}
	// no other formats are supported so return false
	else 
	{
		ESP_LOGE(TAG, "Unsupported format: %d", format);
		free(out_buf);
		return false;
	}

	// set the output and output length
	*out = out_buf;
	*out_len = out_size;
	return true;
}

/*------------------------------------------------------------------------------------------------*/

bool frame2bmp(camera_fb_t * fb, uint8_t ** out, size_t * out_len)
{
	// return the result of the frm2bmp function (wrapper for jpg2bmp and rgb2bmp)
	return frm2bmp(fb->buf, fb->len, fb->width, fb->height, fb->format, out, out_len);
}

/*------------------------------------------------------------------------------------------------*/



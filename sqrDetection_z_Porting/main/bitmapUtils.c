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

// tag used for ESP_LOGx functions
static const char *TAG = "bitmapUtils";

static const int BMP_HEADER_LEN = 54;

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief struct used to create the BMP header for the image.
 * 
 */
typedef struct {
	uint32_t filesize; // file size in bytes
	uint32_t reserved; // 0
	uint32_t fileoffset_to_pixelarray; // 54
	uint32_t dibheadersize; // 40
	int32_t width; // width in pixels
	int32_t height; // height in pixels
	uint16_t planes; // 1 (color planes) 
	uint16_t bitsperpixel; // bits per pixel (16) for rgb565
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
 * 
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

bool jpg2rgb565(const uint8_t *src, size_t src_len, uint8_t * out, jpg_scale_t scale)
{
	// create a struct to decode the JPG image and set the values
	rgb_jpg_decoder jpeg;
	jpeg.width = 0;
	jpeg.height = 0;
	jpeg.input = src;
	jpeg.output = out;
	jpeg.data_offset = 0;

	// decode the JPG image and return false if it fails
	if(esp_jpg_decode(src_len, scale, _jpg_read, _rgb565_write, (void*)&jpeg) != ESP_OK)
	{
		return false;
	}
	return true;
}

/*------------------------------------------------------------------------------------------------*/
/* First version of the function, not used anymore for camera_fb_t but still used for Mat 
	 this function writes the BMP header to the output buffer modifying every byte of the buffer.
	 It is not used anymore because it is not scalable, it is better to use the function below.
	 Those functions are used to convert images from RGB565 to RGB888 and to create the BMP header,
	 but they also work for JPG images and for grayscale images.
	 This allows to use the same function to create a BMP image from a camera_fb_t with any format.
	 For Mat, the function is called with the parameters of the Mat.
*/

void makebmpheader(uint8_t *pbuf, uint16_t width, uint16_t height, uint16_t bpp, uint8_t size)
{
	uint8_t headersize = size;
	uint32_t l;
	// precelar buffer
	for (int i = 0; i<headersize; i++) 
	{
		*(pbuf + i) = 0;
	}
	
	// fill in the numbers
	*(pbuf + 0) = 0x42; // B
	*(pbuf + 1) = 0x4d; // M
	
	l = (width*height*bpp) + headersize; // bmp-filesize
	*(pbuf + 2) = l;  // we have to convert the byte-order to the bmp standard!(little-endian) thats why we do it like this.
	*(pbuf + 3) = l >> 8;
	*(pbuf + 4) = l >> 16;
	*(pbuf + 5) = l >> 24;
	
	*(pbuf + 10) = headersize-1; // somehow colors are only correct if headersize. headersize-1 give correct colors??!! try and error;).maybe n-1 counting

	*(pbuf + 14) = 40; //dib-hd-size
	
	*(pbuf + 18) = width; 
	*(pbuf + 19) = width >> 8; 
	
	*(pbuf + 22) = height; 
	*(pbuf + 23) = height >> 8; 
	
	*(pbuf + 26) = 1; //planes

	*(pbuf + 28) = 16; //bitsperpixel
	*(pbuf + 30) = 3; //compression, 3 rgb-bit fields defined in the masks below
	
	l = width*height*bpp; // imagesize of raw camera image appended after the header.
	*(pbuf + 34) = l;
	*(pbuf + 35) = l >> 8;
	*(pbuf + 36) = l >> 16;
	*(pbuf + 37) = l >> 24;
	
	l = 0xf800; // red color mask. look at rgb565 description! it matches the definition of the camera rgb bitfields.
	*(pbuf + 54) = l;
	*(pbuf + 55) = l >> 8;
	
	l = 0x07e0; // green color mask
	*(pbuf + 58) = l;
	*(pbuf + 59) = l >> 8;
	
	l = 0x001f; // blue color mask
	*(pbuf + 62) = l;
	*(pbuf + 63) = l >> 8;
	// the rest is 0.
}

/*------------------------------------------------------------------------------------------------*/
// Grayscale version of the function above, not used anymore for camera_fb_t but still used for Mat
void make_grayscale_bmp_header(uint8_t *pbuf, uint16_t width, uint16_t height, uint8_t size) {
	uint8_t headersize = size; // BMP header size
	uint32_t l;

	// precelar buffer
	for (int i = 0; i<headersize; i++) 
	{
		*(pbuf + i) = 0;
	}

	// BMP header
	*(pbuf + 0) = 0x42; // B
	*(pbuf + 1) = 0x4d; // M

	l = (width*height) + headersize; // bmp-filesize
	*(pbuf + 2) = l;
	*(pbuf + 3) = l >> 8;
	*(pbuf + 4) = l >> 16;
	*(pbuf + 5) = l >> 24;

	*(pbuf + 10) = headersize-1;

	// DIB header
	*(pbuf + 14) = 40;

	*(pbuf + 18) = width; // width
	*(pbuf + 19) = width >> 8;
	*(pbuf + 20) = width >> 16;
	*(pbuf + 21) = width >> 24;

	*(pbuf + 22) = height; // height
	*(pbuf + 23) = height >> 8;
	*(pbuf + 24) = height >> 16;
	*(pbuf + 25) = height >> 24;

	*(pbuf + 26) = 1;

	*(pbuf + 28) = 8; // bits per pixel

	l = width * height; // imagesize 
	*(pbuf + 34) = l;
	*(pbuf + 35) = l >> 8;
	*(pbuf + 36) = l >> 16;
	*(pbuf + 37) = l >> 24;
	// the rest is 0.
}

/*------------------------------------------------------------------------------------------------*/

uint8_t make_fb_BMP_Header(uint8_t size, uint8_t *BMPhead)
{
	uint16_t width,height, bpp, colorfmt, framesize;
  sensor_t *s = esp_camera_sensor_get(); // get the cameras settings from camera.c driver with current settings
	colorfmt = s->pixformat;
	framesize = s->status.framesize;
	width = resolution[s->status.framesize].width;
	height = resolution[s->status.framesize].height;
	bpp = 2; //bytes per pixel
	makebmpheader(BMPhead, width, height, bpp, size);
	ESP_LOGI(TAG,"BMP Settings: w:%u h:%u framesize:%u colfmt:%u BmpMode:%d",width,height,framesize,colorfmt,1);

	return 0;
}

/*------------------------------------------------------------------------------------------------*/

uint8_t make_Mat_BMP_Header(uint8_t size, uint8_t *BMPhead, uint16_t width, uint16_t height, uint8_t bpp, bool isGray)
{
	if(!isGray)
		makebmpheader(BMPhead, width, height, bpp, size);
	else
		make_grayscale_bmp_header(BMPhead, width, height, size);
	ESP_LOGI(TAG,"BMP Settings: w:%u h:%u",width,height);
	return 0;
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


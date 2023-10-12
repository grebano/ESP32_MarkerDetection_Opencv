/**
 * @file take_picture.c
 * @author simone maschio (simonemaschio01@gmail.com)
 * @brief  These functions are used to take pictures from the camera and save them on the SD card.
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */
// ============================================= CODE ==============================================

#include <bitmapUtils.h>

// tag used for ESP_LOGx functions
static const char *TAG = "bitmapUtils";

/*------------------------------------------------------------------------------------------------*/

void makebmpheader(uint8_t *pbuf, uint16_t width, uint16_t height, uint16_t bpp, uint8_t size)
{
	int headersize = size;
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

void make_grayscale_bmp_header(uint8_t *pbuf, int width, int height, uint8_t size) {
	int headersize = size; // BMP header size
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

int make_fb_BMP_Header(uint8_t size, uint8_t *BMPhead)
{
	uint16_t width,height, bpp, colorfmt, framesize;
  sensor_t *s;
  s = esp_camera_sensor_get(); // get the cameras settings from camera.c driver with current settings
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

int make_Mat_BMP_Header(uint8_t size, uint8_t *BMPhead, uint16_t width, uint16_t height, uint8_t bpp, bool isGray)
{
	if(!isGray)
		makebmpheader(BMPhead, width, height, bpp, size);
	else
		make_grayscale_bmp_header(BMPhead, width, height, size);
	ESP_LOGI(TAG,"BMP Settings: w:%u h:%u",width,height);
	return 0;
}
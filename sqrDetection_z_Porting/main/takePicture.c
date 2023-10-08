/**
 * @file take_picture.c
 * @author simone maschio (simonemaschio01@gmail.com)
 * @brief  This file is used to take pictures from the camera and save them on the SD card.
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */
// ============================================= CODE ==============================================

#include <takePicture.h>

// tag used for ESP_LOGx functions
static const char *TAG = "take_picture";

// buffer used to store the bmp header
uint8_t BMPhead[100];
#define BMPHDSIZE 68

// camera pins
//#if ESP_CAMERA_SUPPORTED
static camera_config_t camera_config_ = {
  .pin_pwdn = CAM_PIN_PWDN,
  .pin_reset = CAM_PIN_RESET,
  .pin_xclk = CAM_PIN_XCLK,
  .pin_sccb_sda = CAM_PIN_SIOD,
  .pin_sccb_scl = CAM_PIN_SIOC,

  .pin_d7 = CAM_PIN_D7,
  .pin_d6 = CAM_PIN_D6,
  .pin_d5 = CAM_PIN_D5,
  .pin_d4 = CAM_PIN_D4,
  .pin_d3 = CAM_PIN_D3,
  .pin_d2 = CAM_PIN_D2,
  .pin_d1 = CAM_PIN_D1,
  .pin_d0 = CAM_PIN_D0,
  .pin_vsync = CAM_PIN_VSYNC,
  .pin_href = CAM_PIN_HREF,
  .pin_pclk = CAM_PIN_PCLK,

  //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
  .xclk_freq_hz = 20000000,
  .ledc_timer = LEDC_TIMER_0,
  .ledc_channel = LEDC_CHANNEL_0,

  .pixel_format = CAMERA_PIXEL_FORMAT, 
  .frame_size = CAMERA_FRAME_SIZE,    

  .jpeg_quality = 12, //0-63, for OV series camera sensors, lower number means higher quality
  .fb_count = 1,       //When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
  .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};


esp_err_t init_camera()
{
  //initialize the camera
  esp_err_t err = esp_camera_init(&camera_config_);
  gpio_set_direction(4, GPIO_MODE_OUTPUT);

  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "Camera Init Failed");
    return err;
  }

  return ESP_OK;
}
//#endif


esp_err_t initSDCard()
{
  ESP_LOGI(TAG, "Initializing SD card");

  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();



  // Options for mounting the filesystem.
  // If format_if_mount_failed is set to true, SD card will be partitioned and
  // formatted in case when mounting fails.
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = false,
      .max_files = 20};

  // Use settings defined above to initialize SD card and mount FAT filesystem.
  // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
  // Please check its source code and implement error recovery when developing
  // production applications.

  sdmmc_card_t *card;
  esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);
  
  if (ret != ESP_OK)
  {
    if (ret == ESP_FAIL)
    {
      ESP_LOGE(TAG, "Failed to mount filesystem. "
                    "If you want the card to be formatted, set format_if_mount_failed = true.");
    }
    else
    {
      ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                    "Make sure SD card lines have pull-up resistors in place.",
               esp_err_to_name(ret));
    }
  }
  else
  {
    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);
  }
  return ret;
}


bool savePicture(camera_fb_t *pic, char *picName)
{
  setbmp();

  ESP_LOGI(TAG, "Saving picture as %s", picName);
  // open file for writing
  FILE *file = fopen(picName, "wb");
  if (file == NULL)
  {
    // error opening file for writing
    ESP_LOGE(TAG, "Failed to open file for writing");
    return false;
  }
  // write the buffer to the file
  fwrite(BMPhead, 1, BMPHDSIZE, file);
  fwrite(pic->buf, 1, pic->len, file);
  fclose(file);
  ESP_LOGI(TAG, "File saved as %s", picName);
  return true;
}


camera_fb_t * takePicture()
{
  ESP_LOGI(TAG, "Taking picture...");
  camera_fb_t *pic = esp_camera_fb_get();

  // use pic->buf to access the image
  ESP_LOGI(TAG, "Picture taken! Its size was: %zu bytes", pic->len);

  return pic;
}


bool calibrateCamera()
{
  // check if camera is supported
  //#if ESP_CAMERA_SUPPORTED
    // check if camera and sd card can be initialized
    if(ESP_OK != init_camera() || ESP_OK != initSDCard()) {
        return false;
    }

    ESP_LOGI(TAG, "Calibrating...");

    // take 5 pictures to calibrate camera
    for (int i = 0; i < 2; i++)
    {
      camera_fb_t *pic = esp_camera_fb_get();

      vTaskDelay(100 / portTICK_RATE_MS);

      esp_camera_fb_return(pic);
    }
    // end of calibration
    ESP_LOGI(TAG, "Calibration done!");
    return true;
}

void makebmpheader(uint8_t *pbuf, uint16_t width, uint16_t height, uint16_t bpp)
{
	int i,headersize = BMPHDSIZE;
	uint32_t l;
	// precelar buffer
	for (i=0;i<headersize;i++) *(pbuf+i)=0;
	
	// fill in the numbers
	*pbuf=0x42; // B
	*(pbuf+1)=0x4d; // M
	
	l=(width*height*bpp)+headersize; // bmp-filesize
	*(pbuf+2)=l;  // we have to convert the byte-order to the bmp standard!(little-endian) thats why we do it like this.
	*(pbuf+3)=l>>8;
	*(pbuf+4)=l>>16;
	*(pbuf+5)=l>>24;
	
	*(pbuf+10)=headersize-1; // somehow colors are only correct if headersize. headersize-1 give correct colors??!! try and error;).maybe n-1 counting
	*(pbuf+14)=40; //dib-hd-size
	
	*(pbuf+18)=width; 
	*(pbuf+19)=width>>8; 
	
	*(pbuf+22)=height; 
	*(pbuf+23)=height>>8; 
	
	*(pbuf+26)=1; //planes
	*(pbuf+28)=16; //bitsperpixel
	*(pbuf+30)=3; //compression, 3 rgb-bit fields defined in the masks below
	
	l=width*height*bpp; // imagesize of raw camera image appended after the header.
	*(pbuf+34)=l;
	*(pbuf+35)=l>>8;
	*(pbuf+36)=l>>16;
	*(pbuf+37)=l>>24;
	
	l=0xf800; // red color mask. look at rgb565 description! it matches the definition of the camera rgb bitfields.
	*(pbuf+54)=l;
	*(pbuf+55)=l>>8;
	
	l=0x07e0; // green color mask
	*(pbuf+58)=l;
	*(pbuf+59)=l>>8;
	
	l=0x001f; // blue color mask
	*(pbuf+62)=l;
	*(pbuf+63)=l>>8;
	// the rest is 0.
	
}

int setbmp(void)
{
	uint16_t width,height, bpp, colorfmt, framesize;
  sensor_t *s;
  s = esp_camera_sensor_get(); // get the cameras settings from camera.c driver with current settings
	colorfmt = s->pixformat;
	framesize = s->status.framesize;
	width = resolution[s->status.framesize].width;
	height = resolution[s->status.framesize].height;
	bpp = 2; //bytes per pixel
	makebmpheader(BMPhead, width, height, bpp);
	ESP_LOGI(TAG,"BMP Settings: w:%u h:%u framesize:%u colfmt:%u BmpMode:%d",width,height,framesize,colorfmt,1);

	return 0;
}
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
#include <bitmapUtils.h>

// tag used for ESP_LOGx functions
static const char *TAG = "take_picture";

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
  // buffer used to store the bmp header
  uint8_t BMPhead[100];
  #define BMPHDSIZE 68
  setbmp(BMPHDSIZE, BMPhead);

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



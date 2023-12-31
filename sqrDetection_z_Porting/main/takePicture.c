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

/*------------------------------------------------------------------------------------------------*/
camera_config_t config;
// camera pins
// static camera_config_t camera_config_ = {
//   .pin_pwdn = CAM_PIN_PWDN,
//   .pin_reset = CAM_PIN_RESET,
//   .pin_xclk = CAM_PIN_XCLK,
//   .pin_sccb_sda = CAM_PIN_SIOD,
//   .pin_sccb_scl = CAM_PIN_SIOC,

//   .pin_d7 = CAM_PIN_D7,
//   .pin_d6 = CAM_PIN_D6,
//   .pin_d5 = CAM_PIN_D5,
//   .pin_d4 = CAM_PIN_D4,
//   .pin_d3 = CAM_PIN_D3,
//   .pin_d2 = CAM_PIN_D2,
//   .pin_d1 = CAM_PIN_D1,
//   .pin_d0 = CAM_PIN_D0,
//   .pin_vsync = CAM_PIN_VSYNC,
//   .pin_href = CAM_PIN_HREF,
//   .pin_pclk = CAM_PIN_PCLK,

//   //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
//   .xclk_freq_hz = 10000000,
//   .ledc_timer = LEDC_TIMER_0,
//   .ledc_channel = LEDC_CHANNEL_0,

//   .pixel_format = CAMERA_PIXEL_FORMAT, 
//   .frame_size = CAMERA_FRAME_SIZE,    

//   .jpeg_quality = 12, //0-63, for OV series camera sensors, lower number means higher quality
//   .fb_count = 2,       //When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
//   .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
// };

/*------------------------------------------------------------------------------------------------*/

void configInitCamera(pixformat_t pixel_format, framesize_t frame_size)
{
  config.pin_pwdn = CAM_PIN_PWDN;
  config.pin_reset = CAM_PIN_RESET;
  config.pin_xclk = CAM_PIN_XCLK;
  config.pin_sccb_sda = CAM_PIN_SIOD;
  config.pin_sccb_scl = CAM_PIN_SIOC;

  config.pin_d7 = CAM_PIN_D7;
  config.pin_d6 = CAM_PIN_D6;
  config.pin_d5 = CAM_PIN_D5;
  config.pin_d4 = CAM_PIN_D4;
  config.pin_d3 = CAM_PIN_D3;
  config.pin_d2 = CAM_PIN_D2;
  config.pin_d1 = CAM_PIN_D1;
  config.pin_d0 = CAM_PIN_D0;
  config.pin_vsync = CAM_PIN_VSYNC;
  config.pin_href = CAM_PIN_HREF;
  config.pin_pclk = CAM_PIN_PCLK;

  //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
  config.xclk_freq_hz = 10000000;
  config.ledc_timer = LEDC_TIMER_0;
  config.ledc_channel = LEDC_CHANNEL_0;

  config.pixel_format = pixel_format; 
  config.frame_size = frame_size;    

  config.jpeg_quality = 12; //0-63, for OV series camera sensors, lower number means higher quality
  config.fb_count = 2;       //When jpeg mode is used, if fb_count more than one, the driver will work in continuous modeconfig.
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
}

/*------------------------------------------------------------------------------------------------*/

esp_err_t init_camera (pixformat_t pixel_format, framesize_t frame_size)
{
  //initialize the camera
  configInitCamera(pixel_format, frame_size);
  esp_err_t err = esp_camera_init(&config);
  gpio_set_direction(4, GPIO_MODE_OUTPUT);
  //setCameraParams(-2, 0, 0);

  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "Camera Init Failed");
    return err;
  }

  ESP_LOGI(TAG, "Camera Init Succeed");
  return ESP_OK;
}

/*------------------------------------------------------------------------------------------------*/

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
    ESP_LOGI(TAG, "SD card mounted successfully!");
    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);
  }
  return ret;
}

/*------------------------------------------------------------------------------------------------*/

camera_fb_t * takePicture()
{
  sensor_t * s = esp_camera_sensor_get();
  ESP_LOGI(TAG, "Taking picture...");
  ESP_LOGI(TAG, "Picture format: %d", s->pixformat);
  ESP_LOGI(TAG, "Picture size: %d", s->status.framesize);
  camera_fb_t *pic = esp_camera_fb_get();

  // use pic->buf to access the image
  ESP_LOGI(TAG, "Picture taken! Its size was: %zu bytes", pic->len);

  return pic;
}

/*------------------------------------------------------------------------------------------------*/

void setCameraParams(int brightness, int contrast, int saturation)
{
  // set camera parameters
  sensor_t * s = esp_camera_sensor_get();
  s->set_brightness(s, brightness);     // -2 to 2
  s->set_contrast(s, contrast);       // -2 to 2
  s->set_saturation(s, saturation);     // -2 to 2
  s->set_special_effect(s, 0); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
  s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
  s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
  s->set_wb_mode(s, 0);        // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
  s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
  s->set_aec2(s, 0);           // 0 = disable , 1 = enable
  s->set_ae_level(s, 0);       // -2 to 2
  s->set_aec_value(s, 300);    // 0 to 1200
  s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
  s->set_agc_gain(s, 0);       // 0 to 30
  s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
  s->set_bpc(s, 0);            // 0 = disable , 1 = enable
  s->set_wpc(s, 1);            // 0 = disable , 1 = enable
  s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
  s->set_lenc(s, 1);           // 0 = disable , 1 = enable
  s->set_hmirror(s, 0);        // 0 = disable , 1 = enable
  s->set_vflip(s, 0);          // 0 = disable , 1 = enable
  s->set_dcw(s, 1);            // 0 = disable , 1 = enable
  s->set_colorbar(s, 0);       // 0 = disable , 1 = enable
}



/**
 * @file take_picture.c
 * @author simone maschio (simonemaschio01@gmail.com)
 * @brief  This file is used to take pictures from the camera and save them on the SD card.
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */
// ============================================= SETUP =============================================

// 1. Board setup (Uncomment):
// #define BOARD_WROVER_KIT
// #define BOARD_ESP32CAM_AITHINKER

// ============================================= CODE ==============================================

#include <takePicture.h>

static const char *TAG = "take_picture";

#if ESP_CAMERA_SUPPORTED
static camera_config_t camera_config = {
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

esp_err_t init_camera(void)
{
  //initialize the camera
  esp_err_t err = esp_camera_init(&camera_config);
  if (err != ESP_OK)
  {
    ESP_LOGE(TAG, "Camera Init Failed");
    return err;
  }

  return ESP_OK;
}
#endif

void takePictures(u_int16_t pictureCount)
{
  #if ESP_CAMERA_SUPPORTED
    if(ESP_OK != init_camera()) {
        return;
    }

    for(u_int16_t i = 0; i < pictureCount; i++)
    {
      ESP_LOGI(TAG, "Taking picture...");
      camera_fb_t *pic = esp_camera_fb_get();

      // use pic->buf to access the image
      ESP_LOGI(TAG, "Picture taken! Its size was: %zu bytes", pic->len);
      esp_camera_fb_return(pic);

      // save picture as .jpg
      char *picName = "ì/picture.jpg";
      ESP_LOGI(TAG, "Picture saved as %s", picName);


      // delay 5 seconds
      vTaskDelay(5000 / portTICK_RATE_MS);
    }
  #else
    ESP_LOGE(TAG, "Camera support is not available for this chip");
    return;
  #endif
}

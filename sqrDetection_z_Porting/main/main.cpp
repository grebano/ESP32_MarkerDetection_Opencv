/**
 * @file main.cpp
 * @author simone maschio (simonemaschio01@gmail.com)
 * @brief  This is the main file of the project. It contains the main function and the task that
 *         is executed by the ESP32, like taking a picture and detecting squares in it.
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */
// ============================================= CODE ==============================================

#include <esp_log.h>
#include <esp_err.h>
#include <esp_timer.h>
#include <esp_camera.h>

#include <takePicture.h>
#include <sqrDetection.hpp>
#include <device.h>
#include <detectSquares.hpp>
#include <saveUtils.hpp>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_freertos_hooks.h>


#include <iostream>
#include <map>

// tag used for ESP_LOGx functions
#define TAG "main"

// Number of squares expected in the picture
#ifndef EXPECTED_SQUARES
#define EXPECTED_SQUARES 10

// Number of pictures to take
#ifndef PIC_NUMBER
#define PIC_NUMBER 1


/**
 * PIXFORMAT_RGB565,    // 2BPP/RGB565
 * PIXFORMAT_YUV422,    // 2BPP/YUV422
 * PIXFORMAT_GRAYSCALE, // 1BPP/GRAYSCALE
 * PIXFORMAT_JPEG,      // JPEG/COMPRESSED
 * PIXFORMAT_RGB888,    // 3BPP/RGB888
 */
#define CAMERA_PIXEL_FORMAT PIXFORMAT_RGB565

/*
 * FRAMESIZE_QQVGA,    // 160x120
 * FRAMESIZE_QQVGA2,   // 128x160
 * FRAMESIZE_QCIF,     // 176x144
 * FRAMESIZE_HQVGA,    // 240x176
 * FRAMESIZE_QVGA,     // 320x240
 * FRAMESIZE_CIF,      // 400x296
 * FRAMESIZE_VGA,      // 640x480
 * FRAMESIZE_SVGA,     // 800x600
 * FRAMESIZE_XGA,      // 1024x768
 * FRAMESIZE_SXGA,     // 1280x1024
 * FRAMESIZE_UXGA,     // 1600x1200
 */
#define CAMERA_FRAME_SIZE FRAMESIZE_SVGA

extern "C" {
  void app_main(void);
}

// Main task pinned to core 0
void main_Task(void *arg);

/*------------------------------------------------------------------------------------------------*/

// initial setups and tasks creation
void app_main(void)
{
  // log
  ESP_LOGI(TAG, "Starting...");

  // Init the camera and the SD card  
  if(init_camera((pixformat_t)CAMERA_PIXEL_FORMAT, (framesize_t)CAMERA_FRAME_SIZE) != ESP_OK || initSDCard() != ESP_OK)
  {
    ESP_LOGE(TAG, "Stopping due to errors");
    return;
  }

  // Display some useful information about the system (heap left, stack high watermark)
  disp_infos();

  /* Start the tasks */
  xTaskCreatePinnedToCore(main_Task, "main", 1024 * 9, nullptr, 24, nullptr, 0);
}

/*------------------------------------------------------------------------------------------------*/

void main_Task(void *arg) 
{
  ESP_LOGI(TAG, "Starting main_task");

  // Create the base path for the pictures 
  string basePath = "/sdcard/";

  // Main loop (take a picture, save it to the SD card, detect squares)
  for (int i = 0; i < PIC_NUMBER; i++)
  {
    // Take a picture checking if the frame buffer is not NULL
    camera_fb_t* fb = takePicture();
    while (fb == NULL)
    {
      // LOG if the frame buffer is NULL and take another picture
      ESP_LOGW(TAG, "Frame buffer is NULL - taking another picture");
      fb = takePicture();
    }

    // Save the picture to the SD card 
    savePicture(fb, basePath, "COL" + to_string(i));

    // Deinit camera
    esp_camera_deinit();
    // Deinit sdcard
    esp_vfs_fat_sdmmc_unmount();
    sdmmc_host_deinit();
    // init with grayscale
    if(init_camera((pixformat_t)PIXFORMAT_GRAYSCALE, (framesize_t)CAMERA_FRAME_SIZE) != ESP_OK || initSDCard() != ESP_OK)
    {
      ESP_LOGE(TAG, "Stopping due to errors");
      return;
    }
    wait_msec(500);

    // Take a picture checking if the frame buffer is not NULL
    fb = takePicture();
    while (fb == NULL)
    {
      // LOG if the frame buffer is NULL and take another picture
      ESP_LOGW(TAG, "Frame buffer is NULL - taking another picture");
      fb = takePicture();
    }

    // Save the picture to the SD card
    savePicture(fb, basePath, "PIC" + to_string(i));
    
    // Detect squares
    extractSquares(fb, EXPECTED_SQUARES, i, "result" + to_string(i) + ".txt", false);
  
    // Release the memory of the frame buffer if is not null
    if (fb != NULL)
      esp_camera_fb_return(fb);
  }
  wait_msec(3000);
  vTaskDelete(NULL);
}

#endif // PIC_NUMBER
#endif // EXPECTED_SQUARES


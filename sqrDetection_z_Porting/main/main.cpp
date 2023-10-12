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
  if(init_camera() != ESP_OK || initSDCard() != ESP_OK)
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

  // Create the list of filenames (pic00.jpg , pic00.bmp ...)  
  string basePath = "/sdcard/";
  vector<string> photosPaths = vector<string>();
  fileNames(PIC_NUMBER,"PIC",photosPaths,".bmp");

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
    savePicture(fb, basePath, photosPaths.at(i));
    
    // Detect squares
    extractSquares(fb, EXPECTED_SQUARES, "result" + to_string(i) + ".txt", true);
  
    // Release the memory of the frame buffer
    esp_camera_fb_return(fb);
  }
  wait_msec(3000);
}

#endif // PIC_NUMBER
#endif // EXPECTED_SQUARES


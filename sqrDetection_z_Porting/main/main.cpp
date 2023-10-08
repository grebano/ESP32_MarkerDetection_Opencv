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
#include <detectSquares.hpp>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_freertos_hooks.h>


#include <iostream>
#include <map>


#define TAG "main"

#define EXPECTED_SQUARES 10
#define PIC_NUMBER 10

extern "C" {
  void app_main(void);
}

void app_main(void)
{
  // log
  ESP_LOGI(TAG, "Starting...");

  // Create the list of filenames (test0.jpg ...)  
  string basePath = "/sdcard/pic";
  vector<string> photosPaths = vector<string>();
  fileNames(PIC_NUMBER,basePath,photosPaths,".bmp");

    // Take a picture
  if(calibrateCamera())
    ESP_LOGI(TAG, "Camera calibrated");
  else{
    ESP_LOGE(TAG, "Camera not calibrated");
    return;
  }

  for (int i = 0; i < PIC_NUMBER; i++)
  {
    // Take a picture checking if the frame buffer is not NULL
    camera_fb_t* fb = takePicture();
    while (fb == NULL)
      fb = takePicture();
    
    // Save the picture to the SD card
    if(!savePicture(fb,(char *)photosPaths[i].c_str()))
      ESP_LOGE(TAG, "Error saving picture");
    else
      ESP_LOGI(TAG, "Picture saved");
    
    // Detect squares
    //extractSquares(fb, EXPECTED_SQUARES, "result" + to_string(i) + ".txt");
    
    // Release the memory of the frame buffer
    esp_camera_fb_return(fb);
  }
     

}


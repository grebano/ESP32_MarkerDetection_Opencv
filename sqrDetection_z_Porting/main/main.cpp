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
#include <sqrDetection.hpp>
#include <system.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_freertos_hooks.h>
#include <iostream>
#include <map>

//#include "system.h"

using namespace cv;

#define TAG "main"


void app_main(void)
{
  xTaskCreatePinnedToCore(demo_task, "demo", 1024 * 9, nullptr, 24, nullptr, 0);
}
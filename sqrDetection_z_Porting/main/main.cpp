#include <esp_log.h>
#include <esp_err.h>
#include <esp_timer.h>

#include <esp_camera.h>
#include <sqrDetection.hpp>

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
    ESP_LOGI(TAG, "Starting main task");
}
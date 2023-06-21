#undef EPS      // specreg.h defines EPS which interfere with opencv
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#define EPS 192

#include <esp_log.h>
#include <esp_err.h>
#include <esp_timer.h>

#include <esp_camera.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_freertos_hooks.h>
#include <iostream>
#include <map>

//#include "system.h"

using namespace cv;

extern "C" {
void app_main(void);
}


#define TAG "main"


void app_main(void)
{
    ESP_LOGI(TAG, "Starting main task");
}
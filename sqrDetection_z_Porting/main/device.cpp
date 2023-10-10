/**
 * @file device.cpp
 * @author simone maschio (simonemaschio01@gmail.com)
 * @brief  These functions are used to wait for a certain amount of time and to print some useful 
 *         information about the system.
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */
// ============================================= CODE ==============================================

#include "device.h"

#define TAG "Device"


void wait_msec(uint16_t v) {
  // delay in milliseconds 
  vTaskDelay(v / portTICK_PERIOD_MS);
}

void wait_sec(uint16_t v) {
  // delay in seconds
  vTaskDelay(v * 1000 / portTICK_PERIOD_MS);
}

void disp_infos() {
  /* Print memory information */
  // Print stack high watermark 
  ESP_LOGI(TAG, "task %s stack high watermark: %d Bytes", pcTaskGetName(NULL), (int)uxTaskGetStackHighWaterMark(NULL));
  // Print heap left
  ESP_LOGI(TAG, "heap left: %d Bytes", (int)esp_get_free_heap_size());
  // Print PSRAM infos
  ESP_LOGI(TAG, "PSRAM size: %d Bytes", (int)heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
}
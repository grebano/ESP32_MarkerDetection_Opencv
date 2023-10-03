/**
 * @file takePicture.h
 * @author simone maschio (simonemaschio01@gmail.com)
 * @brief  This file contains the function to take a picture with the ESP32-CAM and save it to the
 *         SD card with a progressive number in the name.
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */
// ============================================= CODE ==============================================

#ifndef __TAKEPICTURE_H
#define __TAKEPICTURE_H

#pragma once
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>

#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "esp_camera.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// support IDF 5.x
#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif

#ifdef __cplusplus
extern "C"{
#endif

// ============================================= SETUP =============================================
// 1. Board setup (Uncomment):
#define BOARD_ESP32CAM_AITHINKER 1
// #define BOARD_WROVER_KIT 1
// =================================================================================================


// WROVER-KIT PIN Map
#ifdef BOARD_WROVER_KIT

#define CAM_PIN_PWDN -1  //power down is not used
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK 21
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27

#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 19
#define CAM_PIN_D2 18
#define CAM_PIN_D1 5
#define CAM_PIN_D0 4
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22

#endif

// ESP32Cam (AiThinker) PIN Map
#ifdef BOARD_ESP32CAM_AITHINKER

#define CAM_PIN_PWDN 32
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK 0
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27

#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 21
#define CAM_PIN_D2 19
#define CAM_PIN_D1 18
#define CAM_PIN_D0 5
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22

#endif


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
#define CAMERA_FRAME_SIZE FRAMESIZE_CIF



/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Initialize the camera checking if it's working correctly.
 * 
 * @return esp_err_t 
 */
esp_err_t init_camera(void);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Initialize the SD card.
 * 
 * @return esp_err_t
 */
esp_err_t initSDCard(void);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Save the picture to the SD card.
 * 
 * @param camera_fb_t Picture to save.
 * @param char * Name of the picture.
 * @return true If the picture is saved correctly false otherwise.
 */
bool savePicture(camera_fb_t *pic, char * picName);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Take a picture with the ESP32-CAM and save it to the SD card with a progressive number in
 *        the name.
 * 
 */
camera_fb_t* takePicture();

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Take some pictures with the ESP32-CAM to calibrate the camera.
 * 
 * @return true If the calibration is successful false otherwise.
 */
bool calibrateCamera(void);


#if __cplusplus
}
#endif

#endif // __TAKEPICTURE_H

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
#include <stdint.h>

#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "esp_camera.h"


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



/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Initialize the camera.
 * 
 * @param pixel_format pixel format of the camera (PIXFORMAT_RGB565, PIXFORMAT_YUV422..)
 * @param frame_size frame size of the camera (FRAMESIZE_QQVGA, FRAMESIZE_QQVGA2, FRAMESIZE_QCIF...)
 */
void configInitCamera(pixformat_t pixel_format, framesize_t frame_size);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Initialize the camera checking if it's working correctly.
 * 
 * @param pixel_format pixel format of the camera (PIXFORMAT_RGB565, PIXFORMAT_YUV422..)
 * @param frame_size frame size of the camera (FRAMESIZE_QQVGA, FRAMESIZE_QQVGA2, FRAMESIZE_QCIF...)
 * 
 * @return esp_err_t 
 */
esp_err_t init_camera(pixformat_t pixel_format, framesize_t frame_size);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Initialize the SD card.
 * 
 * @return esp_err_t
 */
esp_err_t initSDCard(void);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Take a picture with the ESP32-CAM and save it to the SD card with a progressive number in
 *        the name.
 * 
 */
camera_fb_t* takePicture();

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Set the camera parameters (brightness, contrast, saturation)...
 * 
 * @param brightness brightness value (-2 to 2)
 * @param contrast contrast value (-2 to 2)
 * @param saturation saturation value (-2 to 2)
 */
void setCameraParams(int brightness, int contrast, int saturation);

/*------------------------------------------------------------------------------------------------*/

#if __cplusplus
}
#endif // __cplusplus

#endif // __TAKEPICTURE_H

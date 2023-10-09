/**
 * @file bitmap_utils.h
 * @author simone maschio (simonemaschio01@gmail.com)
 * @brief  This file contains the functions to create a bmp header 
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */
// ============================================= CODE ==============================================

#ifndef __BITMAPUTILS_H
#define __BITMAPUTILS_H

#include <stdint.h>
#include <esp_log.h>
#include <esp_camera.h>

#pragma once

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief Make bmp header for given resolution
 * 
 * @param pbuf pointer to a buffer of min size BMPHDSIZE (68)
 * @param width picture width (dividable by 4!)
 * @param height picture height/rows (any count)
 * @param bpp bytes per pixel (2 for rgb565/yuv422)
 */
void makebmpheader(uint8_t *pbuf, uint16_t width, uint16_t height, uint16_t bpp, uint8_t size);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Set the bmp header and get actual parameters.
 * 
 * @param size size of the header to set (68 for rgb565/yuv422)
 * @param BMPhead pointer to a buffer of min size BMPHDSIZE (68)
 * 
 * @return int 1 if success, 0 otherwise
 */
int setbmp(uint8_t size, uint8_t *BMPhead);

/*------------------------------------------------------------------------------------------------*/


#if __cplusplus
}
#endif

#endif // __BITMAP_UTILS_H
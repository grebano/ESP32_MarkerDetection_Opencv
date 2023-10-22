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

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Convert image buffer to BMP buffer
 *
 * @param src       Source buffer in JPEG, RGB565, RGB888, YUYV or GRAYSCALE format
 * @param src_len   Length in bytes of the source buffer
 * @param width     Width in pixels of the source image
 * @param height    Height in pixels of the source image
 * @param format    Format of the source image
 * @param out       Pointer to be populated with the address of the resulting buffer
 * @param out_len   Pointer to be populated with the length of the output buffer
 *
 * @return true on success
 */
bool frm2bmp(uint8_t *src, size_t src_len, uint16_t width, uint16_t height, pixformat_t format, uint8_t ** out, size_t * out_len);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Convert camera frame buffer to BMP buffer
 *
 * @param fb        Source camera frame buffer
 * @param out       Pointer to be populated with the address of the resulting buffer
 * @param out_len   Pointer to be populated with the length of the output buffer
 *
 * @return true on success
 */
bool frame2bmp(camera_fb_t * fb, uint8_t ** out, size_t * out_len);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Convert JPEG buffer to rgb565 buffer
 * 
 * @param src  source buffer in JPEG format
 * @param src_len  Length in bytes of the source buffer
 * @param out  Pointer to be populated with the address of the resulting buffer
 * @param out_len  Pointer to be populated with the length of the output buffer
 * @param out_width  Pointer to be populated with the width of the output buffer
 * @param out_height  Pointer to be populated with the height of the output buffer
 * @param scale  scale factor
 * 
 * @return true on success 
 */
bool jpg2rgb_565(const uint8_t *src, size_t src_len, uint8_t ** out, size_t * out_len, size_t * out_width, size_t * out_height, jpg_scale_t scale);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Convert JPEG buffer to rgb888 buffer
 * 
 * @param src  source buffer in JPEG format
 * @param src_len  Length in bytes of the source buffer
 * @param out  Pointer to be populated with the address of the resulting buffer
 * @param out_len  Pointer to be populated with the length of the output buffer
 * @param out_width  Pointer to be populated with the width of the output buffer
 * @param out_height  Pointer to be populated with the height of the output buffer
 * @param scale  scale factor
 * 
 * @return true on success 
 */
bool jpg2rgb_888(const uint8_t *src, size_t src_len, uint8_t ** out, size_t * out_len, size_t * out_width, size_t * out_height, jpg_scale_t scale);



#if __cplusplus
}
#endif

#endif // __BITMAP_UTILS_H
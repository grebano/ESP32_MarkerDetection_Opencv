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
 * @param size size of the header to set (68 for rgb565/yuv422)
 */
void makebmpheader(uint8_t *pbuf, uint16_t width, uint16_t height, uint16_t bpp, uint8_t size);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Make bmp header for given resolution
 * 
 * @param pbuf  pointer to a buffer of min size BMPHDSIZE (68)
 * @param width picture width (dividable by 4!)
 * @param height picture height/rows (any count)
 * @param size size of the header to set (68 for rgb565/yuv422)
 */
void make_grayscale_bmp_header(uint8_t *pbuf, uint16_t width, uint16_t height, uint8_t size);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Set the bmp header and get actual parameters.
 * 
 * @param size size of the header to set (68 for rgb565/yuv422)
 * @param BMPhead pointer to a buffer of min size BMPHDSIZE (68)
 * 
 * @return uint8_t 1 if success, 0 otherwise
 */
uint8_t make_fb_BMP_Header(uint8_t size, uint8_t *BMPhead);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Set the bmp header and get actual parameters.
 * 
 * @param size size of the header to set (68 for rgb565/yuv422)
 * @param BMPhead pointer to a buffer of min size BMPHDSIZE (68)
 * @param img pointer to the image to get the parameters from
 * @param bpp bytes per pixel (2 for rgb565/yuv422)
 * @param isGray true if the image is grayscale, false otherwise
 * 
 * @return uint8_t 1 if success, 0 otherwise
 */
uint8_t make_Mat_BMP_Header(uint8_t size, uint8_t *BMPhead, uint16_t width, uint16_t height, uint8_t bpp , bool isGray);

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
 * @param scale  scale factor
 * 
 * @return true on success 
 */
bool jpg2rgb565(const uint8_t *src, size_t src_len, uint8_t * out, jpg_scale_t scale);



#if __cplusplus
}
#endif

#endif // __BITMAP_UTILS_H
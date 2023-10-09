/**
 * @file device.h
 * @author simone maschio (simonemaschio01@gmail.com)
 * @brief  This file contains the functions to wait for a certain amount of time and to print some useful
 *         information about the system.
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */
// ============================================= CODE ==============================================

#ifndef __DEVICE_H
#define __DEVICE_H

#pragma once
#include <esp_log.h>
#include <esp_system.h>

#include <sdkconfig.h>

#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// support IDF 5.x
#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif 


#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief wait for a certain amount of time in milliseconds
 * 
 * @param v time to wait in milliseconds
 */
void wait_msec(uint16_t v);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief wait for a certain amount of time in seconds
 * 
 * @param v time to wait in seconds
 */
void wait_sec(uint16_t v);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief display some useful information about the system
 *        using the ESP_LOGI macro
 * 
 */
void disp_infos();

#if __cplusplus
}
#endif // __cplusplus


#endif // __DEVICE_H
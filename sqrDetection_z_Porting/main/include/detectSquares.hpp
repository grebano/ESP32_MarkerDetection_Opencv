/**
 * @file detectSquares.hpp
 * @author Simone Maschio (simonemaschio01@gmail.com)
 * @brief File that effectively runs the square detection algorithm 
 *        using the functions defined in the other files.
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */
// ============================================= CODE ==============================================

#ifndef __DETECTSQUARES_HPP
#define __DETECTSQUARES_HPP

#pragma once
#include "sqrDetection.hpp"
#include "esp_camera.h"

/**
 * @brief Function that runs the square detection algorithm.
 * 
 * @param fb Pointer to the camera frame buffer.
 * @param expectedSquares The number of squares expected in the picture.
 * @param resultFileTag The tag to use for the result file.
 */
void extractSquares(camera_fb_t * fb, int expectedSquares, string resultFileTag = string("result0.txt"));

#endif // __DETECTSQUARES_HPP
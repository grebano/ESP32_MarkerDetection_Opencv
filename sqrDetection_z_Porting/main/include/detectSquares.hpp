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

#include "sqrDetection.hpp"
#include <fstream>

/**
 * @brief Function that runs the square detection algorithm.
 * 
 * @param pictureNumber The number of the picture to be analized.
 * @param expectedSquares The number of squares expected in the picture.
 */
void extractSquares(uint16_t pictureNumber, uint16_t expectedSquares);

#endif // __DETECTSQUARES_HPP
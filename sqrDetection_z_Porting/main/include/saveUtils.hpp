/**
 * @file sqrDetection.hpp
 * @author simone maschio (simonemaschio01@gmail.com)
 * @brief  This file containd functions to save pictures and Mat objects
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */
// ============================================= CODE ==============================================

#ifndef __SAVEUTILS_HPP
#define __SAVEUTILS_HPP

#undef EPS // specreg.h defines EPS which interfere with opencv
#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#define EPS 192

#include <stdlib.h>
#include <bitmapUtils.h>
#include <esp_camera.h>
#include <esp_log.h>

/*------------------------------------------------------------------------------------------------*/
// Namesapces
using namespace std;
using namespace cv;

// ============================================= MAT ===============================================
/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Save a Mat object as a raw file
 * 
 * @param image image file to save (Mat object)
 * @param path  path where to save the image
 * @param name Name and extension of the file.
 * 
 * @return true if the image is saved correctly false otherwise 
 */
bool saveRawMat(Mat &image, string path, string name);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Save a Mat object as a bmp file
 * 
 * @param img  Mat object to save
 * @param path  path where to save the image
 * @param name  Name and extension of the file.
 * 
 * @return true if the image is saved correctly false otherwise 
 */
bool Mat2bmp(Mat &img, string path, string name);

// ============================================= PICTURE ===========================================
/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Save the picture
 * 
 * @param camera_fb_t Picture to save.
 * @param path Path where to save the picture.
 * @param name Name and extension of the file.
 * 
 * @return true If the picture is saved correctly false otherwise.
 */
bool savePicture(camera_fb_t *pic, string path, string name);

// ============================================= EXTRA ==============================================
/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Generate a list of filenames
 * 
 * @param number number of file names to generate
 * @param basename base name to reuse e.g.(photo-1,2,3,...)
 * @param destArray vector where to store the file names
 * 
 * @return std::vector<std::string> - list of file names as strings
 */
void fileNames(unsigned int number, string basename,vector<string> & destArray);


#endif // __SAVEUTILS_HPP
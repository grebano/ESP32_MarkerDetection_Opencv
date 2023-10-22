/**
 * @file detectSquares.cpp
 * @author simone maschio (simonemaschio01@gmail.com)
 * @brief  This file implements the function that runs the square detection algorithm.
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */
// ============================================= CODE ==============================================


#include <detectSquares.hpp>
#include <esp_log.h>
#include <fstream>
#include <saveUtils.hpp>
#include <esp_camera.h>

// tag used for ESP_LOGx functions
static const char *TAG = "detectSquares";

void extractSquares(camera_fb_t * fb, int expectedSquares, string resultFileTag, bool onlyCanny){

  // log
  ESP_LOGI(TAG, "Starting square detection...");
  
  // Create a Mat object
  Mat img;

  /*
  The camera_fb_t * fb is a pointer to a struct that contains the following fields:
  uint8_t * buf;        // Pointer to the pixel data
  size_t len;           // Length of the buffer in bytes
  size_t width;         // Width of the buffer in pixels
  size_t height;        // Height of the buffer in pixels
  pixformat_t format;   // Format of the pixel data

  Considering that the ESP32-CAM has a OV2640 sensor, the used formats are:
  PIXFORMAT_JPEG, PIXFORMAT_GRAYSCALE, PIXFORMAT_RGB565

  So in order to create a Mat object from the frame buffer is necessary to know the format of the
  image. This is done by checking the pixformat_t format field of the camera_fb_t * fb struct.
  */

  // JPEG format is available but is not useful for this project
  if(fb->format == PIXFORMAT_JPEG){
    ESP_LOGI(TAG, "Image format: JPEG");

    // Convert to a rgb565 image using jpg2rgb888 function
    uint8_t *rgb_image = NULL;
    size_t rgb_image_len = 0;
    size_t w = 0, h = 0;
    if(!jpg2rgb_888(fb->buf, fb->len, &rgb_image, &rgb_image_len, &w, &h, JPG_SCALE_NONE)){
      ESP_LOGE(TAG, "Conversion to rgb888 failed");
      return;
    }
    ESP_LOGI(TAG, "Image converted to rgb888");
    ESP_LOGI(TAG, "Image width: %d", w);
    ESP_LOGI(TAG, "Image height: %d", h);

    // Create a Mat object from the rgb888 image
    Mat temp(h, w, CV_8UC3, rgb_image);
    img = temp.clone();
    temp.release();
    ESP_LOGI(TAG, "Mat created");
    saveMat(img, "/sdcard/", "mat00", 3, false);
    saveRawMat(img, "/sdcard/", "mat00");

    // Convert image to greyscale
    cvtColor(img, img, COLOR_BGR2GRAY);
    ESP_LOGI(TAG, "Image converted to greyscale");
    saveMat(img, "/sdcard/", "gray00", 1, true);
    saveRawMat(img, "/sdcard/", "gray00");
  }

  // RGB565 is the default format for this project --> bmp header creation is available
  else if(fb->format == PIXFORMAT_RGB565){
    ESP_LOGI(TAG, "Image format: RGB565");

    // Create a Mat object from the frame buffer
    Mat temp(fb->height, fb->width, CV_8UC2, fb->buf);
    img = temp.clone();
    temp.release();
    ESP_LOGI(TAG, "Mat created");
    Mat2bmp(img, "/sdcard/", "mak00");
    saveMat(img, "/sdcard/", "mat00", 3, false);
    saveRawMat(img, "/sdcard/", "mat00"); 
    
    // Convert image to greyscale
    cvtColor(img, img, COLOR_BGR5652GRAY);
    ESP_LOGI(TAG, "Image converted to greyscale");
    saveMat(img, "/sdcard/", "gray00", 1, true);
    saveRawMat(img, "/sdcard/", "gray00");
  }

  // GRAYSCALE format doesn't need conversion to greyscale 
  else if(fb->format == PIXFORMAT_GRAYSCALE){
    ESP_LOGI(TAG, "Image format: GRAYSCALE");

    // Create a Mat object from the frame buffer
    Mat temp(fb->height, fb->width, CV_8UC1, fb->buf);
    img = temp.clone();
    temp.release();
    ESP_LOGI(TAG, "Mat created");
    saveMat(img, "/sdcard/", "mat00", 1, true);
    saveRawMat(img, "/sdcard/", "mat00"); 
  }

  // RGB888 bmp header cration is not complete (OV2640 does not support this format)
  else if(fb->format == PIXFORMAT_RGB888){
    ESP_LOGI(TAG, "Image format: RGB888");

    // Create a Mat object from the frame buffer
    Mat temp(fb->height, fb->width, CV_8UC3, fb->buf);
    img = temp.clone();
    temp.release();
    ESP_LOGI(TAG, "Mat created");
    //saveMat(img, "/sdcard/", "mat00", 3, false);
    saveRawMat(img, "/sdcard/", "mat00"); 

    // Convert image to greyscale
    cvtColor(img, img, COLOR_BGR2GRAY);
    ESP_LOGI(TAG, "Image converted to greyscale");
    //saveMat(img, "/sdcard/", "gray00", 1, true);
    saveRawMat(img, "/sdcard/", "gray00");
  }

  else{
    ESP_LOGE(TAG, "Image format: UNKNOWN");
    return;
  }

  // Blur image for better edge detection --> was(3,3)
  GaussianBlur(img, img, Size(3,3), 0);
  ESP_LOGI(TAG, "Image blurred");
  // Save blur output
  saveMat(img, "/sdcard/", "blur00", 1, true);
  saveRawMat(img, "/sdcard/", "blur00");


  // Apply canny edge detection --> was 30,60,3,false
  Canny(img, img, 30, 60, 3);
  ESP_LOGI(TAG, "Canny edge detection applied");
  // Dilate canny output to remove potential holes between edge segments
  dilate(img, img, Mat(), Point(-1,-1));
  ESP_LOGI(TAG, "Canny dilated");
  // Save canny output
  saveMat(img, "/sdcard/", "canny00", 1, true );
  saveRawMat(img, "/sdcard/", "canny00");

  // Check if only canny is used
  if(onlyCanny){
    return;
  }
/*
  // Create a txt file where results are saved
  ofstream outfile(resultFileTag);

  // List of squares in given image
  vector<Square> sqrList;

  // Find image contours using dedicated function
  vector<vector<Point>> contours;
  findContours(img, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

  // Loop through all the contours and get the approximate polygonal curves for each contour
  for (unsigned int i = 0; i < contours.size(); i++)
  {
    // Vector approx will contain the vertices of the polygonal approximation for the contour
    vector<Point> approx;

    // Approximate contour with accuracy proportional to the contour perimeter --> was 0.02
    approxPolyDP(Mat(contours[i]), approx, 0.03 * arcLength(contours[i], true), true);

    // Skip small or non-convex objects
    if (fabs(contourArea(contours[i])) < 400 || !isContourConvex(approx) ||
      fabs(contourArea(contours[i])) > 1700)
    {
      continue;
    }
    
    // Possible square
    if (approx.size() == 4)
    {      
      // Draw square contours
      //polylines(originalImage, approx, true, Scalar(0,0,255), 1);

      // Get square from approximated contour
      sqrList.push_back(getSquare(approx, originalImage, true));
    }
  }

  // Check if some squares are overlapped (RETR_TREE)
    for(unsigned int i=0; i<sqrList.size()-1; i++)
  {
    // If two squares are in the same spot, one is deleted
    if(areOverlapping(sqrList[i],sqrList[i+1],10))
      sqrList.erase(sqrList.begin()+i);
  }

  // Check if some squares are missing
  vector<Square> missedSquares;
  findMissingSquares(sqrList, missedSquares, expectedSquares, 10, 100);

  // Print the list of square centers and their colour
  for(unsigned int i=0; i<sqrList.size(); i++)
  {
    // Print square center and colour
    outfile << sqrList[i].center << " [" << sqrList[i].colour[0] << ","
      << sqrList[i].colour[1] << "," <<  sqrList[i].colour[2] << "]" << endl;
  }
  outfile << endl;

  outfile.close();
 */ 
}
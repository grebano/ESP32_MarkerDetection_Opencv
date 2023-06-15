/**
 * @file main.cpp
 * @author Simone Maschio (simonemaschio01@gmail.com)
 * @brief square detection 
 * @version 0.1
 * @date 2023-04-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "sqrDetection.hpp"
#include <fstream>
#include <iostream>

#define IMAGES 6
#define MARKERS 20

void extractMarkers();

int main(){

  // Extract markers from images
  extractMarkers();
  
  return 0;
}

void extractMarkers(){

  // Create the list of filenames (test0.jpg ...)
  vector<string> files = fileNames(IMAGES,string("images/test"));

  // Create a txt file where results are saved
  ofstream outfile ("results.txt");

  // Loop over input images 
  for(string file : files)
  {
    // List of squares in given image
    vector<Square> sqrList;

    // Open image file
    Mat originalImage = imread(file);
    Mat img;

    // Convert image to greyscale
    cvtColor(originalImage,img, COLOR_BGR2GRAY);

    // Blur image for better edge detection --> was(3,3)
    GaussianBlur(img, img, Size(3,3), 0);
  
    // Apply canny edge detection --> was 30,60,3,false
    Canny(img, img, 30, 60, 3, false);

    // Dilate canny output to remove potential holes between edge segments
    dilate(img, img, Mat(), Point(-1,-1));

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
        polylines(originalImage, approx, true, Scalar(0,0,255), 1);

        // Get square from approximated contour
        sqrList.push_back(getSquare(approx, originalImage, true));
      }
    }

    // Check if there are missing squares 
    if(sqrList.size()<MARKERS)
    {
      cout << "There are " << MARKERS-sqrList.size() << " missing squares" << endl;
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
    findMissingSquares(sqrList, missedSquares, MARKERS, 10, 100);

    for(Square missedSquare : missedSquares)
    {
      // Draw missing square center
      drawMarker(originalImage,missedSquare.center,Scalar(0,255,0));
    }

    // Print the list of square centers and their colour
    for(unsigned int i=0; i<sqrList.size(); i++)
    {
      // Print square center and colour
      outfile << sqrList[i].center << " [" << sqrList[i].colour[0] << ","
       << sqrList[i].colour[1] << "," <<  sqrList[i].colour[2] << "]" << endl;
    }
    outfile << endl;

    // Save processed image
    saveImage(originalImage,file);

  }
  outfile.close();
}
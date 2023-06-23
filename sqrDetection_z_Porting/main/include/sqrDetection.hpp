/**
 * @file sqrDetection.hpp
 * @author simone maschio (simonemaschio01@gmail.com)
 * @brief  This file containd functions to detect squares in an image and get their colour.
 *         Here is also defined the Square struct used to store the center and the colour of a square.
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */
// ============================================= CODE ==============================================

#ifndef __SQRDETECTION_HPP
#define __SQRDETECTION_HPP

#undef EPS // specreg.h defines EPS which interfere with opencv
#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#define EPS 192

#include <stdlib.h>

/*------------------------------------------------------------------------------------------------*/
// Namesapces
using namespace std;
using namespace cv;

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Square object with a center and a colour:
 * center is stored as a point ([x,y])
 * colour is stored in BGR ([B,G,R])
 */
struct Square
{
  Point center;
  vector<unsigned int> colour;
};

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Generate a list of filenames
 * 
 * @param number number of file names to generate
 * @param basename base name to reuse e.g.(photo-1,2,3,...)
 * @param destArray vector where to store the file names
 * @param format extension of image file
 * @return std::vector<std::string> - list of file names as strings
 */
void fileNames(unsigned int number, string basename,vector<string> & destArray, string format = string(".jpg"));

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Get the Center of a square from a list of vertices
 * 
 * @param vertices list of square vertices
 * @return cv::Point - square center as a (x,y) point 
 */
Point getCenter(vector<Point> & vertices);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Get a Square object from a list of vertices
 * 
 * @param vertices list of square vertices
 * @param image image where the square is located
 * @param highAccuracy if 0 low accuracy is used, if 1 better colour measurement is done
 * @return Square - square object with center and colour
 */
Square getSquare(vector<Point> & vertices, Mat & image, bool highAccuracy = 0);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Get the distance between two centers
 * 
 * @param center1 center of first polygon
 * @param center2 center of second polygon
 * @return int - distance between the two points
 */
int centerToCenter(Point & center1, Point & center2);

/**
 * @brief Get the distance between two squares
 * 
 * @param square1 first square
 * @param square2 second square
 * @return int - distance between the two points
 */
int centerToCenter(Square & square1, Square & square2);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Save image changing its file name 
 * 
 * @param image image that is going to be showed and saved
 * @param original_fileName original filename used to generate a new one
 */
void saveImage(Mat & image, string fileName);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Get the BGR Colour of a point in an image
 * 
 * @param image image where colour is going to be retrieved
 * @param point point of interest
 * @param bgrArray array where bgr colour is going to be stored
 * @param highAccuracy if 0 low accuracy is used, if 1 better colour measurement is done
 */
void getColour(Mat & image, Point & point, vector<unsigned int> & bgrArray, bool highAccuracy = 0);

/**
 * @brief Get the BGR Colour of a square in an image
 * 
 * @param image image where colour is going to be retrieved
 * @param sqr square in which colour is measured
 * @param highAccuracy if 0 low accuracy is used, if 1 better colour measurement is done
 */
void getColour(Mat & image, Square & sqr, bool highAccuracy = 0);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Check if two points are overlapping
 * 
 * @param center1 first point 
 * @param center2 second point
 * @param threshold distance threshold between the points
 * @return true 
 * @return false 
 */
bool areOverlapping(Point & center1, Point & center2, int threshold);

/**
 * @brief Check if two squares are overlapping
 * 
 * @param square1 first square
 * @param square2 second square
 * @param threshold distance threshold between the points
 * @return true 
 * @return false 
 */
bool areOverlapping(Square & square1, Square & square2, int threshold);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Check if there are missing squares in the list and eventually find them
 * 
 * @param foundSquares Squares that have been already found
 * @param missingSquares Squares that are going to be found
 * @param expected number of expected squares in the image
 * @param tol tolerance in average center distance measuring
 * @param maxDist max distance between centers of squares
 */
void findMissingSquares(vector<Square> & foundSquares,vector<Square> & destinationVector,
                        unsigned int expected, unsigned int tol, unsigned int maxDist);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Find a square between two existing squares following the axis
 * 
 * @param square1 first square
 * @param square2 second square
 * @return Square - Intermediate square between the two given
 */
Square intermediateSquare(Square & square1, Square & square2);

/**
 * @brief Find a poi t between two existing points following the axis
 * 
 * @param point1 first point
 * @param point2 second point
 * @return Point - Intermediate point between the two given
 */
Point intermediatePoint(Point & point1, Point & point2);

/*------------------------------------------------------------------------------------------------*/
/**
 * @brief Obtain the size of an image
 * 
 * @param image image of which size is going to be measured
 * @param width width of the image
 * @param height height of the image 
 */
void getImageSize(Mat & image, unsigned int & width, unsigned int & height);

/**
 * @brief Obtain the size of an image
 * 
 * @param image image of which size is going to be measured
 * @return vector<unsigned int> - vector containing width and height of the image
 */ 
vector<unsigned int> getImageSize(Mat & image);

/*------------------------------------------------------------------------------------------------*/

#endif // __SQRDETECTION_HPP
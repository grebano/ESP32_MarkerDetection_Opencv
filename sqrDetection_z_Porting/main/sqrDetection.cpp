/**
 * @file sqrDetection.cpp
 * @author simone maschio (simonemaschio01@gmail.com)
 * @brief This file contains the implementation of the functions defined in sqrDetection.hpp
 *        and is used to detect squares in an image and get their colour.
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */
// ============================================= CODE ==============================================

#include "sqrDetection.hpp"

/*------------------------------------------------------------------------------------------------*/

vector<string> fileNames(unsigned int number, string basename, string format)
{
  // Format filename using a base, a number and an extension
  vector<string> names;
  for(unsigned int i=0; i<number; i++)
  {
    string name = basename;
    name.append(to_string(i));
    name.append(format);
    names.push_back(name);
  }
  return names;
}

/*------------------------------------------------------------------------------------------------*/

Point getCenter(vector<Point> & vertices)
{
  if(vertices.size() == 4)
  {
    // Find delta x and delta y
    int deltaY = abs(vertices[2].y - vertices[0].y);
    int deltaX = abs(vertices[3].x - vertices[1].x);
    Point center = vertices[0];

    // Search the top left vertex
    for(unsigned int i=0; i<vertices.size(); i++)
    {
      if(vertices[i].x < center.x)
        center.x = vertices[i].x;
      if(vertices[i].y < center.y)
        center.y = vertices[i].y;
    }

    // Add delta x and delta y to starting vertex
    center.x += deltaX/2;
    center.y += deltaY/2;
    return center;
  }
  else
  {
    // Not implemented
    return vertices[0];
  }
}

/*------------------------------------------------------------------------------------------------*/

Square getSquare(vector<Point> & vertices, Mat & image, bool highAccuracy)
{
  // Create a square object
  Square square;

  // Find center of square
  square.center = getCenter(vertices);

  // Find colour of square
  getColour(image,square.center,square.colour,highAccuracy);

  return square;
}

/*------------------------------------------------------------------------------------------------*/

int centerToCenter(Point & center1, Point & center2)
{
  // Find distance between the two points (x,y)
  return sqrt(pow(center1.x-center2.x,2)+pow(center1.y-center2.y,2));
}

int centerToCenter(Square & square1, Square & square2)
{
  // Find distance between the two points (x,y)
  return centerToCenter(square1.center,square2.center);
}

/*------------------------------------------------------------------------------------------------*/

void saveImage(Mat & image, string & fileName)
{
  // Split name and extension, then add a modifier and previous extension
  string extension = fileName.substr(fileName.find_last_of("."));
  fileName.resize(fileName.find_last_of("."));
  fileName.append("_Processed_");
  fileName.append(extension);

  // Save image
  imwrite(fileName,image);
}

/*------------------------------------------------------------------------------------------------*/

void getColour(Mat & image, Point & point, vector<unsigned int> & bgrArray, bool highAccuracy)
{
  // Extract BGR colour of a single pixel
  unsigned int b = 0, g = 0, r = 0;

  if(highAccuracy)
  {
    // Calculate offset to center the square
    int offset = -2;

    // Retrieve colour of n points inside a square of nxn pixels centered on the given point
    for(unsigned int i=0; i<5; i++)
    {
      for(unsigned int j=0; j<5; j++)
      {
        // Add BGR values
        b += image.at<Vec3b>(point.y+offset+i,point.x+offset+j)[0];
        g += image.at<Vec3b>(point.y+offset+i,point.x+offset+j)[1];
        r += image.at<Vec3b>(point.y+offset+i,point.x+offset+j)[2];
      }
    }

    // Calculate average colour
    b /= 25;
    g /= 25;
    r /= 25;
  }
  else
  {
    // Retrieve colour of a single pixel
    b = image.at<Vec3b>(point.y,point.x)[0];
    g = image.at<Vec3b>(point.y,point.x)[1];
    r = image.at<Vec3b>(point.y,point.x)[2];
  }

  bgrArray.clear();
  
  bgrArray.push_back(b);
  bgrArray.push_back(g);
  bgrArray.push_back(r);
}

void getColour(Mat & image, Square & sqr, bool highAccuracy)
{
  // Wrapper function
  getColour(image, sqr.center, sqr.colour, highAccuracy);
}

/*------------------------------------------------------------------------------------------------*/

bool areOverlapping(Point & center1, Point & center2, int threshold)
{
  // Measure distance between centers and compare with given threshold
  return abs(centerToCenter(center1,center2)) < threshold;
}

bool areOverlapping(Square & square1, Square & square2, int threshold)
{
  // Wrapper function
  return areOverlapping(square1.center,square2.center,threshold);
}

/*------------------------------------------------------------------------------------------------*/

void findMissingSquares(vector<Square> & foundSquares,vector<Square> & destinationVector, 
                        unsigned int expected, unsigned int tol, unsigned int maxDist)
{
  if(foundSquares.size() < expected)
  {
    
    // Obtain minimum distance between squares and use it as a reference
    unsigned int minDistance;
    minDistance = centerToCenter(foundSquares[0],foundSquares[1]);
    // Find the minium distance between squares
    for(unsigned int i=1; i<foundSquares.size(); i++)
    {
      if(centerToCenter(foundSquares[i],foundSquares[i-1]) < minDistance)
      {
        minDistance = centerToCenter(foundSquares[i],foundSquares[i-1]);
      }
    }

    // Now find out if there are missing squares using given parameters
    unsigned int newSquares= 0;
    for(unsigned int i=1; i<foundSquares.size(); i++)
    {
      // Only add necessary squares (not more than expected)
      if((expected - (foundSquares.size() + newSquares)) > 0)
      {
        unsigned int dist = centerToCenter(foundSquares[i],foundSquares[i-1]);
        if(dist > (minDistance + tol) && dist < maxDist)
        {
          // Insert an intermediate square in the list of missed squares
          destinationVector.push_back(intermediateSquare(foundSquares[i],foundSquares[i-1]));
          newSquares++;
        }
      }
      // If no squares are still missing exit the loop
      else
        break;
    }

    // Possible missing squares near the edge of the image (not between other squares)
    // while((expected - (foundSquares.size() + newSquares)) > 0)
    // {
    // }
  }
}

/*------------------------------------------------------------------------------------------------*/

Point intermediatePoint(Point & point1, Point & point2)
{
  // return the center of a point between the two given points
  Point intermediatePoint;
  intermediatePoint.x = (point1.x + point2.x)/2;
  intermediatePoint.y = (point1.y + point2.y)/2;
  return intermediatePoint;
}

Square intermediateSquare(Square & square1, Square & square2)
{
  // return the center of a square between the two given squares
  Square intermediateSquare;
  intermediateSquare.center = intermediatePoint(square1.center,square2.center);
  return intermediateSquare;
}

/*------------------------------------------------------------------------------------------------*/

void getImageSize(Mat & image, unsigned int & width, unsigned int & height)
{
  // Get image size
  width = image.cols;
  height = image.rows;
}

vector<unsigned int> getImageSize(Mat & image)
{
  // Get image size
  vector<unsigned int> size;
  size.push_back(image.cols);
  size.push_back(image.rows);
  return size;
}

/*------------------------------------------------------------------------------------------------*/
/**
 * @file take_picture.c
 * @author simone maschio (simonemaschio01@gmail.com)
 * @brief  This file contains multiple functions that are used
 *         to save images and Mat objects.
 * @version 0.1
 * 
 * @copyright Copyright (c) 2023
 * 
 */
// ============================================= CODE ==============================================

#include <saveUtils.hpp>

// tag used for ESP_LOGx functions
static const char *TAG = "saveUtils";

// ============================================= MAT ===============================================
/*------------------------------------------------------------------------------------------------*/

bool saveMat(Mat &image, string path, string name, int bpp, bool isGray)
{

  // check if the extension is already present in the last 4 characters of the name
  if (name.substr(name.length() - 4, 4) != ".bmp")
  {
    name.append(".bmp");
  }
  // open file for writing
  string picName = path + name;
  FILE *file = fopen((char*)picName.c_str() , "wb");
  if (file == NULL) {
    ESP_LOGE(TAG, "Failed to open file for writing");
  }
  else{
    // buffer used to store the bmp header
    uint8_t BMPhead[100];
    uint8_t BMPHDSIZE = 68; 
    for(int i=0; i<100; i++)
    {   
      BMPhead[i] = 0;
    }
    make_Mat_BMP_Header(BMPHDSIZE, BMPhead, image.cols, image.rows, bpp, isGray);

    fwrite(BMPhead, 1, BMPHDSIZE, file);
    fwrite(image.data, 1, image.total() * image.elemSize(), file);
    fclose(file);
    ESP_LOGI(TAG, "File saved as %s", (char*)picName.c_str());
  }
  return 0;
}

/*------------------------------------------------------------------------------------------------*/

bool saveRawMat(Mat &image, string path, string name)
{
  // check if the extension is already present in the last 4 characters of the name
  if (name.substr(name.length() - 4, 4) != ".raw")
  {
    name.append(".raw");
  }
  // open file for writing
  string picName = path + name;
  FILE *file = fopen((char*)picName.c_str() , "wb");
  if (file == NULL) {
    ESP_LOGE(TAG, "Failed to open file for writing");
  }
  else{
    fwrite((char*)image.data, 1, image.total() * image.elemSize(), file);
    fclose(file);
    ESP_LOGI(TAG, "File saved as %s", (char*)picName.c_str());
  }
  return 0;
}

// ============================================= PICTURE ===========================================
/*------------------------------------------------------------------------------------------------*/

bool savePicture(camera_fb_t *pic, string path, string name)
{
  // save jpeg without creating the bmp header
  if(pic->format == PIXFORMAT_JPEG || pic->format == PIXFORMAT_GRAYSCALE || pic->format == PIXFORMAT_RGB565)
  {  
    // use the frame2bmp function from bitmap_utils.h
    uint8_t *bmp_buf;
    size_t bmp_buf_len;

    // convert the frame buffer to bmp
    if(frame2bmp(pic, &bmp_buf, &bmp_buf_len))
    {
      // check if the extension is already present in the last 4 characters of the name
      if (name.substr(name.length() - 4, 4) != ".bmp")
      {
        name.append(".bmp");
      }
      string picName = path + name;
      // open file for writing
      FILE *file = fopen((char*)picName.c_str(), "wb");
      if (file == NULL)
      {
        // error opening file for writing
        ESP_LOGE(TAG, "Saving Error : Failed to open file for writing");
        return false;
      }
      // write the buffer to the file
      fwrite(bmp_buf, 1, bmp_buf_len, file);
      fclose(file);
      ESP_LOGI(TAG, "File saved as %s", (char*)picName.c_str());
      return true;
    }
    else
    {
      ESP_LOGE(TAG, "Saving Error : Failed to convert frame to bmp");
      return false;
    }
  }
  // error if the format is not supported
  else
  {
    ESP_LOGE(TAG, "Unknown format");
    return false;
  }
}

// ============================================= EXTRA ==============================================
/*------------------------------------------------------------------------------------------------*/

void fileNames(unsigned int number, string basename, vector<string> & destArray)
{
  // Format filename using a base, a number and an extension;
  for(unsigned int i=0; i<number; i++)
  {
    string name = basename;
    name.append(to_string(i/10));
    name.append(to_string(i%10));
    destArray.push_back(name);
  }
}
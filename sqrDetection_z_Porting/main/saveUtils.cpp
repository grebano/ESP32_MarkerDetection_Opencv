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

bool Mat2bmp(Mat & img, string path, string name)
{
  uint8_t * bmp_buf = NULL;
  size_t bmp_buf_len = 0;

	// call the frm2bmp function with the parameters of the Mat (considering the format)
	if(img.type() == CV_8UC1)
  {
    ESP_LOGI(TAG, "Image format: GRAYSCALE");
    // use grayscale format for the bmp header
		if(!frm2bmp(img.data, img.total(), img.cols, img.rows, PIXFORMAT_GRAYSCALE, &bmp_buf, &bmp_buf_len))
    {
      ESP_LOGE(TAG, "Saving Error : Failed to convert frame to bmp");
      free(bmp_buf);
      return false;
    }
  }
	else if (img.type() == CV_8UC2)
  {
    ESP_LOGI(TAG, "Image format: RGB565");
    // use rgb565 format for the bmp header
    if(!frm2bmp(img.data, img.total(), img.cols, img.rows, PIXFORMAT_RGB565, &bmp_buf, &bmp_buf_len))
    {
      ESP_LOGE(TAG, "Saving Error : Failed to convert frame to bmp");
      free(bmp_buf);
      return false;
    }
  }
  else if (img.type() == CV_8UC3)
  {
    ESP_LOGI(TAG, "Image format: RGB");
    // use rgb888 format for the bmp header
    if(!frm2bmp(img.data, img.total(), img.cols, img.rows, PIXFORMAT_RGB888, &bmp_buf, &bmp_buf_len))
    {
      ESP_LOGE(TAG, "Saving Error : Failed to convert frame to bmp");
      free(bmp_buf);
      return false;
    }
  }
  else
  {
    // error if the format is not supported
    ESP_LOGE(TAG, "Saving Error : Unknown format");
    return false;
  }

  // save the buffer to the file
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

  // deallocate the memory used by bmp_buf
  free(bmp_buf);

  return true;  
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
    uint8_t *bmp_buf = NULL;
    size_t bmp_buf_len = 0;

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
        free(bmp_buf);
        return false;
      }
      // write the buffer to the file
      fwrite(bmp_buf, 1, bmp_buf_len, file);
      fclose(file);
      ESP_LOGI(TAG, "File saved as %s", (char*)picName.c_str());

      // deallocate the memory used by bmp_buf 
      free(bmp_buf);
      return true;
    }
    else
    {
      ESP_LOGE(TAG, "Saving Error : Failed to convert frame to bmp");
      // deallocate the memory used by bmp_buf 
      free(bmp_buf);
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
# :small_orange_diamond: ESP32_MarkerDetection_Opencv :small_orange_diamond:
In this project the goal is to detect square markers in a photo taken by the esp32-Cam. 

## :hammer_and_wrench: Hardware 	
- ESP32-CAM (AI-Thinker)
- 5V to 3.3V step down converter
- 5V power supply

## 	:computer: Software
- Espressif IDF
- Opencv C++

## :camera: Esp32-Cam
![Esp32-Cam](/Images/Esp32_Cam.png)

## :gear: How it works
The esp32-Cam takes a photo in a controlled environment and the photo is then processed by the opencv algorithm, that runs on the esp32 itself. The algorithm detects the markers following specific steps:
- The photo is taken in grayscale (otherwise is converted to grayscale)
- The grayscale photo is then filtered with median Blur and Gaussian Blur
- A canny edge detection is applied to the filtered photo
- The canny edge detection is then dilated 
- Contours are detected using FindContours function
- The contours are then analyzed and the ones that are not squares are discarded
- If some markers are missing the algorithm can detect them and send a message to the user
- The algorithm will save a list of the detected markers in a file called "results.txt"

## Results 
![Esp32-Cam](/Images/MARK0.png)

In this example the algorithm detected my keyboard keys as markers. Is clear that all closed polygons with equal sides are detected as markers.
In the picture most of the markers are detected twice (two red squares are drawn around them) because the algorithm detects the markers using RETR_TREE mode, that detects all the contours in the image. This is not really a problem because the algorithm will save only the markers that are not inside other markers. So in the result file only one marker per square is saved.
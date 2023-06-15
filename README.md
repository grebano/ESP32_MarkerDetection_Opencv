# :small_orange_diamond: ESP32_MarkerDetection_Opencv :small_orange_diamond:
In this poject the goal is to detect square markers in a photo taken by the esp32-Cam. Those markers indicate specific presence of various chemical indicators by changing color. The device itself is a medical POCD with the ability to detect patient pathologies. The device is a prototype and is not intended for use in medical practice.

## :hammer_and_wrench: Hardware 	
- ESP32-CAM (AI-Thinker)
- 5V to 3.3V step down converter
- 5V power supply
- 3D printed case
- 9V flash light
- 9V power supply

## 	:computer: Software
- Espressif IDF
- Opencv C++

## :camera: Esp32-Cam
![Esp32-Cam](/Images/Esp32_Cam.png)

## :gear: How it works
The esp32-Cam takes a photo in a controlled environment and the photo is then processed by the opencv algorithm, that runs on the esp32 itself. The algorithm detects the markers and their color following specific steps:
- The photo is converted to grayscale
- The grayscale photo is then filtered with a gaussian blur
- A canny edge detection is applied to the filtered photo
- Square contours are detected using FindContours function
- After some checks the algorithm detects the color of the marker
- If some markers are missing the algorithm will detect it and will send a message to the user
- The algorithm will save a list of the detected markers in a file called "results.txt"

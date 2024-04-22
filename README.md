# Spy-Camera-with-ESP32-and-OpenCV
This device will be able to record video feed from the ESP32-CAM on the PC when ever it detects a hand wave going from right to left and stops recording whenever it detects hand wave going from left to right.
The code to the ESP32-CAM catures the video frame and sends it to the PC through the wifi network as an IP camera.
I used OpenCV to read the video stream frame by frame from the IP camera. When each frame is read, it is being processed and the cascade classifier was used to detect features of the palm on the frame. If a palm was found, it stores the location as a cv::Rect in a vector. with each palm found it check wheater the palm moved or at a place as the video streams. When the palm moves from right to left, 

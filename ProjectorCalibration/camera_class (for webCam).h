//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// This is camera class for PointGrey camera
// If you use other camera (ex: usb camera), you should change this class, espetially "initCam" and "AsaCapture"
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


#pragma once

#include <iostream>



// opencv include & liblary
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_lib.hpp>

class CameraClass{

private:
	cv::VeideoCapture cap;
public:
	CameraClass(void){};
	~CameraClass(void){};

	// initialize
	void Initialize()
	{
		cap.open(0);
		if (!cap.isOpened()){
			std::cout << "Error: we can't open webcam" << std::endl;
			return;
		}

		cv::Mat frame;
		for (int ii = 0; ii < 5;ii++)
			cap >> frame;
	}

	// capture
	cv::Mat Capture()
	{
		cv::Mat frame;
		cap >> frame;
		return frame;
	}

	

};
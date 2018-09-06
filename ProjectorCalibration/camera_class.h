//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// This is camera class for PointGrey camera
// If you use other camera (ex: usb camera), you should change this class, espetially "initCam" and "AsaCapture"
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


#pragma once

#include <iostream>

// PointGrey
#include "FlyCapture2.h"
#pragma comment(lib, "FlyCapture2.lib")

// opencv include & liblary
#include <opencv2/opencv.hpp>
//#include <opencv2/opencv_lib.hpp>

class CameraClass{

private:
	FlyCapture2::Camera _camera;
	FlyCapture2::PGRGuid _guid;

	FlyCapture2::Property prop;

public:
	CameraClass(void){};
	~CameraClass(void){};

	// initialize
	void Initialize(int width, int height, float shutter, int wb_red, int wb_blue, int skipOpenFC2, int kk)
	{
		// パラメータ初期化
		FlyCapture2::BusManager busMgr;

		busMgr.GetCameraFromIndex(kk, &_guid);
		_camera.Connect(&_guid);
		//_camera.SetVideoModeAndFrameRate(FlyCapture2::VIDEOMODE_1600x1200RGB, FlyCapture2::FRAMERATE_15);

		prop.absControl = true;
		prop.onePush = false;
		prop.onOff = true;
		prop.autoManualMode = false;
		// gain
		prop.type = FlyCapture2::GAIN;
		prop.absValue = 0.0;
		_camera.SetProperty(&prop);
		// brightness
		prop.type = FlyCapture2::BRIGHTNESS;
		prop.absValue = 0;
		_camera.SetProperty(&prop);
		// shutter
		prop.type = FlyCapture2::SHUTTER;
		prop.absValue = shutter;
		_camera.SetProperty(&prop);
		// frame rate
		prop.type = FlyCapture2::FRAME_RATE;
		float anan = 1000.0 / shutter;
		if (anan > 60.0)anan = 60.0;
		anan = 15;
		prop.absValue = anan;
		_camera.SetProperty(&prop);
		// white balance
		prop.absControl = false;
		prop.type = FlyCapture2::WHITE_BALANCE;
		prop.valueA = wb_red;
		prop.valueB = wb_blue;
		_camera.SetProperty(&prop);

		prop.onOff = false;
		prop.absControl = false;
		prop.type = FlyCapture2::AUTO_EXPOSURE;
		_camera.SetProperty(&prop);
		prop.type = FlyCapture2::SHARPNESS;
		_camera.SetProperty(&prop);
		prop.type = FlyCapture2::HUE;
		_camera.SetProperty(&prop);
		prop.type = FlyCapture2::SATURATION;
		_camera.SetProperty(&prop);
		prop.type = FlyCapture2::GAMMA;
		_camera.SetProperty(&prop);

		// FlyCap2オープン
		if (skipOpenFC2 == 0){
			std::cout << "open flycap2 -> ";
			int ret = system("\"C:\\Program Files\\Point Grey Research\\FlyCapture2\\bin64\\Point Grey FlyCap2.exe\"");
			if (ret != 0){
				printf("command error\n");
			}
			std::cout << "done\n";
		}
		else{
			std::cout << "skip open flycap2\n";
		}

		// 設定したshutter speed読み取り
		prop.type = FlyCapture2::SHUTTER;
		_camera.GetProperty(&prop);
		//shutter = prop.absValue;
		prop.type = FlyCapture2::WHITE_BALANCE;
		_camera.GetProperty(&prop);
		//wb_red = prop.valueA;
		//wb_blue = prop.valueB;

		// キャプチャスタート
		_camera.StartCapture();

		// 解像度
		//width[0] = 1280;
		//height[0] = 960;
	}

	// capture
	FlyCapture2::Image convertedImage;
	cv::Mat Capture(int capture_width, int capture_height)
	{
		FlyCapture2::Image rawImage;
		_camera.RetrieveBuffer(&rawImage);
		rawImage.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &convertedImage);
		return cv::Mat(capture_height, capture_width, CV_8UC3, convertedImage.GetData());
	}

	// change shutter speed
	void ChangeShutter(float shutter){
		prop.type = FlyCapture2::FRAME_RATE;
		prop.absControl = true;
		float anan = 1000.0 / shutter;
		if (anan > 60.0)anan = 60.0;
		prop.absValue = anan;
		_camera.SetProperty(&prop);
		prop.type = FlyCapture2::SHUTTER;
		prop.absControl = true;
		prop.absValue = shutter;
		_camera.SetProperty(&prop);
	}
	void ChangeGain(float gain){
		prop.type = FlyCapture2::GAIN;
		prop.absValue = gain;
		_camera.SetProperty(&prop);
	}

};
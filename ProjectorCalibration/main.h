#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <Windows.h>
#include <math.h>

// opencv include & liblary
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_lib.hpp>

// camera class
#include "camera_class.h"

using namespace std;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// global method
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// global variable
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// camera resolution
int camera_width = 1280, camera_height = 960;

// projector resolution 
int window_width = 1280, window_height = 800;

// projector position
int init_window_posx = 1280, init_window_posy = 100;

// file directory
namespace
{
	static const string SAVEIMAGE_BASENAME = "Data\\";
	static const string SAVEIMAGE_EXT = ".png";

	static const string CAMERA_INTRINSIC_PARAM = "CAM_PARAM_MAT.xml";
	static const string PROJECTOR_INTRINSIC_PARAM = "PRO_PARAM_MAT.xml";
	static const string PROCAM_EXTRINSIC_PARAM = "PROCAM_MAT.txt";
}

// checker config
float checkSize = 114.0 / 5.0;
int checkPointX = 7, checkPointY = 10;

// white color
int white_color = 255;

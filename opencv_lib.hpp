// version
#define CV_VERSION_STR CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) CVAUX_STR(CV_SUBMINOR_VERSION)

// build mode
#ifdef _DEBUG
#define CV_BUILD_STR "d.lib"
#else
#define CV_BUILD_STR ".lib"
#endif


#pragma comment(lib, "opencv_calib3d" CV_VERSION_STR CV_BUILD_STR)
#pragma comment(lib, "opencv_contrib" CV_VERSION_STR CV_BUILD_STR)
#pragma comment(lib, "opencv_core" CV_VERSION_STR CV_BUILD_STR)
#pragma comment(lib, "opencv_features2d" CV_VERSION_STR CV_BUILD_STR)
#pragma comment(lib, "opencv_flann" CV_VERSION_STR CV_BUILD_STR)
#pragma comment(lib, "opencv_gpu" CV_VERSION_STR CV_BUILD_STR)
#pragma comment(lib, "opencv_highgui" CV_VERSION_STR CV_BUILD_STR)
#pragma comment(lib, "opencv_imgproc" CV_VERSION_STR CV_BUILD_STR)
#pragma comment(lib, "opencv_legacy" CV_VERSION_STR CV_BUILD_STR)
#pragma comment(lib, "opencv_ml" CV_VERSION_STR CV_BUILD_STR)
#pragma comment(lib, "opencv_nonfree" CV_VERSION_STR CV_BUILD_STR)
#pragma comment(lib, "opencv_objdetect" CV_VERSION_STR CV_BUILD_STR)
#pragma comment(lib, "opencv_ocl" CV_VERSION_STR CV_BUILD_STR)
#pragma comment(lib, "opencv_photo" CV_VERSION_STR CV_BUILD_STR)
#pragma comment(lib, "opencv_stitching" CV_VERSION_STR CV_BUILD_STR)
#pragma comment(lib, "opencv_superres" CV_VERSION_STR CV_BUILD_STR)
#pragma comment(lib, "opencv_ts" CV_VERSION_STR CV_BUILD_STR)
#pragma comment(lib, "opencv_video" CV_VERSION_STR CV_BUILD_STR)
#pragma comment(lib, "opencv_videostab" CV_VERSION_STR CV_BUILD_STR)
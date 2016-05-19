#include "main.h"

// 事前に内部パラメータを取得しているならDefine
// If you have a intrinsic parameter of camera, you should define this
//#define PRE_CALIBRATION


int main(int argc, char **argv){

	// 撮影済みであるなら、ここの数字をいじって、'q'で計算する
	// If you have already captured calobaration images, you change this variable;
	int img_num = 0;

	// camera init
	CameraClass *camera = new CameraClass();
	camera->Initialize(camera_width, camera_height, 4, 760, 600, 1, 0);

	// initialize Mat
	cv::Mat cap_img = cv::Mat::zeros(camera_height, camera_width, CV_8UC3);
	cv::Mat pro_img = cv::Mat::zeros(window_height, window_width, CV_8UC1);

	// make window 
	// (OpenCVで無理やりフルスクリーン表示っぽいことをしている。うまくいかないならixとiyを調整する、もしくはOpenCVのフルスクリーンを使う)
	// "ix" and "iy" is frame size of window. You should change these variable manually, if you see the frame in projected image.
	int ix = GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXFIXEDFRAME) + GetSystemMetrics(SM_CXEDGE);
	int iy = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYEDGE);
	cout << "ix: " << ix << endl << "iy: " << iy << endl;
	cv::imshow("project_window", pro_img);
	cv::waitKey(10);
	cv::moveWindow("project_window", init_window_posx - ix, init_window_posy - iy);
	cv::waitKey(10);

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// capture step
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	int x = checkPointY + 1, y = checkPointX + 1;
	int xkazu = 50, ykazu = 50;
	int xzure = 200, yzure = 200;
	while (true){

		// project checker
		pro_img = cv::Scalar(white_color);
		if ((x*xkazu + x + xzure) < window_width && (y*ykazu + y + yzure) < window_height){
			bool tm = true;
			for (int ii = 1; ii <= x; ii++){
				bool fff;
				if (tm) fff = true;
				else fff = false;
				tm = !tm;
				for (int jj = 1; jj <= y; jj++){
					if (fff){
						cv::rectangle(pro_img, cv::Point((ii - 1)*xkazu + ii + xzure, (jj - 1)*ykazu + jj + yzure),
							cv::Point(ii*xkazu + ii + xzure, jj*ykazu + jj + yzure), cv::Scalar(0), -1, 4);
					}
					fff = !fff;
				}
			}
		}
		cv::flip(pro_img, pro_img, 0);
		cv::imshow("project_window", pro_img);
		//cv::flip(pro_img, pro_img, 0);

		// capture
		cap_img = camera->Capture(camera_width, camera_height);
		cv::flip(cap_img, cap_img, 1);
		cv::Mat temp;
		cv::resize(cap_img, temp, cv::Size(), 640.0 / (double)camera_width, 480.0 / (double)camera_height);
		cv::imshow("c:save, q:next, asdw:translate, zx:size", temp);


		int key = cv::waitKey(10);
		// c:save
		if (key == 'c'){
			cv::waitKey(300);
			string saveName1 = SAVEIMAGE_BASENAME + "CapWithPro_" + to_string(img_num) + SAVEIMAGE_EXT;
			string saveName2 = SAVEIMAGE_BASENAME + "CapWithoutPro_" + to_string(img_num) + SAVEIMAGE_EXT;
			string saveName3 = SAVEIMAGE_BASENAME + "Pro_" + to_string(img_num) + SAVEIMAGE_EXT;

			cv::imwrite(saveName1, cap_img);
			cv::imwrite(saveName3, pro_img);

			pro_img = cv::Scalar(white_color);
			cv::imshow("project_window", pro_img);
			cv::waitKey(300);

			// capture
			cap_img = camera->Capture(camera_width, camera_height);
			cv::flip(cap_img, cap_img, 1);

			cv::imwrite(saveName2, cap_img);

			cout << "capture image: " << img_num << endl;

			img_num++;
		}
		// q: end capture
		else if (key == 'q'){
			cout << "Capture End" << endl;
			cv::destroyAllWindows();
			break;
		}
		// esc: end program
		else if (key == '\x1b'){
			return 0;
		}
		// change checker
		else if (key == 'z'){
			xkazu -= 5; ykazu -= 5;
		}
		else if (key == 'x'){
			xkazu += 5; ykazu += 5;
		}
		else if (key == 'a') xzure -= 10;
		else if (key == 'd') xzure += 10;
		else if (key == 'w') yzure += 10;
		else if (key == 's') yzure -= 10;
	}

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// recognize step
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	vector<cv::Mat> checkerImgs1;
	vector<cv::Mat> checkerImgs2;
	vector<cv::Mat> projectImgs;
	const cv::Size patternSize(checkPointX, checkPointY);
	vector<vector<cv::Point2f>> imagePoints1(img_num);
	vector<vector<cv::Point3f>> worldPoints1(img_num);
	vector<vector<cv::Point2f>> imagePoints2(img_num);
	vector<vector<cv::Point3f>> worldPoints2(img_num);
	vector<vector<cv::Point2f>> projectPoints(img_num);


	// read image
	cv::Mat newmatrix;
	for (int ii = 0; ii < img_num; ii++){
		string saveName1 = SAVEIMAGE_BASENAME + "CapWithPro_" + to_string(ii) + SAVEIMAGE_EXT;
		string saveName2 = SAVEIMAGE_BASENAME + "CapWithoutPro_" + to_string(ii) + SAVEIMAGE_EXT;
		string saveName3 = SAVEIMAGE_BASENAME + "Pro_" + to_string(ii) + SAVEIMAGE_EXT;
		checkerImgs1.push_back(cv::imread(saveName2));
		checkerImgs2.push_back(cv::imread(saveName1));
		projectImgs.push_back(cv::imread(saveName3));
		cout << "Load image: " << ii << endl;
	}

	// devide
	for (int ii = 0; ii < img_num; ii++){
		int sizen = checkerImgs1[ii].cols*checkerImgs1[ii].rows;
		for(int cc = 0; cc<sizen; cc++){
			checkerImgs1[ii].data[cc * 3 + 1] = 0;
			checkerImgs1[ii].data[cc * 3 + 2] = 0;
			checkerImgs2[ii].data[cc * 3 + 0] = 0;
			checkerImgs2[ii].data[cc * 3 + 1] = 0;
		}
		string saveName1 = SAVEIMAGE_BASENAME + "Z_SourceCHESS_" + to_string(ii) + SAVEIMAGE_EXT;
		string saveName2 = SAVEIMAGE_BASENAME + "Z_ProjectCHESS_" + to_string(ii) + SAVEIMAGE_EXT;
		cv::imwrite(saveName1, checkerImgs1[ii]);
		cv::imwrite(saveName2, checkerImgs2[ii]);
	}

	// read camera's intrinsic parameter
	string paramName = SAVEIMAGE_BASENAME + CAMERA_INTRINSIC_PARAM;
#ifdef PRE_CALIBRATION
	cout << "read pre calibrated data" << endl;

#else
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// calcurate camera intrinsic parameter
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	std::vector<int> t_misspoint;
	int flag = 0;
	for (int i = 0; i < img_num; i++){
		imagePoints1[i].clear();
		cv::Mat tempimg = checkerImgs1[i].clone();
		if (cv::findChessboardCorners(tempimg, patternSize, imagePoints1[i], CV_CALIB_CB_ADAPTIVE_THRESH)){
			cout << "*********";
			cout << " Success: " << i << endl;

			// subpicel
			cv::Mat gray;
			cv::cvtColor(tempimg, gray, CV_BGR2GRAY);
			cv::cornerSubPix(gray, imagePoints1[i], cv::Size(2, 2), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.001));

			flag++;
			// render
			cv::drawChessboardCorners(tempimg, patternSize, (cv::Mat)(imagePoints1[i]), true);
			cv::imshow("result", tempimg);
			cv::waitKey(30);
		}
		else{
			cout << "Fail: " << i << endl;
			t_misspoint.push_back(i);
		}
	}
	if (flag == 0) return 0;

	// set world point of checker board
	for (int i = 0; i < img_num; i++){
		worldPoints1[i].clear();
		for (int k = 0; k < patternSize.height; k++){
			for (int j = 0; j < patternSize.width; j++){
				worldPoints1[i].push_back(cv::Point3f(
					(float)(k * checkSize),
					(float)(j * checkSize),
					0.0f
					));
			}
		}
	}

	// lost 
	for (int ii = t_misspoint.size() - 1; ii >= 0; ii--){
		int are = t_misspoint[ii];
		std::vector<std::vector<cv::Point2f>>::iterator unkite = imagePoints1.begin() + are;
		imagePoints1.erase(unkite);
	}
	for (int ii = t_misspoint.size() - 1; ii >= 0; ii--){
		int are = t_misspoint[ii];
		std::vector<std::vector<cv::Point3f>>::iterator unkite = worldPoints1.begin() + are;
		worldPoints1.erase(unkite);
	}
	img_num -= t_misspoint.size();


	// camera parameter
	cv::Mat         cameraMatrix;       // intrinsic patameter matrix
	cv::Mat         distCoeffs;         // distortion matrix
	std::vector<cv::Mat> rotationVectors;    // rotation vector in each image
	std::vector<cv::Mat> translationVectors; // translation vector in each image

	std::cout << "Start Pre Calibration " << std::endl;

	// XML file
	cv::FileStorage rfs(paramName, cv::FileStorage::READ);
	bool bExistCameraParam = rfs.isOpened();

	double a = cv::calibrateCamera(worldPoints1, imagePoints1, checkerImgs1[0].size(),
		cameraMatrix, distCoeffs, rotationVectors, translationVectors);
	cout << "reprojection_error:" << a << endl;

	// calcurate 
	double apertureWidth = 0, apertureHeight = 0;						// apeature
	double fovx = 0, fovy = 0;											// angle of view [deg]
	double focalLength = 0;												// focal length [mm]
	cv::Point2d principalPoint = (0, 0);								// focal length [pixel]
	double aspectRatio = 0;												// aspect ratio
	std::cout << "Calc Camera Param" << std::endl;
	cv::calibrationMatrixValues(cameraMatrix, checkerImgs1[0].size(), apertureWidth, apertureHeight,
		fovx, fovy, focalLength, principalPoint, aspectRatio);

	// output XML file
	std::cout << "Start Output Xml File" << std::endl;

	cv::FileStorage wfs(paramName, cv::FileStorage::WRITE);
	wfs << "CameraMatrix" << cameraMatrix;
	wfs << "DistortCoeffs" << distCoeffs;

	wfs << "aperture_Width" << apertureWidth;
	wfs << "aperture_Height" << apertureHeight;
	wfs << "fov_x" << fovx;
	wfs << "fov_y" << fovy;
	wfs << "focal_Length" << focalLength;
	wfs << "principal_Point" << principalPoint;
	wfs << "aspect_Ratio" << aspectRatio;
	wfs.release();

	img_num += t_misspoint.size();
	std::cout << "Finish Output Xml File" << std::endl;
#endif

	// read intrinsic parameter
	cv::Mat intrinsicv1, distortionv1;
	cv::FileStorage cvfs1(paramName, cv::FileStorage::READ);
	cv::FileNode param1(cvfs1.fs, NULL);
	cv::read(cvfs1["CameraMatrix"], intrinsicv1);
	cv::read(cvfs1["DistortCoeffs"], distortionv1);

	// distortion correction
	for (int ii = 0; ii < img_num; ii++){
		cv::Mat dst;
		cv::undistort(checkerImgs1[ii], dst, intrinsicv1, distortionv1);
		dst.copyTo(checkerImgs1[ii]);
		cv::undistort(checkerImgs2[ii], dst, intrinsicv1, distortionv1);
		dst.copyTo(checkerImgs2[ii]);
	}


	// recognize chess board & render intersect points
	vector<int> missCount;
	int flags[3] = { 0, 0, 0 };
	for (int ii = 0; ii < img_num; ii++){
		imagePoints1[ii].clear();
		imagePoints2[ii].clear();
		projectPoints[ii].clear();

		cout << "CapWithoutPro -> ";
		if (cv::findChessboardCorners(checkerImgs1[ii], patternSize, imagePoints1[ii], CV_CALIB_CB_ADAPTIVE_THRESH)){
			std::cout << " Success: " << ii << std::endl;

			// subpicel
			cv::Mat gray;
			cv::cvtColor(checkerImgs1[ii], gray, CV_BGR2GRAY);
			cv::cornerSubPix(gray, imagePoints1[ii], cv::Size(5, 5), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.001));

			flags[0]++;
			// render
			cv::drawChessboardCorners(checkerImgs1[ii], patternSize, (cv::Mat)(imagePoints1[ii]), true);
			cv::imshow("result", checkerImgs1[ii]);
			cv::waitKey(30);

		}
		else{
			std::cout << "Fail: " << ii << std::endl;
			missCount.push_back(ii);
			continue;
		}

		cout << "CapWithPro -> ";
		if (cv::findChessboardCorners(checkerImgs2[ii], patternSize, imagePoints2[ii], CV_CALIB_CB_ADAPTIVE_THRESH)){
			std::cout << " Success: " << ii << std::endl;

			// subpicel
			cv::Mat gray;
			cv::cvtColor(checkerImgs2[ii], gray, CV_BGR2GRAY);
			cv::cornerSubPix(gray, imagePoints2[ii], cv::Size(5, 5), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.001));

			flags[1]++;
			// render
			cv::drawChessboardCorners(checkerImgs2[ii], patternSize, (cv::Mat)(imagePoints2[ii]), true);
			cv::imshow("result", checkerImgs2[ii]);
			cv::waitKey(30);

		}
		else{
			std::cout << "Fail: " << ii << std::endl;
			missCount.push_back(ii);
			continue;
		}

		cout << "Pro -> ";
		if (cv::findChessboardCorners(projectImgs[ii], patternSize, projectPoints[ii], CV_CALIB_CB_ADAPTIVE_THRESH)){
			std::cout << " Success: " << ii << std::endl;

			// subpicel
			cv::Mat gray;
			cv::cvtColor(projectImgs[ii], gray, CV_BGR2GRAY);
			cv::cornerSubPix(gray, projectPoints[ii], cv::Size(2, 2), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.001));

			flags[2]++;
			// render
			cv::drawChessboardCorners(projectImgs[ii], patternSize, (cv::Mat)(projectPoints[ii]), true);
			cv::imshow("result", projectImgs[ii]);
			cv::waitKey(30);

		}
		else{
			std::cout << "Fail: " << ii << std::endl;
			missCount.push_back(ii);
			continue;
		}
	}
	if (flags[0] == 0 || flags[1] == 0 || flags[2] == 0) return 0;

	// set world point in sourceCHESS
	for (int i = 0; i < img_num; i++){
		worldPoints1[i].clear();
		for (int k = 0; k < patternSize.height; k++){
			for (int j = 0; j < patternSize.width; j++){
				worldPoints1[i].push_back(cv::Point3f(
					(float)((float)k * checkSize),
					(float)((float)j * checkSize),
					0.0f
					));
			}
		}
	}

	// calcurate world point in projectCHESS
	distortionv1 = cv::Scalar(0);
	for (int i = 0; i < img_num; i++){
		if (imagePoints1[i].size() == 0 || imagePoints2[i].size() == 0) continue;
		cv::Mat rvec, tvec, R;
		cv::solvePnP(worldPoints1[i], imagePoints1[i], intrinsicv1, distortionv1, rvec, tvec);
		cv::Rodrigues(rvec, R);

		cv::Mat trans = cv::Mat::zeros(3, 3, CV_64F);
		trans.at<double>(0, 0) = R.at<double>(0, 0); trans.at<double>(0, 1) = R.at<double>(0, 1);
		trans.at<double>(1, 0) = R.at<double>(1, 0); trans.at<double>(1, 1) = R.at<double>(1, 1);
		trans.at<double>(2, 0) = R.at<double>(2, 0); trans.at<double>(2, 1) = R.at<double>(2, 1);
		trans.at<double>(0, 2) = tvec.at<double>(0, 0);
		trans.at<double>(1, 2) = tvec.at<double>(1, 0);
		trans.at<double>(2, 2) = tvec.at<double>(2, 0);

		worldPoints2[i].clear();
		for (int ii = 0; ii < imagePoints2[i].size(); ii++){
			cv::Mat a = cv::Mat::zeros(3, 1, CV_64F);
			a.at<double>(0, 0) = (double)imagePoints2[i][ii].x;
			a.at<double>(1, 0) = (double)imagePoints2[i][ii].y;
			a.at<double>(2, 0) = 1.0;

			cv::Mat b = trans.inv() * intrinsicv1.inv()*a;
			b.at<double>(0, 0) /= b.at<double>(2, 0);
			b.at<double>(1, 0) /= b.at<double>(2, 0);

			worldPoints2[i].push_back(cv::Point3f((float)b.at<double>(0, 0), (float)b.at<double>(1, 0), 0.0));
		}
	}

	// delete failed image
	for (int ii = missCount.size() - 1; ii >= 0; ii--){
		int are = missCount[ii];
		std::vector<std::vector<cv::Point2f>>::iterator unkite = imagePoints1.begin() + are;
		imagePoints1.erase(unkite);
		unkite = imagePoints2.begin() + are;
		imagePoints2.erase(unkite);
		unkite = projectPoints.begin() + are;
		projectPoints.erase(unkite);
	}
	for (int ii = missCount.size() - 1; ii >= 0; ii--){
		int are = missCount[ii];
		std::vector<std::vector<cv::Point3f>>::iterator unkite = worldPoints1.begin() + are;
		worldPoints1.erase(unkite);
		unkite = worldPoints2.begin() + are;
		worldPoints2.erase(unkite);
	}
	img_num -= missCount.size();

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// calcurate step
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	//******************* inner ******************************* 
	cv::Mat         pro_cameraMatrix;     
	cv::Mat         pro_distCoeffs;        
	std::vector<cv::Mat> pro_rotationVectors;   
	std::vector<cv::Mat> pro_translationVectors;

	std::cout << "Start Projector Calibration " << std::endl;

	// XLM file
	string proName = SAVEIMAGE_BASENAME + PROJECTOR_INTRINSIC_PARAM;
	cv::FileStorage pro_rfs(proName, cv::FileStorage::READ);
	bool pro_bExistCameraParam = pro_rfs.isOpened();

	double pro_a = cv::calibrateCamera(worldPoints2, projectPoints, projectImgs[0].size(),
		pro_cameraMatrix, pro_distCoeffs, pro_rotationVectors, pro_translationVectors);
	cout << "reprojection_error: " << pro_a << endl;

	double pro_apertureWidth = 0, pro_apertureHeight = 0; 
	double pro_fovx = 0, pro_fovy = 0;                                       
	double pro_focalLength = 0;                                        
	cv::Point2d pro_principalPoint = (0, 0);                         
	double pro_aspectRatio = 0;                                           

	cv::calibrationMatrixValues(pro_cameraMatrix, projectImgs[0].size(), pro_apertureWidth, pro_apertureHeight,
		pro_fovx, pro_fovy, pro_focalLength, pro_principalPoint, pro_aspectRatio);

	std::cout << "Calc Projector Param" << std::endl;

	// output XML
	std::cout << "Start Output Xml File" << std::endl;

	cv::FileStorage pro_wfs(proName, cv::FileStorage::WRITE);
	pro_wfs << "CameraMatrix" << pro_cameraMatrix;
	pro_wfs << "DistortCoeffs" << pro_distCoeffs;

	pro_wfs << "aperture_Width" << pro_apertureWidth;
	pro_wfs << "aperture_Height" << pro_apertureHeight;
	pro_wfs << "fov_x" << pro_fovx;
	pro_wfs << "fov_y" << pro_fovy;
	pro_wfs << "focal_Length" << pro_focalLength;
	pro_wfs << "principal_Point" << pro_principalPoint;
	pro_wfs << "aspect_Ratio" << pro_aspectRatio;
	pro_wfs.release();

	std::cout << "Finish Output Xml File" << std::endl;


	//******************** outer ****************************** 
	cv::Mat R, T, E, F;
	cv::TermCriteria criteria{ 10000, 10000, 0.0001 };
	double tms = cv::stereoCalibrate(worldPoints2, imagePoints2, projectPoints, intrinsicv1, distortionv1,
		pro_cameraMatrix, pro_distCoeffs, checkerImgs1[0].size(), R, T, E, F, criteria,
		CV_CALIB_FIX_INTRINSIC/*CV_CALIB_USE_INTRINSIC_GUESS*//*CV_CALIB_FIX_ASPECT_RATIO*/);
	//printf("\n%f\n", tms);
	//cout << R << "\n" << T << "\n\n";

	if (true){
		string saveName = SAVEIMAGE_BASENAME + PROCAM_EXTRINSIC_PARAM;
		FILE *fp;
		fopen_s(&fp, saveName.c_str(), "w");
		for (int jj = 0; jj < 3; jj++){
			fprintf_s(fp, "%f,%f,%f,%f\n", R.at<double>(jj, 0), R.at<double>(jj, 1), R.at<double>(jj, 2), T.at<double>(jj, 0));
		}
		fclose(fp);
	}


	////********************************************************/
	//cv::Mat R1, R2, P1, P2, Q;
	//cv::stereoRectify(intrinsicv1, distortionv1, pro_cameraMatrix, pro_distCoeffs,
	//	checkerImgs1[0].size(), R, T, R1, R2, P1, P2, Q, CV_CALIB_ZERO_DISPARITY, 1, projectImgs[0].size());
	//cout << "\n" << R1 << "\n" << R2 << "\n" << P1 << "\n" << P2;

	///* 平行化マップを求める */
	//cv::Mat mapX1, mapY1;
	//cv::Mat mapX2, mapY2;

	//cv::initUndistortRectifyMap(intrinsicv1, distortionv1, R1, P1, checkerImgs1[0].size(), CV_32FC1, mapX1, mapY1);
	//cv::initUndistortRectifyMap(pro_cameraMatrix, pro_distCoeffs, R2, P2, projectImgs[0].size(), CV_32FC1, mapX2, mapY2);

	//cv::Mat dst1, dst2;
	//cv::remap(cv::imread("C:\\TempData\\a.png"), dst1, mapX1, mapY1, CV_INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));
	//cv::remap(cv::imread("C:\\TempData\\pro.png"), dst2, mapX2, mapY2, CV_INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));
	//cv::resize(dst1, dst1, cv::Size(), 0.5, 0.5);
	//cv::resize(dst2, dst2, cv::Size(), 0.5, 0.5);
	//cv::imshow("1", dst1);
	//cv::imshow("2", dst2);

	cout << "終わりEND -> enter" << endl;
	getchar();


	return 0;
}
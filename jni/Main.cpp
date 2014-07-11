#include <opencv2/opencv.hpp>  
#include "FaceFixing.h"

int main()
{
	char modelImg[] = "E:\\JCN\\test\\OpenCVDemo\\model7.jpg";
	char userImg[] = "E:\\JCN\\test\\OpenCVDemo\\user7.jpg";
	char faceImg[] = "E:\\JCN\\test\\OpenCVDemo\\face7.png";
	char resultImg[] = "E:\\JCN\\test\\OpenCVDemo\\result.jpg";

	CvPoint facePoint[3];
	facePoint[0] = cvPoint(966, 398);
	facePoint[1] = cvPoint(1084, 400);
	facePoint[2] = cvPoint(1014, 522);

	FaceFix2File(modelImg, userImg, faceImg, facePoint, resultImg);

	return 0;
}
#include <opencv2/opencv.hpp>  
#include "FaceFixing.h"

int main()
{
	char modelImg[] = "E:\\JCN\\test\\OpenCVDemo\\model8.jpg";
	char userImg[] = "E:\\JCN\\test\\OpenCVDemo\\user9.jpg";
	char faceImg[] = "E:\\JCN\\test\\OpenCVDemo\\face8.png";
	char resultImg[] = "E:\\JCN\\test\\OpenCVDemo\\result.jpg";

	CvPoint facePoint[3];
	facePoint[0] = cvPoint(365, 146);
	facePoint[1] = cvPoint(428, 146);
	facePoint[2] = cvPoint(394, 216);

	FaceFix2File(modelImg, userImg, faceImg, facePoint, resultImg);

	return 0;
}
#include "FaceFixing.h"
#include "FaceColor.h"
#include "FaceFitting.h"
#include "Util.h"

void FaceFix2File(char *fileMod, char *fileUser, char *fileFace, CvPoint facePoint[3], char *fileFix)
{
	IplImage *imgFaceFix = FaceFixing(fileMod, fileUser, fileFace, facePoint);
	if (imgFaceFix)
	{
		cvSaveImage(fileFix, imgFaceFix);
		cvReleaseImage(&imgFaceFix);
	}
	return;
}

IplImage* FaceFix2Memo(char *fileMod, char *fileUser, char *fileFace, CvPoint facePoint[3])
{
	IplImage *imgFaceFix = FaceFixing(fileMod, fileUser, fileFace, facePoint);
	return imgFaceFix;
}

IplImage* FaceFixing(char *fileMod, char *fileUser, char *fileFace, CvPoint facePoint[3])
{
	// 读入模特和用户图像  
	IplImage *imgMod = cvLoadImage(fileMod);
	IplImage *imgUsr = cvLoadImage(fileUser);
	if (!imgMod || !imgUsr || imgMod->nChannels < 3 || imgUsr->nChannels < 3)
	{
		if (imgMod) cvReleaseImage(&imgMod);
		if (imgUsr) cvReleaseImage(&imgUsr);
		return NULL;
	}

	CvSize imgSize = cvSize(imgUsr->width, imgUsr->height);
	IplImage *imgFix = cvCreateImage(imgSize, imgUsr->depth, 3);
	cvCopy(imgUsr, imgFix);

	// 读入抠脸图像，必须是png
	int len = strlen(fileFace);
	if (len <= 4 || _stricmp(fileFace + len - 4, ".png") != 0)
	{
		cvReleaseImage(&imgMod);
		cvReleaseImage(&imgUsr);
		return NULL;
	}
	Mat faceMat = imread(fileFace, -1);
	IplImage faceImg = IplImage(faceMat);
	IplImage *imgFace = &faceImg;

	// 检查图像大小是否一致
	if (imgMod->width != imgFace->width || imgFace->width != imgUsr->width || imgMod->height != imgFace->height || imgFace->height != imgUsr->height)
	{
		cvReleaseImage(&imgMod);
		cvReleaseImage(&imgUsr);
		return NULL;
	}

	// 初始化眼睛和嘴的位置
	int kerDist[3], extDist[3];
	initLocation(facePoint, kerDist, extDist);

	// 脸部肤色转换
	IplImage *imgFaceColor = cvCreateImage(imgSize, imgMod->depth, 3);
	transUserSkin(imgMod, imgUsr, imgFace, imgFaceColor);

	// 脸部皮肤拟合
	IplImage *imgFaceFix = cvCreateImage(imgSize, imgMod->depth, 3);
	fittingUserFace(imgMod, imgFace, imgFaceColor, facePoint, kerDist, extDist, imgFaceFix);

	cvNamedWindow("dealMerge");  
    cvShowImage("dealMerge",imgFaceFix);

	// 磨皮（双边滤波）
	IplImage *imgFaceSmooth = cvCreateImage(imgSize, imgMod->depth, 3);
	cvSmooth(imgFaceFix, imgFaceSmooth, CV_BILATERAL, 5, 5, 20, 20);
	cvNamedWindow("dealSmooth");  
    cvShowImage("dealSmooth",imgFaceSmooth);

	cvWaitKey(0);
	cvReleaseImage(&imgMod);
	cvReleaseImage(&imgUsr);
	cvReleaseImage(&imgFaceColor);
	cvReleaseImage(&imgFaceFix);

	return imgFaceSmooth;
}

void initLocation(CvPoint p[3], int kd[3], int md[3])
{
	int x = (p[0].x + p[1].x) >> 1;
	int y = (p[0].y + p[1].y) >> 1;
	int d = calDistance(cvPoint(x, y), p[2]);

	x += ((p[2].x - x) * 3) / 4;
	y += ((p[2].y - y) * 3) / 4;
	p[2] = cvPoint(x, y);

	kd[0] = kd[1] = d * 9 / 20;
	kd[2] = d * 7 / 10;
	
	md[0] = md[1] = kd[0] + (kd[0] * 4 / 10);
	md[2] = kd[2] + (kd[2] >> 2);
}


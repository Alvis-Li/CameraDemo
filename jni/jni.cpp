#include <opencv2/opencv.hpp>
//#include "FaceFixing.h"
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <com_demo_lib_Myjni.h>
using namespace cv;
#include <android/log.h>

#define LOG_TAG "jni_debug"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
void transUserSkin(IplImage *imgMod, IplImage *imgUsr, IplImage *imgFace,
		IplImage *imgFaceSkin);
CvScalar detectFaceColor(IplImage* img, IplImage* ref);
void transFaceColor(IplImage* user, IplImage* face, int diff[3],
		IplImage* dest);
void fittingUserFace(IplImage *model, IplImage *face, IplImage *user,
		CvPoint facePoint[3], int kerDist[3], int farDist[3], IplImage *dest);
void FaceFix2File(char *fileMod, char *fileUser, char *fileFace,
		CvPoint facePoint[3], char *fileFix);
void FaceFix2Memo(char *fileMod, char *fileUser, char *fileFace,
		CvPoint facePoint[3], char *buffer);
IplImage* FaceFixing(char *fileMod, char *fileUser, char *fileFace,
		CvPoint facePoint[3]);
void initLocation(CvPoint p[3], int kd[3], int md[3]);
int calDistance(CvPoint p1, CvPoint p2);

void JNICALL Java_com_demo_lib_Myjni_add2(JNIEnv * jenv, jobject, jstring model,
		jstring user, jstring face, jstring result, jint point1x, jint point1y, jint point2x, jint point2y,
		jint point3x, jint point3y) {
	//	char modelImg[] = "/storage/emulated/0/model8.jpg";
	//	char userImg[] = "/storage/emulated/0/user8.jpg";
	//	char faceImg[] = "/storage/emulated/0/face8.png";
	//	char resultImg[] = "/storage/emulated/0/result8.jpg";
	char* modelImg = (char*) jenv->GetStringUTFChars(model, NULL);
	char* userImg = (char*) jenv->GetStringUTFChars(user, NULL);
	char* faceImg = (char*) jenv->GetStringUTFChars(face, NULL);
	char* resultImg = (char*) jenv->GetStringUTFChars(result, NULL);
	CvPoint facePoint[3];
	facePoint[0] = cvPoint(point1x, point1y);
	facePoint[1] = cvPoint(point2x, point2y);
	facePoint[2] = cvPoint(point3x, point3y);
	LOGD("Java_com_demo_lib_Myjni_add2");
	FaceFix2File(modelImg, userImg, faceImg, facePoint, resultImg);
}
;

void FaceFix2File(char *fileMod, char *fileUser, char *fileFace,
		CvPoint facePoint[3], char *fileFix) {
	LOGD("FaceFix2File");
	IplImage *imgFaceFix = FaceFixing(fileMod, fileUser, fileFace, facePoint);
	if (imgFaceFix) {
		cvSaveImage(fileFix, imgFaceFix);
		cvReleaseImage(&imgFaceFix);
	}
	return;
}

IplImage* FaceFix2Memo(char *fileMod, char *fileUser, char *fileFace,
		CvPoint facePoint[3]) {
	IplImage *imgFaceFix = FaceFixing(fileMod, fileUser, fileFace, facePoint);
	return imgFaceFix;
}

IplImage* FaceFixing(char *fileMod, char *fileUser, char *fileFace,
		CvPoint facePoint[3]) {
	// 读入模特和用户图像
	IplImage *imgMod = cvLoadImage(fileMod);
	IplImage *imgUsr = cvLoadImage(fileUser);
	LOGD("FaceFixing");
	if (!imgMod || !imgUsr || imgMod->nChannels < 3 || imgUsr->nChannels < 3) {
		if (imgMod)
			cvReleaseImage(&imgMod);
		if (imgUsr)
			cvReleaseImage(&imgUsr);
		LOGD("FaceFixingnullimg:%s,%s", fileMod, fileUser);
		return NULL;
	}

	CvSize imgSize = cvSize(imgUsr->width, imgUsr->height);
	IplImage *imgFix = cvCreateImage(imgSize, imgUsr->depth, 3);
	cvCopy(imgUsr, imgFix);

	// 读入抠脸图像，必须是png
	int len = strlen(fileFace);
	if (len <= 4 || strcmp(fileFace + len - 4, ".png") != 0) {
		cvReleaseImage(&imgMod);
		cvReleaseImage(&imgUsr);
		LOGD("%s", fileFace);
		return NULL;
	}
	Mat faceMat = imread(fileFace, -1);
	IplImage faceImg = IplImage(faceMat);
	IplImage *imgFace = &faceImg;

	// 检查图像大小是否一致
	if (imgMod->width != imgFace->width || imgFace->width != imgUsr->width
			|| imgMod->height != imgFace->height
			|| imgFace->height != imgUsr->height) {
		cvReleaseImage(&imgMod);
		cvReleaseImage(&imgUsr);
		LOGD("FaceFixingsize not same");
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
	fittingUserFace(imgMod, imgFace, imgFaceColor, facePoint, kerDist, extDist,
			imgFaceFix);

//	cvNamedWindow("dealMerge");
//	cvShowImage("dealMerge", imgFaceFix);

// 磨皮（双边滤波）
	IplImage *imgFaceSmooth = cvCreateImage(imgSize, imgMod->depth, 3);
	cvSmooth(imgFaceFix, imgFaceSmooth, CV_BILATERAL, 5, 5, 20, 20);
//	cvNamedWindow("dealSmooth");
//	cvShowImage("dealSmooth", imgFaceSmooth);
//
//	cvWaitKey(0);
	cvReleaseImage(&imgMod);
	cvReleaseImage(&imgUsr);
	cvReleaseImage(&imgFaceColor);
	cvReleaseImage(&imgFaceFix);
	LOGD("FaceFixing finish");
	return imgFaceSmooth;
}

void initLocation(CvPoint p[3], int kd[3], int md[3]) {
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

void fittingUserFace(IplImage *model, IplImage *face, IplImage *user,
		CvPoint facePoint[3], int kerDist[3], int extDist[3], IplImage *dest) {
	unsigned char *pMod, *pUsr, *pRef, *pDst;
	pMod = (unsigned char *) model->imageData;
	pUsr = (unsigned char *) user->imageData;
	pRef = (unsigned char *) face->imageData;
	pDst = (unsigned char *) dest->imageData;
	int height = user->height, width = user->width;
	int d[3], dm, di, dim;
	bool kernel = false, far = false;
	float weight, value;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			// 非脸部区域直接用模特图
			if (face->nChannels != 4 || *(pRef + 3) != 0) {
				memcpy(pDst, pMod, model->nChannels);
				pMod += model->nChannels;
				pUsr += user->nChannels;
				pRef += face->nChannels;
				pDst += dest->nChannels;
				continue;
			}

			// 计算到各个特征点的距离、最小距离，同时判断是否在核心区域内，是否在扩展区域外
			dm = 2147483647, dim = 2147483647;
			kernel = false;
			far = true;
			for (int i = 0; i < 3; i++) {
				d[i] = calDistance(cvPoint(x, y), facePoint[i]);
				if (dm > d[i])
					dm = d[i];
				if (d[i] <= kerDist[i])
					kernel = true;
				if (d[i] < extDist[i]) {
					far = false;
					if (d[i] > kerDist[i] && dim > d[i] - kerDist[i]) {
						dim = d[i] - kerDist[i];
						di = i;
					}
				}
			}

			// 在扩展区域外，直接用模特图
			if (far) {
				memcpy(pDst, pMod, model->nChannels);
				pMod += model->nChannels;
				pUsr += user->nChannels;
				pRef += face->nChannels;
				pDst += dest->nChannels;
				continue;
			}

			// 在核心区域内，直接用用户图
			if (kernel) {
				memcpy(pDst, pUsr, user->nChannels);
				pMod += model->nChannels;
				pUsr += user->nChannels;
				pRef += face->nChannels;
				pDst += dest->nChannels;
				continue;
			}

			// 在扩展区域，取加权平均
			weight = (float) (d[di] - kerDist[di])
					/ (extDist[di] - kerDist[di]);
			for (int i = 0; i < 3; i++) {
				value = weight * pMod[i] + (1.0 - weight) * pUsr[i];
				if (value >= 255)
					pDst[i] = 255;
				if (value <= 0)
					pDst[i] = 0;
				else
					pDst[i] = (int) value;
			}

			pMod += model->nChannels;
			pUsr += user->nChannels;
			pRef += face->nChannels;
			pDst += dest->nChannels;
		}
		for (int x = width * model->nChannels; x < model->widthStep; x++)
			pMod++;
		for (int x = width * user->nChannels; x < user->widthStep; x++)
			pUsr++;
		for (int x = width * face->nChannels; x < face->widthStep; x++)
			pRef++;
		for (int x = width * dest->nChannels; x < dest->widthStep; x++)
			pDst++;
	}
}
void transUserSkin(IplImage *imgMod, IplImage *imgUsr, IplImage *imgFace,
		IplImage *imgFaceSkin) {
	// 检测模特脸部肤色
	CvScalar modColor = detectFaceColor(imgMod, imgFace);
	// 检测用户脸部肤色
	CvScalar usrColor = detectFaceColor(imgUsr, imgFace);

	int diff[3];
	for (int i = 0; i < 3; i++)
		diff[i] = (int) modColor.val[i] - usrColor.val[i];

	// 用用户肤色转换为模特肤色
	transFaceColor(imgUsr, imgFace, diff, imgFaceSkin);
}

CvScalar detectFaceColor(IplImage* img, IplImage* ref) {
	CvSize imageSize = cvSize(img->width, img->height);
	IplImage *imgYCrCb = cvCreateImage(imageSize, img->depth, 3);
	cvCvtColor(img, imgYCrCb, CV_BGR2YCrCb);

	int y, cr, cb, x1, y1, value, num = 0;
	unsigned char *pImg, *pYCrCb, *pRef;
	float red = 0, green = 0, blue = 0;

	pImg = (unsigned char *) img->imageData;
	pYCrCb = (unsigned char *) imgYCrCb->imageData;
	pRef = (unsigned char *) ref->imageData;

	int height = img->height, width = img->width;
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			if (ref->nChannels != 4 || *(pRef + 3) != 0) {
				pImg += img->nChannels;
				pRef += ref->nChannels;
				pYCrCb += imgYCrCb->nChannels;
				continue;
			}

			y = *pYCrCb;
			cr = *(pYCrCb + 1);
			cb = *(pYCrCb + 2);
			cb -= 109;
			cr -= 152;
			x1 = (819 * cr - 614 * cb) / 32 + 51;
			y1 = (819 * cr + 614 * cb) / 32 + 77;
			x1 = x1 * 41 / 1024;
			y1 = y1 * 73 / 1024;
			value = x1 * x1 + y1 * y1;

			if ((y < 100 && value < 600) || (y >= 100 && value < 750)
					|| (y >= 150 && value < 850)) {
				blue += *pImg;
				green += *(pImg + 1);
				red += *(pImg + 2);
				num++;
			}

			pImg += img->nChannels;
			pRef += ref->nChannels;
			pYCrCb += imgYCrCb->nChannels;
		}
		for (int i = width * img->nChannels; i < img->widthStep; i++) {
			pImg++;
			pYCrCb++;
		}
		for (int i = width * ref->nChannels; i < ref->widthStep; i++) {
			pRef++;
		}
	}

	cvReleaseImage(&imgYCrCb);

	red /= num;
	green /= num;
	blue /= num;

	return CV_RGB(red, green, blue);
}

void transFaceColor(IplImage* user, IplImage* face, int diff[3],
		IplImage* dest) {
	unsigned char *pUsr, *pRef, *pDst;
	pUsr = (unsigned char *) user->imageData;
	pRef = (unsigned char *) face->imageData;
	pDst = (unsigned char *) dest->imageData;
	int height = user->height, width = user->width;
	int temp;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (face->nChannels != 4 || *(pRef + 3) != 0) {
				memcpy(pDst, pUsr, user->nChannels);
				pUsr += user->nChannels;
				pRef += face->nChannels;
				pDst += dest->nChannels;
				continue;
			}

			for (int j = 0; j < 3; j++) {
				temp = pUsr[j];
				temp += diff[j];
				if (temp >= 255)
					pDst[j] = 255;
				else if (temp <= 0)
					pDst[j] = 0;
				else
					pDst[j] = (unsigned char) temp;
			}

			pUsr += user->nChannels;
			pRef += face->nChannels;
			pDst += dest->nChannels;
		}
		for (int x = width * user->nChannels; x < user->widthStep; x++) {
			pUsr++;
			pDst++;
		}
		for (int x = width * face->nChannels; x < face->widthStep; x++) {
			pRef++;
		}
	}
}
int calDistance(CvPoint p1, CvPoint p2) {
	int x = p1.x - p2.x;
	int y = p1.y - p2.y;
	return (int) sqrt((double) (x * x + y * y));
}

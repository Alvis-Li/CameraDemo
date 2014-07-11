#include "FaceColor.h"

void transUserSkin(IplImage *imgMod, IplImage *imgUsr, IplImage *imgFace, IplImage *imgFaceSkin)
{
	// 检测模特脸部肤色
	CvScalar modColor = detectFaceColor(imgMod, imgFace);
	// 检测用户脸部肤色
	CvScalar usrColor = detectFaceColor(imgUsr, imgFace);

	int diff[3];
	for (int i = 0; i < 3; i++)
		diff[i] = (int)modColor.val[i] - usrColor.val[i];

	// 用用户肤色转换为模特肤色
	transFaceColor(imgUsr, imgFace, diff, imgFaceSkin);
}

CvScalar detectFaceColor(IplImage* img, IplImage* ref)
{
    CvSize imageSize = cvSize(img->width, img->height);  
    IplImage *imgYCrCb = cvCreateImage(imageSize, img->depth, 3);  
    cvCvtColor(img,imgYCrCb,CV_BGR2YCrCb);  

    int y, cr, cb, x1, y1, value, num = 0;  
    unsigned char *pImg, *pYCrCb, *pRef;  
	float red = 0, green = 0, blue = 0;
      
	pImg = (unsigned char *)img->imageData;
    pYCrCb = (unsigned char *)imgYCrCb->imageData;  
	pRef = (unsigned char *)ref->imageData;

    int height = img->height, width = img->width;  
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			if (ref->nChannels != 4 || *(pRef+3) != 0){
				pImg += img->nChannels;
				pRef += ref->nChannels;
				pYCrCb += imgYCrCb->nChannels;
				continue;
			}
        
			y  = *pYCrCb;  
			cr = *(pYCrCb + 1);  
			cb = *(pYCrCb + 2); 
			cb -= 109;  
			cr -= 152  
				;  
			x1 = (819*cr-614*cb)/32 + 51;  
			y1 = (819*cr+614*cb)/32 + 77;  
			x1 = x1*41/1024;  
			y1 = y1*73/1024;  
			value = x1*x1+y1*y1;

			if((y < 100 && value < 500) || (y >= 100 && value < 600))
			{
				blue += *pImg;
				green += *(pImg + 1);
				red += *(pImg + 2);
				num++;
			}

			pImg += img->nChannels;
			pRef += ref->nChannels;
			pYCrCb += imgYCrCb->nChannels;
		}
		for (int i = width*img->nChannels; i < img->widthStep; i++)
		{
			pImg++;
			pYCrCb++;
		}
		for (int i = width*ref->nChannels; i < ref->widthStep; i++)
		{
			pRef++;  
		}
	}  

    cvReleaseImage(&imgYCrCb);  

	red /= num;
	green /= num;
	blue /= num;

	return CV_RGB(red, green, blue);
}

void transFaceColor(IplImage* user, IplImage* face, int diff[3], IplImage* dest)
{  
    unsigned char *pUsr, *pRef, *pDst;  
	pUsr = (unsigned char *)user->imageData;
	pRef = (unsigned char *)face->imageData;
    pDst = (unsigned char *)dest->imageData;  
    int height = user->height, width = user->width;  
	int temp;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (face->nChannels != 4 || *(pRef+3) != 0){
				memcpy(pDst, pUsr, user->nChannels);
				pUsr += user->nChannels;
				pRef += face->nChannels;  
				pDst += dest->nChannels;
				continue;
			}
        
			for (int j = 0; j < 3; j++)
			{
				temp = pUsr[j];
				temp += diff[j];
				if (temp >= 255)
					pDst[j] = 255;
				else if (temp <= 0)
					pDst[j] = 0;
				else
					pDst[j] = (unsigned char)temp;
			}

			pUsr += user->nChannels;
			pRef += face->nChannels;  
			pDst += dest->nChannels;
		}
		for (int x = width*user->nChannels; x < user->widthStep; x++)
		{
			pUsr++;
			pDst++;
		}
		for (int x = width*face->nChannels; x < face->widthStep; x++)
		{
			pRef++;  
		}
	}  
}




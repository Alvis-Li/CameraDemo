#include "FaceFitting.h"
#include "Util.h"

void fittingUserFace(IplImage *model, IplImage *face, IplImage *user, CvPoint facePoint[3], int kerDist[3], int extDist[3], IplImage *dest)
{
    unsigned char *pMod, *pUsr, *pRef, *pDst;  
	pMod = (unsigned char *)model->imageData;
	pUsr = (unsigned char *)user->imageData;
	pRef = (unsigned char *)face->imageData;
    pDst = (unsigned char *)dest->imageData;  
    int height = user->height, width = user->width;
	int d[3], dm, di, dim;
	bool kernel = false, far = false;
	float weight, value;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			// ����������ֱ����ģ��ͼ
			if (face->nChannels != 4 || *(pRef+3) != 0){
				memcpy(pDst, pMod, model->nChannels);
				pMod += model->nChannels;
				pUsr += user->nChannels;
				pRef += face->nChannels;  
				pDst += dest->nChannels;
				continue;
			}
        
			// ���㵽����������ľ��롢��С���룬ͬʱ�ж��Ƿ��ں��������ڣ��Ƿ�����չ������
			dm = 2147483647, dim = 2147483647;
			kernel = false;
			far = true;
			for (int i = 0; i < 3; i++)
			{
				d[i] = calDistance(cvPoint(x, y), facePoint[i]);
				if (dm > d[i]) dm = d[i];
				if (d[i] <= kerDist[i]) kernel = true;
				if (d[i] < extDist[i])
				{ 
					far = false; 
					if (d[i] > kerDist[i] && dim > d[i] - kerDist[i])
					{
						dim = d[i] - kerDist[i]; 
						di = i; 
					}
				}
			}

			// ����չ�����⣬ֱ����ģ��ͼ
			if (far)
			{
				memcpy(pDst, pMod, model->nChannels);
				pMod += model->nChannels;
				pUsr += user->nChannels;
				pRef += face->nChannels;  
				pDst += dest->nChannels;
				continue;
			}

			// �ں��������ڣ�ֱ�����û�ͼ
			if (kernel)
			{
				memcpy(pDst, pUsr, user->nChannels);
				pMod += model->nChannels;
				pUsr += user->nChannels;
				pRef += face->nChannels;  
				pDst += dest->nChannels;
				continue;
			}

			// ����չ����ȡ��Ȩƽ��
			weight = (float)(d[di] - kerDist[di]) / (extDist[di] - kerDist[di]);
			for (int i = 0; i < 3; i++)
			{
				value = weight * pMod[i] + (1.0 - weight) * pUsr[i];
				if (value >= 255)
					pDst[i] = 255;
				if (value <= 0)
					pDst[i] = 0;
				else
					pDst[i] = (int)value;
			}

			pMod += model->nChannels;
			pUsr += user->nChannels;
			pRef += face->nChannels;  
			pDst += dest->nChannels;
		}
		for (int x = width*model->nChannels; x < model->widthStep; x++)
			pMod++;
		for (int x = width*user->nChannels; x < user->widthStep; x++)
			pUsr++;
		for (int x = width*face->nChannels; x < face->widthStep; x++)
			pRef++;  
		for (int x = width*dest->nChannels; x < dest->widthStep; x++)
			pDst++;
	}  
}

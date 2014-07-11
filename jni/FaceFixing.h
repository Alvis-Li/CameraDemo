#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

void FaceFix2File(char *fileMod, char *fileUser, char *fileFace, CvPoint facePoint[3], char *fileFix);
void FaceFix2Memo(char *fileMod, char *fileUser, char *fileFace, CvPoint facePoint[3], char *buffer);
IplImage* FaceFixing(char *fileMod, char *fileUser, char *fileFace, CvPoint facePoint[3]);
void initLocation(CvPoint p[3], int kd[3], int md[3]);
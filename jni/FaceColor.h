#include <opencv2/opencv.hpp>  
#include <opencv2/highgui/highgui.hpp>  

void transUserSkin(IplImage *imgMod, IplImage *imgUsr, IplImage *imgFace, IplImage *imgFaceSkin);
CvScalar detectFaceColor(IplImage* img, IplImage* ref);
void transFaceColor(IplImage* user, IplImage* face, int diff[3], IplImage* dest);
#include <opencv2/opencv.hpp>  
#include <opencv2/highgui/highgui.hpp>

void fittingUserFace(IplImage *model, IplImage *face, IplImage *user, CvPoint facePoint[3], int kerDist[3], int farDist[3], IplImage *dest);
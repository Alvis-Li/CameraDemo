#include <math.h>
#include "Util.h"

int calDistance(CvPoint p1, CvPoint p2)
{
	int x = p1.x - p2.x;
	int y = p1.y - p2.y;
	return (int)sqrt((double)(x * x + y * y));
}

	
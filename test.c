#include<opencv/highgui.h>

int main(void)
{
	cvNamedWindow("test",1);
	char key=cvWaitKey(0);
	cvDestroyWindow("test");
	return 0;
}

#include <math.h>
void fcMap(double *inx,double *outx){
	double x1=inx[0];
	double x2=inx[1];
	double x3=inx[2];
	double x4=inx[3];
	double x5=inx[4];
	double x6=inx[5];
	double x7=inx[6];
	outx[0]=(-2.9)*x3+x7+sin(02.05*x7+sin(9.09*x4+sin(x3+(-51.43)*x5+sin(((-10.9)/02.05)*x7))));
	outx[1]=((-85.8)/(-9.702))*x4+cos(6.353*x4+x1);
}

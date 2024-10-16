#include <math.h>
void fcMap(double *inx,double *outx){
	double x1=inx[0];
	double x2=inx[1];
	double x3=inx[2];
	double x4=inx[3];
	double x5=inx[4];
	double x6=inx[5];
	double x7=inx[6];
	outx[0]=((-8.936)*x3/(8.221*x7-5.079*x3));
	outx[1]=abs((x7/((-9.594)*x3)));
}

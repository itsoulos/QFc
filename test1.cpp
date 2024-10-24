#include <math.h>
void fcMap(double *inx,double *outx){
	double x1=inx[0];
	double x2=inx[1];
	double x3=inx[2];
	double x4=inx[3];
	outx[0]=(x2-x3+x1);
	outx[1]=(1.800*x4-cos(abs(((-7.561)*x4))));
}

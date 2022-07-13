#include <math.h>
void fcMap(double *inx,double *outx){
	double x1=inx[0];
	double x2=inx[1];
	double x3=inx[2];
	double x4=inx[3];
	double x5=inx[4];
	double x6=inx[5];
	outx[0]=(x3-(x2+((-99.999)*x1+((-9.96)*x4+((-22.4)*x5)))));
	outx[1]=((-79.009)/2.00)*x2+4.150*x5+x4+662.9*x6;
}

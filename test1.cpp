#include <math.h>
void fcMap(double *inx,double *outx){
	double x1=inx[0];
	double x2=inx[1];
	double x3=inx[2];
	double x4=inx[3];
	outx[0]=(0.50*x3);
	outx[1]=(27.5*x3*(042.38*x3*((-06.305)/(-852.39))*x1+4.75*x2+(440.23/(-64.651))*x3));
}

#include <math.h>
void fcMap(double *inx,double *outx){
	double x1=inx[0];
	double x2=inx[1];
	double x3=inx[2];
	double x4=inx[3];
	double x5=inx[4];
	double x6=inx[5];
	double x7=inx[6];
	double x8=inx[7];
	double x9=inx[8];
	double x10=inx[9];
	double x11=inx[10];
	double x12=inx[11];
	double x13=inx[12];
	outx[0]=(409.785*x7-(52.610/1.58)*x4+(-55.1)*x10+((-8.94)/1.852)*x5);
	outx[1]=(x13-456.3*x11+(-129.27)*x9+216.83*x1+((-099.7)/(-00.745))*x3);
}

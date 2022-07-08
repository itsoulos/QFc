#include <math.h>
void fcMap(double *inx,double *outx){
	double x1=inx[0];
	double x2=inx[1];
	double x3=inx[2];
	double x4=inx[3];
	double x5=inx[4];
	double x6=inx[5];
	double x7=inx[6];
	outx[0]=exp((5.4*x1/(5.3*x4*(38.599*x1*(x7*cos(((-55.68)/5.4)*x1))))));
	outx[1]=((-88.007)*x3/(9.998*x7+cos(((-8.81)*x5*sqrt((783.138*x2))))));
}

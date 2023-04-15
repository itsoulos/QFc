#include <math.h>
void fcMap(double *inx,double *outx){
	double x1=inx[0];
	double x2=inx[1];
	outx[0]=(x1*((-2.208)*x1));
	outx[1]=((-6.83)*x1/abs(1.988*x2+1.08*x2+log(x1)));
}

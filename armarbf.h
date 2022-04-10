#ifndef ARMARBF_H
#define ARMARBF_H
# include <model.h>
# include <math.h>
# include <stdlib.h>
# include <vector>
# include <stdio.h>
# include <iostream>
# include <QJsonObject>
# include <QFile>
# include <QTextStream>
# include <omp.h>
#include <adept.h>
#include <armadillo>
using namespace std;
class ArmaRbf : public Model
{
public:
    ArmaRbf(Mapper *m);
    virtual	double 	train1();
    virtual double	train2();
    virtual double	output(Matrix x);
    virtual void	getDeriv(Matrix x,Matrix &g);
    virtual double	funmin(Matrix x);
    arma::vec train( vector<double> &x );
    double neuronOutput( vector<double> &x, vector<double> &patt, unsigned pattDim, unsigned offset);
};

#endif // ARMARBF_H

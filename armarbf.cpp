#include "armarbf.h"

ArmaRbf::ArmaRbf(Mapper *m)
    :Model(m)
{

}

double 	ArmaRbf::train1()
{
    weight.resize((pattern_dimension+1)*num_weights);
    lmargin.resize(weight.size());
    rmargin.resize(weight.size());
    for(int i=0;i<(int)weight.size();i++)
    {
        lmargin[i]=-100.0;
        rmargin[i]= 100.0;
    }
    for(int i=0;i<(int)xpoint.size();i++)
    {
        mapper->map(origx[i],xpoint[i]);
    }
    return funmin(weight);
}

double	ArmaRbf::train2()
{
   return train1();
}

double	ArmaRbf::output(Matrix x)
{
    Matrix pattern = x;
    arma::vec neuronOuts(num_weights);
    arma::vec Linear = train(weight);

    for(int j = 0; j < num_weights;j++)
    {
              neuronOuts[j] = neuronOutput(x,pattern,pattern.size(),j);
    }
    double tempOut = arma::dot(neuronOuts,Linear);
    return tempOut;
}

void	ArmaRbf::getDeriv(Matrix x,Matrix &g)
{

}

int icount=0;
double ArmaRbf::neuronOutput( vector<double> &x, vector<double> &patt, unsigned pattDim, unsigned offset ){
    double out = 0;
    for(unsigned i = 0; i < pattDim;i++){
        out += (patt[i] - x[offset*pattDim + i]) * (patt[i] - x[offset*pattDim + i]);
    }
    double df=(-out / (x[num_weights*pattDim+offset] * x[num_weights*pattDim+offset]) );
    if(fabs(df)>100) return 1e+8;

    return exp(df);
}

arma::vec ArmaRbf::train( vector<double> &x ){
    arma::mat A = arma::zeros(xpoint.size(),num_weights);
    arma::vec B(ypoint.size());
    for(unsigned i = 0; i < ypoint.size(); i++){
        B.at(i) = ypoint[i];
    icount = 0;
        for(unsigned j = 0; j < num_weights;j++){
            A.at(i,j) = neuronOutput(x, xpoint[i], xpoint[0].size() , j);

        }
    }

    arma::vec RetVal;
    try{
    RetVal=arma::vec(arma::pinv(A)*B);
   //RetVal=arma::vec(arma::pinv(A,1e-10,"dc")*B);
    }catch(std::runtime_error & e)
    {
        RetVal = arma::zeros(arma::size(RetVal));
    }
    if(RetVal.has_nan() || RetVal.has_inf()) {
        RetVal = arma::zeros(arma::size(RetVal));
        }
    return RetVal;
}


double	ArmaRbf::funmin(Matrix x)
{
    double errorSum=0.0;
    arma::vec Linear = train(x);


        for(unsigned i = 0; i < xpoint.size(); i++){
            Matrix pattern = xpoint[i];
            arma::vec neuronOuts(num_weights);
        icount=0;
            for(unsigned j = 0; j < num_weights;j++){
                neuronOuts[j] = neuronOutput(x,pattern,pattern.size(),j);
            }
            double tempOut = arma::dot(neuronOuts,Linear);
            errorSum += ( tempOut - ypoint[i] ) * ( tempOut - ypoint[i] );
        }

      return errorSum;
}

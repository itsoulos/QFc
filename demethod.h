#ifndef DEMETHOD_H
#define DEMETHOD_H
# include <problem.h>
# include <vector>
# include <QString>
using namespace std;

class DeMethod
{
private:
    Problem *myProblem;
    double **agentx;
    double *agenty;
    Matrix xx;
    double *F;
    double de_fvalue = 0.8;
    double de_crvalue = 0.9;
    double de_maxiters = 200;
    int     de_iter = 0;
    double  de_lrate=0.00;//local search rate
    int NP; //number of agents
    QString de_fmethod="number";//available values: number,random,adaptive,migrant
public:
    DeMethod(Problem *p,int np);
    void    setLocalSearchRate(double lr);
    double  getLocalSearchRate() const;
    void    setFMethod(QString s);
    QString  getFMethod() const;
    void    Solve();
    void    setFValue(double f);
    double  getFValue() const;
    void    setCRValue(double cr);
    double  getCRValue() const;
    double    localSearch(Matrix &x);
    double  sumFitness() const;
    void    calculateMigrantWeights();
    int     tournament();
    double  fitness(double *y);
    void    setMaxIters(int m);
    int     getMaxIters() const;
    double  getAdaptiveWeight();
    void    getBest(Matrix&x,double &y);
    ~DeMethod();
};

#endif // DEMETHOD_H

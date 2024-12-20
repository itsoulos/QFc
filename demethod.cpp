#include "demethod.h"
# include "model.h"
# include "kmeans.h"
DeMethod::DeMethod(Problem *p,int np)
{
    myProblem = p;
    NP = np;
    agentx=new double*[np *sizeof(double *)];
    agenty=new double[np *sizeof(double)];
    F     =new double[np *sizeof(double)];
    Model *m = (Model *)myProblem;
    extern QString de_usekmeans;
    if(de_usekmeans == "no")

    for(int i=0;i<np;i++)
    {
        agentx[i]=new double[myProblem->getDimension()];
        for(int j=0;j<myProblem->getDimension();j++)
            agentx[i][j]=(2.0*m->randomDouble()-1.0);
    }
    else{
    //perform kmeans here
    int K = np;
    int nsamples = 10 * K;
    int n = myProblem->getDimension();
    double *data_vectors = new double[nsamples *n];
    double *centers = new double[K *n];
    double *variances = new double[K*n];
    for(int i=0;i<nsamples;i++)
    {
        for(int j=0;j<n;j++)
            data_vectors[i*n+j]=2.0*m->randomDouble()-1.0;
    }
    Kmeans(m,data_vectors,centers,variances,nsamples,n,K);

    for(int i=0;i<K;i++)
    {
        agentx[i]=new double[myProblem->getDimension()];

        for(int j=0;j<n;j++)
        {
            agentx[i][j]=centers[i*n+j];
        }
    }
    delete[] data_vectors;
    delete[] centers;
    delete[] variances;
    }
    xx.resize(myProblem->getDimension());
}
void    DeMethod::setFValue(double f)
{
    if(f>=0.0 && f<=1.0)
    de_fvalue =f ;
}

double  DeMethod::getFValue() const
{
    return de_fvalue;
}

void    DeMethod::setCRValue(double cr)
{
    if(cr>=0.0 && cr<=1.0)
    de_crvalue = cr;
}

double  DeMethod::getCRValue() const
{
    return de_crvalue;
}

void    DeMethod::setLocalSearchRate(double lr)
{
    if(lr>=0.0 && lr<=1.0)
    de_lrate = lr;
}

double  DeMethod::getLocalSearchRate() const
{
    return de_lrate;
}

void    DeMethod::setFMethod(QString s)
{
    de_fmethod = s;
}

double  DeMethod::sumFitness() const
{
    double sum = 0.0;
    for(int i=0;i<NP;i++)
        sum+=fabs(agenty[i]);
    return sum;
}

QString  DeMethod::getFMethod() const
{
    return de_fmethod;
}
void DeMethod::calculateMigrantWeights()
{
    double maxFitness=agenty[0];
    double sumW = 0.0;

    vector<double> weights;
    weights.resize(NP);
    for (int i = 0; i < NP; ++i)
    {
        if(agenty[i]>maxFitness)
            maxFitness = agenty[i];
    }

    for (int i = 0; i < NP; ++i)
    {
        double d = maxFitness - agenty[i];
        weights[i] = d;
        sumW += d;
    }


    for (int i = 0; i < NP; ++i)
    {
        F[i] = weights[i] / sumW;
    }
}

int     DeMethod::tournament()
{
    const int tsize=8;
    double bestf = 1e+100;
    int bestindex = -1;
    for(int i=0;i<tsize;i++)
    {
        int pos = rand() % NP;
        if(agenty[pos]<bestf)
        {
            bestf = agenty[pos];
            bestindex = pos;
        }
    }
    return bestindex;
}

void    DeMethod::Solve()
{

    //init
    bool is_frandom =false;
    de_iter = 0;
    int best_index = -1;
    double besty=1e+100;
    Matrix trialx;
    Neural *m = (Neural *)myProblem;
    double oldSumFitness = 1e+100;
    int stopcount = 0;
    int stoplimit = 5;
    double old_best_fitness=1e+100;
    trialx.resize(myProblem->getDimension());
    do{
        de_iter++;
        besty = agenty[0];
        best_index =0;
        for(int i=0;i<NP;i++)
        {
            agenty[i]=fitness(agentx[i]);
            if(agenty[i]<besty)
            {
                besty = agenty[i];
                best_index = i;
            }
        }
        if(de_fmethod=="number")
        {
            for(int i=0;i<NP;i++)
                F[i]=de_fvalue;
        }
        else
        if(de_fmethod=="random")
        {
            is_frandom = true;
        }
        else
        if(de_fmethod =="adaptive")
        {
            for(int i=0;i<NP;i++)
                F[i]=getAdaptiveWeight();
        }
        else
        if(de_fmethod == "migrant")
        {
            calculateMigrantWeights();
        }
        for(int i=0;i<NP;i++)
        {
            int randomA, randomB, randomC;
            do{
                randomA=tournament();
                randomB=tournament();
                randomC=tournament();
            }while(randomA==randomB || randomB==randomC || randomC == randomA);
            int R = rand() % myProblem->getDimension();
            for(int j=0;j<myProblem->getDimension();j++)
            {
                double rj = m->randomDouble();
                if (rj < de_crvalue || j == R)
                {
                    if(is_frandom)
                        F[i]=-0.5 + 2.0 * m->randomDouble();
                    trialx[j]=agentx[randomA][j]+F[i]*(agentx[randomB][j]-agentx[randomC][j]);
                }
                else trialx[j]=agentx[i][j];
            }
            //find min distance with other agents
            double minDist = 1e+100;
            for(int k=0;k<NP;k++)
            {
                double sx = 0.0;
                for(int l=0;l<myProblem->getDimension();l++)
                {
                    sx+=pow(trialx[l]-agentx[k][l],2.0);
                }
                sx/=myProblem->getDimension();
                if(sx<minDist) minDist = sx;
            }
            m->setWeights(trialx);
            //double viol = m->countViolate(10.0);

            double ft=0.0;
            if(de_lrate>0.0 && minDist>1e-4  && m->randomDouble()<=de_lrate)
                ft = localSearch(trialx);
            else
                ft = myProblem->funmin(trialx);
            if(ft<agenty[i])
            {
                agenty[i]=ft;
                for(int j=0;j<myProblem->getDimension();j++)
                    agentx[i][j]=trialx[j];
            }
            if(agenty[i]<besty) besty=agenty[i];
	    if(fabs(agenty[i])<1e-6) return;
        }
        double new_sum = sumFitness();
        if(fabs(besty-old_best_fitness)<1e-5) stopcount++; else stopcount=0;
        old_best_fitness = besty;
        if(de_iter%20==0)
        printf("DE. Iter=%4d Diff in fitness %20.10lg Best fitness:%20.10lg\n",
               de_iter,fabs(oldSumFitness - new_sum),agenty[best_index]);
        oldSumFitness = new_sum;
        if(stopcount>=stoplimit) break;
	
    }while(de_iter<=de_maxiters);
    //done
}
double    DeMethod::localSearch(Matrix& x)
{
    MinInfo Info;
    Matrix left,right;
    left.resize(x.size());
    right.resize(x.size());
    for(int i=0;i<x.size();i++)
    {
        left[i]=-2.0 *fabs(x[i]);
        right[i]= 2.0 *fabs(x[i]);
    }
    myProblem->setLeftMargin(left);
    myProblem->setRightMargin(right);
    Info.p=myProblem;
    Info.iters=2001;
    double v=tolmin(x,Info);
    return v;
}
double  DeMethod::getAdaptiveWeight()
{
    const double fmin = 0.2;
    const double fmax = 0.9;
    return fmax - (fmax - fmin )*(de_iter*1.0/de_maxiters)*(de_iter*1.0/de_maxiters);
}

double  DeMethod::fitness(double *y)
{
    for(int i=0;i<myProblem->getDimension();i++)
        xx[i]=y[i];
    return myProblem->funmin(xx);
}

void    DeMethod::setMaxIters(int m)
{
    de_maxiters = m;
}

int     DeMethod::getMaxIters() const
{
    return de_maxiters;
}


void    DeMethod::getBest(Matrix&x,double &y)
{
    double besty = agenty[0];
    int best_index =0;
    for(int i=0;i<NP;i++)
    {
        agenty[i]=fitness(agentx[i]);
        if(agenty[i]<besty)
        {
            besty = agenty[i];
            best_index = i;
        }
    }
    y  = besty;
    for(int i=0;i<myProblem->getDimension();i++)
        x[i]=agentx[best_index][i];
}

DeMethod::~DeMethod()
{
    for(int i=0;i<NP;i++)
        delete[] agentx[i];
    delete[] agentx;
    delete[] agenty;
    delete[] F;
}

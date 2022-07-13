# include <neural.h>
# include <math.h>
# include <gensolver.h>
# include <QDebug>
# include <grs.h>
# include <lbfgs.h>
int pass=0;
double maxx=-1e+100;

static double sig(double x)
{
	return 1.0/(1.0+exp(-x));
}

static double sigder(double x)
{
	double s=sig(x);
	return s*(1-s);
}

Neural::Neural(Mapper *m):Model(m)
{
    model_type = MODEL_NEURAL;
    localSearchMethod="bfgs";//available options are: bfgs, genetic, conj
}
void Neural::enableWeightDecay()
{
    weigh_decay_flag=true;
}

void	Neural::setWeights(Matrix x)
{
	for(int i=0;i<weight.size();i++) weight[i]=x[i];
}

double Neural::train1()
{
	for(int i=0;i<xpoint.size();i++) 
	{
		int d=mapper->map(origx[i],xpoint[i]);
		if(!d) return 1e+100;
		for(int j=0;j<pattern_dimension;j++)
		{
			if(isinf(xpoint[i][j])) return 1e+100;
		}
	}
	double v;
	MinInfo Info;
	Info.p = this;
    Info.iters=10;
     GenSolve(this,weight,v,0,0);
     return v;
	return tolmin(weight,Info);

	double oldval=funmin(weight);
	Matrix saveweight;
	saveweight.resize(weight.size());
	saveweight=weight;
	for(int i=1;i<=200;i++)
	{
		v=tolmin(weight,Info);
		v=v*(1.0+100.0*countViolate(10.0));
		if(v>oldval)
		{
			weight=saveweight;
			v=oldval;
			break;
		}
		oldval=v;
		saveweight=weight;
	}
	v=funmin(weight);
	return v;
}

double	Neural::countViolate(double limit)
{
	//metraei posoi komboi prokaloyn problima sta sigmoeidi
	int count = 0;
	int nodes=weight.size() / (pattern_dimension + 2);
	for(int i=0;i<xpoint.size();i++)
	{
		for(int n=1;n<=nodes;n++)
		{
              		double arg = 0.0;
              		for(int j=1;j<=pattern_dimension;j++)
              		{
                 	 int pos=(pattern_dimension+2)*n-(pattern_dimension+1)+j-1;
                  	 arg+=weight[pos]*xpoint[i][j-1];
			}
              	arg+=weight[(pattern_dimension+2)*n-1];
		count+=(fabs(arg)>=limit);
	      }
	}
	return count*1.0/(xpoint.size()*nodes);
}


double leve_func(double *par, int x, void *fdata)
{
    Neural *neural = (Neural *)fdata;
    Matrix w;
    w.resize(neural->getDimension());
    for(unsigned int i=0;i<w.size();i++) w[i]=par[i];
    neural->setWeights(w);
    double v= neural->getValueAtPos(x);
    return v;
}

void leve_gradient(double *g, double *par, int x, void *fdata)
{
    Neural *neural = (Neural *)fdata;
    Matrix w;
    w.resize(neural->getDimension());
    for(unsigned int i=0;i<w.size();i++) w[i]=par[i];
    neural->setWeights(w);
    neural->gradientAtPos(x,w);
    for(unsigned int i=0;i<w.size();i++) g[i]=w[i];
}

double  Neural::getValueAtPos(int pos)
{
    return output(xpoint[pos]);
}

void    Neural::gradientAtPos(int pos,Matrix &g)
{
    getDeriv(xpoint[pos],g);
}



double    Neural::localSearch()
{
    randomizeWeights();
    lmargin.resize(weight.size());
    rmargin.resize(weight.size());
    for(int i=0;i<weight.size();i++)
    {
        weight[i]=0.01 * (2.0*drand48()-1.0);
        lmargin[i]=-100.0;
        rmargin[i]= 100.0;
    }

    if(localSearchMethod=="bfgs")
    {
        MinInfo Info;
        Info.p=this;
        Info.iters=2001;
        return tolmin(weight,Info);
    }
    else
    if(localSearchMethod=="lbfgs")
    {
        Lbfgs  l(this);
        return l.Solve(weight);
    }
    else
    if(localSearchMethod=="genetic")
    {
        double v;
        if(weigh_decay_flag)
        {
            GenSolve(this,weight,v,0,1);
        }
        else
        GenSolve(this,weight,v,0,0);
        for(int i=0;i<weight.size();i++)
        {

            lmargin[i]=-2.0*fabs(weight[i]);
            rmargin[i]= 2.0*fabs(weight[i]);
        }
        MinInfo Info;
        Info.p=this;
        Info.iters=2001;
        v=tolmin(weight,Info);
        return v;
    }
    else
    if(localSearchMethod=="grs")
    {
        Grs *Solver=new Grs(this);
        Solver->setGenomeCount(200);
        Solver->setGenomeLength(50 * getDimension());
        double v=funmin(weight);
        Solver->Solve(weight,v);
        delete Solver;
        return v;
    }

}


double Neural::train2()
{
	double v;
	for(int i=0;i<xpoint.size();i++) 
	{
		mapper->map(origx[i],xpoint[i]);
	}
    return localSearch();
}


double Neural::output(Matrix x)
{
	double arg=0.0,per=0.0;
	int dimension = pattern_dimension;
	int nodes=weight.size()/(pattern_dimension+2);
        for(int i=1;i<=nodes;i++)
        {
              arg = 0.0;
              for(int j=1;j<=dimension;j++)
              {
                  int pos=(dimension+2)*i-(dimension+1)+j-1;
                  arg+=weight[pos]*x[j-1];
              }
	      
              arg+=weight[(dimension+2)*i-1];
              per+=weight[(dimension+2)*i-(dimension+1)-1]*sig(arg);
        }
        return per;
}

void    Neural::setLocalSearchMethod(QString s)
{
    localSearchMethod=s;
}

QString  Neural::getLocalSearchMethod( ) const
{
    return localSearchMethod;
}

void   Neural::getDeriv(Matrix x,Matrix &g)
{
  	double arg;
        double s;
        int nodes=weight.size()/(pattern_dimension+2);
	int dimension = pattern_dimension;
        double f,f2;
        for(int i=1;i<=nodes;i++)
        {
                arg = 0.0;
                for(int j=1;j<=dimension;j++)
                {
                  	int pos=(dimension+2)*i-(dimension+1)+j-1;
                        arg+=weight[pos]*x[j-1];
                }
                arg+=weight[(dimension+2)*i-1];
                f=sig(arg);
		f2=f*(1.0 -f );
                g[(dimension+2)*i-1]=weight[(dimension+2)*i-(dimension+1)-1]*f2;
                g[(dimension+2)*i-(dimension+1)-1]=f;
                for(int k=1;k<=dimension;k++)
                {
                        g[(dimension+2)*i-(dimension+1)+k-1]=
                             x[k-1]*f2*weight[(dimension+2)*i-(dimension+1)-1];
                }
        }
}


Neural::~Neural()
{
}

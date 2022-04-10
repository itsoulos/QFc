# include <neural.h>
# include <math.h>
# include <gensolver.h>
# include <gsl/gsl_multimin.h>
# include <levmarq.h>
#include <unsupported/Eigen/NonLinearOptimization>
#include <Eigen/Eigen>
# include <QDebug>
# include <grs.h>
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
    Info.iters=25;
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

double
my_f (const gsl_vector *v, void *params)
{
  Neural *p = (Neural *)params;

  Matrix xx;
  xx.resize(p->getDimension());
  for(int i=0;i<p->getDimension();i++)
  {
     xx[i]=gsl_vector_get(v,i);
  }
  return p->funmin(xx);

}

/* The gradient of f, df = (df/dx, df/dy). */
void
my_df (const gsl_vector *v, void *params,
       gsl_vector *df)
{
    Neural *p = (Neural *)params;

    Matrix xx,gg;
    xx.resize(p->getDimension());
    gg.resize(p->getDimension());
    for(int i=0;i<p->getDimension();i++)
    {
       xx[i]=gsl_vector_get(v,i);
    }
    p->granal(xx,gg);
    for(int i=0;i<p->getDimension();i++)
    {
        gsl_vector_set(df,i,gg[i]);
    }
}

/* Compute both f and df together. */
void
my_fdf (const gsl_vector *x, void *params,
        double *f, gsl_vector *df)
{
  *f = my_f(x, params);
  my_df(x, params, df);
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


struct LMFunctor
{
    // 'm' pairs of (x, f(x))
    Eigen::MatrixXf measuredValues;
    Neural *neural;


    // Compute 'm' errors, one for each data point, for the given parameter values in 'x'
    int operator()(const Eigen::VectorXf &x, Eigen::VectorXf &fvec) const
    {
        // 'x' has dimensions n x 1
        // It contains the current estimates for the parameters.

        /** to x gia mena einai ta params tou neuronikou **/
        // 'fvec' has dimensions m x 1
        // It will contain the error for each data point.

        /** to fvec gia mena einai to sfalma /point **/

        Matrix w(x.size());
        double total_error =0.0;
        for(int i=0;i<x.size();i++) w[i]=x(i);
        neural->setWeights(w);
        for (int i = 0; i < values(); i++) {

            fvec(i) = pow(neural->getValueAtPos(i) -neural->getYPoint(i),2.0);
            total_error+=fvec(i)*fvec(i);

        }
        printf("Total Error = %lf \n",total_error);
        if(total_error<neural->bestValue)
        {
            neural->bestValue=total_error;
            neural->bestWeight=w;
        }
        return 0;
    }

    // Compute the jacobian of the errors
    int df(const Eigen::VectorXf &x, Eigen::MatrixXf &fjac) const
    {
        // 'x' has dimensions n x 1
        // It contains the current estimates for the parameters.

        // 'fjac' has dimensions m x n
        // It will contain the jacobian of the errors, calculated numerically in this case.
/*
       Data w(x.size());
        for(int i=0;i<x.size();i++) w[i]=x(i);
        Data g(x.size());
             Eigen::VectorXf fvec(values(),1);

        for (int i = 0; i < values(); i++) {

            fvec(i) = pow(dgetValue(w,trainx[i],fcount)-trainy[i],2.0);
        }

        for (int i = 0; i < values(); i++) {
            getDeriv(w,trainx[i],g);
            for(int j=0;j<w.size();j++)
            {
                fjac(i,j)=2.0*fvec(i)*g[j];
            }
        }

    return 0;*/
        float epsilon;
        epsilon = 1e-5f;

        for (int i = 0; i < x.size(); i++) {
            Eigen::VectorXf xPlus(x);
            xPlus(i) += epsilon;
            Eigen::VectorXf xMinus(x);
            xMinus(i) -= epsilon;

            Eigen::VectorXf fvecPlus(values());
            operator()(xPlus, fvecPlus);

            Eigen::VectorXf fvecMinus(values());
            operator()(xMinus, fvecMinus);

            Eigen::VectorXf fvecDiff(values());
            fvecDiff = (fvecPlus - fvecMinus) / (2.0f * epsilon);

            fjac.block(0, i, values(), 1) = fvecDiff;
        }

        return 0;
    }

    // Number of data points, i.e. values.
    int m;

    // Returns 'm', the number of values.
    int values() const { return m; }

    // The number of parameters, i.e. inputs.
    int n;

    // Returns 'n', the number of inputs.
    int inputs() const { return n; }

};


double    Neural::localSearch()
{
    randomizeWeights();
    lmargin.resize(weight.size());
    rmargin.resize(weight.size());
    for(int i=0;i<weight.size();i++)
    {
        weight[i]=0.01 * drand48();
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
    if(localSearchMethod=="genetic")
    {
        double v;
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
    if(localSearchMethod=="leve")
    {
        Eigen::VectorXf x(weight.size());
           for(int i=0;i<weight.size();i++)
               x(i)=weight[i];//0.1*(2.0*drand48()-1.0);

           this->bestValue=1e+100;

           LMFunctor functor;
           functor.m = ypoint.size();
           functor.n = weight.size();

           functor.neural=this;

           Eigen::LevenbergMarquardt<LMFunctor, float> lm(functor);
           int status =lm.minimize(x);

           weight = this->bestWeight;
           return this->bestValue;
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
    else
    if(localSearchMethod=="conj")
    {
        randomizeWeights();
        size_t iter = 0;
        int status;

        const gsl_multimin_fdfminimizer_type *T;
        gsl_multimin_fdfminimizer *s;

        gsl_vector *x;
        gsl_multimin_function_fdf my_func;

        my_func.n = weight.size();
        my_func.f = my_f;
        my_func.df = my_df;
        my_func.fdf = my_fdf;
        my_func.params = this;

        x = gsl_vector_alloc (weight.size());
        for(int i=0;i<weight.size();i++)
            gsl_vector_set (x, i, weight[i]);

        T = gsl_multimin_fdfminimizer_conjugate_pr;// gsl_multimin_fdfminimizer_conjugate_fr;
        s = gsl_multimin_fdfminimizer_alloc (T, weight.size());

        gsl_multimin_fdfminimizer_set (s, &my_func, x, 0.001, 1e-7);
        do
          {
            iter++;
            status = gsl_multimin_fdfminimizer_iterate (s);

            if (status)
              break;

            status = gsl_multimin_test_gradient (s->gradient, 1e-7);

            if (status == GSL_SUCCESS)
              printf ("Minimum found at:\n");

            printf ("%5d %.5f %.5f %10.5f\n", iter,
                    gsl_vector_get (s->x, 0),
                    gsl_vector_get (s->x, 1),
                    s->f);

          }
        while (status == GSL_CONTINUE && iter < 1000);

        double v=s->f;
        gsl_multimin_fdfminimizer_free (s);
        for(int i=0;i<weight.size();i++) weight[i]=gsl_vector_get(x,i);
        gsl_vector_free (x);
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

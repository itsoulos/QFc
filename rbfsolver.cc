# include <rbfsolver.h>
# include <stdlib.h>
# include <string.h>
# include <rbf_model.h>
# include <iostream>
# include <math.h>
# include <stdio.h>

# define MAX_RULE	256
# define CRAND
# define MAXWEIGHT	50

double xrand()
{
	return rand() *1.0/RAND_MAX;
}

RbfSolver::RbfSolver(int gcount,Problem *p,double mx,int ff)
{
    extern int randomSeed;
	small_tolmin_flag=ff;
    srand(randomSeed);
	maxx=mx;
	problem = p;
	genome_size= 2 * MAXWEIGHT;
	elitism=1;
	selection_rate = 0.2;
	mutation_rate  = 0.05;
	genome_count   = gcount;
	generation     = 0;

	double f;
	genome=new double*[genome_count];
	children=new double*[genome_count];
	for(int i=0;i<genome_count;i++)
	{
		genome[i]=new double[genome_size];
		children[i]=new double[genome_size];
		for(int j=0;j<genome_size;j++)
		{
			if(j<MAXWEIGHT) genome[i][j]=xrand()<0.5?0.0:1.0;
			else
			genome[i][j]=0.1 * (2.0*xrand()-1.0);
		}
				
	}
	fitness_array=new double[genome_count];
}

void	RbfSolver::reset()
{
}

double 	RbfSolver::fitness(double *g)
{
	Rbf *nn=(Rbf *)problem;
 	double v=-nn->setWeightValuesFromPattern(g,genome_size);
	return v;
}

double 	RbfSolver::fitness(vector<double> &g)
{
/*
if(generation%5==0 && rand() % genome_count<=10)
	{
	Matrix w;
	w.resize(g.size());
	for(int i=0;i<g.size();i++) w[i]=g[i];
	double v;
	
	MinInfo Info;
	Info.p=nn;
	Info.iters=10;
	
	v=tolmin(w,Info);
	for(int i=0;i<g.size();i++) g[i]=w[i];
//	v=-v*(1.0+100.0*nn->countViolate(20.0));
	return -v;
}

else*/
 return -problem->funmin(g);
}

void	RbfSolver::select()
{
	double itemp[genome_size];
	for(int i=0;i<genome_count;i++)
	{
		for(int j=0;j<genome_count-1;j++)
		{
			if(fitness_array[j+1]>fitness_array[j])
			{
				double dtemp;
				dtemp=fitness_array[j];
				fitness_array[j]=fitness_array[j+1];
				fitness_array[j+1]=dtemp;
				
				memcpy(itemp,genome[j],genome_size*sizeof(double));
				memcpy(genome[j],genome[j+1],genome_size*sizeof(double));
				memcpy(genome[j+1],itemp,genome_size*sizeof(double));
			}
		}
	}
}

void	RbfSolver::crossover()
{
        int parent[2];
        int nchildren=(int)((1.0 - selection_rate) * genome_count);
	if(!(nchildren%2==0)) nchildren++;
        const int tournament_size =(genome_count<=100)?4:10;
        int count_children=0;
        while(1)
        {
                //epilogi ton goneon
                for(int i=0;i<2;i++)
                {
			
                        double max_fitness=-1e+10;
                        int    max_index=-1;
			int r;
                        for(int j=0;j<tournament_size;j++)
                        {
				r=rand() % (genome_count);
                                if(j==0 || fitness_array[r]>max_fitness)
                                {
                                        max_index=r;
                                        max_fitness=fitness_array[r];
                                }
                        }
                        parent[i]=max_index;
			
                }
		
		for(int i=0;i<genome_size;i++)
		{
			double a =xrand();
			children[count_children][i]=a*genome[parent[0]][i]+
				(1.0-a)*genome[parent[1]][i];
			children[count_children+1][i]=a*genome[parent[1]][i]+
				(1.0-a)*genome[parent[0]][i];
		}
		
		count_children+=2;
		if(count_children>=nchildren) break;
	}
	
	
	for(int i=0;i<nchildren;i++)
	{
		memcpy(genome[genome_count-i-1],
				children[i],genome_size * sizeof(double));
	}
}

void	RbfSolver::setElitism(int s)
{
	elitism = s;
}

void	RbfSolver::mutate()
{
	int start = elitism * (int)(genome_count*selection_rate);
	start = 1;
	for(int i=start;i<genome_count;i++)
	{
		for(int j=0;j<genome_size;j++)
		{
			double r=xrand();
			if(r<mutation_rate)
			{
			double percent=exp(-generation * 1.0/2.0);
				percent = 0.25;
				genome[i][j]*=(1.0+2.0*xrand()*percent-percent);
	
				
			}
		}
	}
}

void	RbfSolver::calcFitnessArray()
{
	double dmin=fitness_array[0];
	for(int i=0;i<genome_count;i++)
	{
		double f=fitness(genome[i]);
		fitness_array[i]=f;
	}
}

int	RbfSolver::getGeneration() const
{
	return generation;
}

int	RbfSolver::getCount() const
{
	return genome_count;
}

int	RbfSolver::getSize() const
{
	return genome_size;
}

void	RbfSolver::nextGeneration()
{
	if(generation)
	mutate();
	calcFitnessArray();
	select();
	crossover();
	++generation;
}

void	RbfSolver::setMutationRate(double r)
{
	if(r>=0 && r<=1) mutation_rate = r;
}

void	RbfSolver::setSelectionRate(double r)
{
	if(r>=0 && r<=1) selection_rate = r;
}

double	RbfSolver::getSelectionRate() const
{
	return selection_rate;
}

double	RbfSolver::getMutationRate() const
{
	return mutation_rate;
}

double	RbfSolver::getBestFitness() const
{
	return  fitness_array[0];
}

vector<double> RbfSolver::getBestGenome() const
{
	vector<double> g;g.resize(genome_size);
	for(int i=0;i<genome_size;i++) g[i]=genome[0][i];
	return g;
}
double	RbfSolver::evaluateBestFitness() 
{
	return fitness(genome[0]);
}

void	RbfSolver::setBest(double *x,double y)
{
	for(int i=0;i<genome_size;i++)
		genome[0][i]=x[i];
	fitness_array[0]=y;
}

void	RbfSolver::local()
{
}

RbfSolver::~RbfSolver()
{
	for(int i=0;i<genome_count;i++)
	{
		delete[] children[i];
		delete[] genome[i];
	}
	delete[] genome;
	delete[] children;
	delete[] fitness_array;
}
void	RbfSolve(Problem *p,double *x,double &y,double mx,int flag)
{
	const int genome_count =500;
	const int max_generations =500;
	RbfSolver pop(genome_count,p,mx,flag);
	Rbf *nn=(Rbf *)p;
	vector<double> g;
	g.resize(pop.getSize());
	for(int i=0;i<max_generations;i++)
	{
		pop.nextGeneration();
		g=pop.getBestGenome();
		if(fabs(pop.getBestFitness())<1e-7) break;
		pop.evaluateBestFitness();
		if(i%50==0) printf("iter=%4d best=%20.8lf \n",
			i,pop.getBestFitness());
//			1.0*nn->countViolate(20.0));
	}
	for(int i=0;i<g.size();i++) x[i]=g[i];
	y=-pop.getBestFitness();
}

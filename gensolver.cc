# include <gensolver.h>
# include <stdlib.h>
# include <string.h>
# include <neural.h>
# include <iostream>
# include <math.h>
# include <stdio.h>
# include <getoptions.h>
# define MAX_RULE	256
# define CRAND


GenSolver::GenSolver(int gcount,Problem *p,double mx,int ff)
{
	small_tolmin_flag=ff;
	maxx=mx;
	problem = p;
	genome_size=p->getDimension();
	elitism=1;
	selection_rate = 0.1;
	mutation_rate  = 0.05;
	genome_count   = gcount;
	generation     = 0;

	double f;
	genome=new double*[genome_count];
	children=new double*[genome_count];
    Model *m = (Neural *)problem;

	for(int i=0;i<genome_count;i++)
	{
		genome[i]=new double[genome_size];
		children[i]=new double[genome_size];
		for(int j=0;j<genome_size;j++)
        {
            genome[i][j]= (2.0*m->randomDouble()-1.0);
        }
				
	}
	fitness_array=new double[genome_count];
}
void    GenSolver::enableWeightDecay()
{
    weight_decay_flag = true;
}

void	GenSolver::reset()
{
}


double 	GenSolver::fitness(vector<double> &g)
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
    Neural *nn=(Neural *)problem;
double    v=problem->funmin(g);
if(weight_decay_flag)
{
	double viol = nn->countViolate(20.0);
    v=v*(1.0+100.0*viol * viol);
    return -v;
}
return -v;
 //return -problem->funmin(g);
}

void	GenSolver::select()
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

void	GenSolver::crossover()
{
        int parent[2];
        int nchildren=(int)((1.0 - selection_rate) * genome_count);
	if(!(nchildren%2==0)) nchildren++;
        const int tournament_size =(genome_count<=100)?4:10;
        int count_children=0;
        Model *m = (Model *)problem;
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
                r=m->randomInt(0,genome_count-1);
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
            double a =m->randomDouble();
			children[count_children][i]=a*genome[parent[0]][i]+
				(1.0-a)*genome[parent[1]][i];
			children[count_children+1][i]=a*genome[parent[1]][i]+
				(1.0-a)*genome[parent[0]][i];
		}
		
		/*	
		int pt1,pt2;
		pt1=rand() % genome_size;
		memcpy(children[count_children],
				genome[parent[0]],pt1 * sizeof(double));
		memcpy(&children[count_children][pt1],
			&genome[parent[1]][pt1],(genome_size-pt1)*sizeof(double));
		memcpy(children[count_children+1],
				genome[parent[1]],pt1 * sizeof(double));
		memcpy(&children[count_children+1][pt1],
			&genome[parent[0]][pt1],(genome_size-pt1)*sizeof(double));
		*/	
		count_children+=2;
		if(count_children>=nchildren) break;
	}
	
	
	for(int i=0;i<nchildren;i++)
	{
		memcpy(genome[genome_count-i-1],
				children[i],genome_size * sizeof(double));
	}
}

void	GenSolver::setElitism(int s)
{
	elitism = s;
}

void	GenSolver::mutate()
{
	int start = elitism * (int)(genome_count*selection_rate);
	start = 1;
    Model *m = (Model *)problem;
	for(int i=start;i<genome_count;i++)
	{
		for(int j=0;j<genome_size;j++)
		{
            double r=m->randomDouble();
			if(r<mutation_rate)
			{
			double percent=exp(-generation * 1.0/2.0);
				percent = 0.25;
                genome[i][j]*=(1.0+2.0*m->randomDouble()*percent-percent);
	
				
			}
		}
	}
}

void	GenSolver::calcFitnessArray()
{
	vector<double> g;
	g.resize(genome_size);
	double minf=-1e+10;
	for(int i=0;i<genome_count;i++)
	{
        if(rand()*1.0/RAND_MAX<=0.01)
            localSearch(i);
		for(int j=0;j<genome_size;j++)
			g[j]=genome[i][j];	
		double f=fitness(g);
		fitness_array[i]=f;
		if(f>minf) minf=f;
		if(generation%10==0 && i%50==0) 
		{
	//		printf("%d:%lf ",i,minf);
	//		fflush(stdout);
		}
		for(int j=0;j<genome_size;j++) genome[i][j]=g[j];
	}
}

int	GenSolver::getGeneration() const
{
	return generation;
}

int	GenSolver::getCount() const
{
	return genome_count;
}

int	GenSolver::getSize() const
{
	return genome_size;
}

int     GenSolver::selectWithTournament(int tournamentSize)
{
    int min_index=-1;
    double min_value=1e+100;
    for(int i=0;i<tournamentSize;i++)
    {
        int r = rand() % genome_count;
        if(min_index==-1 || fitness_array[r]<min_value)
        {
            min_index=r;
            min_value = fitness_array[r];
        }
    }
    return min_index;
}

void    GenSolver::localSearch(int pos)
{
    vector<double> g;
    g.resize(genome_size);

        int randomA,randomB,randomC;
        int tournamentSize=8;
        do
        {
            randomA =  selectWithTournament(tournamentSize);
            randomB =  selectWithTournament(tournamentSize);
            randomC = selectWithTournament(tournamentSize);
        }while(randomA == randomB || randomB == randomC ||
               randomC == randomA);
        double CR= 0.9;
        double F = 0.8;
        int n = genome_size;
        int randomIndex = rand() % n;
    for(int i=0;i<n;i++)
    {
        if(i==randomIndex || rand()*1.0/RAND_MAX <=CR)
        {
            double old_value = genome[pos][i];
            F = -0.5 + 2.0 * rand()*1.0/RAND_MAX;
            genome[pos][i]=genome[randomA][i]+fabs(F*(genome[randomB][i]-genome[randomC][i]));


            for(int j=0;j<n;j++) g[j]=genome[pos][j];
            double trial_fitness=fitness(g);
            if(fabs(trial_fitness)<fabs(fitness_array[pos]))
            {
          //   printf("NEW DE VALUE[%d] = %lf=>%lf\n",pos,fitness_array[pos],trial_fitness);
                fitness_array[pos]=trial_fitness;
            }
            else	genome[pos][i]=old_value;
        }
    }
}

void	GenSolver::nextGeneration()
{
	if(generation)
	mutate();
	calcFitnessArray();
	select();
	crossover();
	++generation;
}

void	GenSolver::setMutationRate(double r)
{
	if(r>=0 && r<=1) mutation_rate = r;
}

void	GenSolver::setSelectionRate(double r)
{
	if(r>=0 && r<=1) selection_rate = r;
}

double	GenSolver::getSelectionRate() const
{
	return selection_rate;
}

double	GenSolver::getMutationRate() const
{
	return mutation_rate;
}

double	GenSolver::getBestFitness() const
{
	return  fitness_array[0];
}

vector<double> GenSolver::getBestGenome() const
{
	vector<double> g;g.resize(genome_size);
	for(int i=0;i<genome_size;i++) g[i]=genome[0][i];
	return g;
}
double	GenSolver::evaluateBestFitness() 
{
	vector<double> g;g.resize(genome_size);
	for(int i=0;i<genome_size;i++) g[i]=genome[0][i];	
	return fitness(g);
}

void	GenSolver::setBest(double *x,double y)
{
	for(int i=0;i<genome_size;i++)
		genome[0][i]=x[i];
	fitness_array[0]=y;
}

void	GenSolver::local()
{
	vector<double> g;
	g.resize(genome_size);
	Neural *nn=(Neural *)problem;
	nn->setWeights(g);
	for(int i=0;i<g.size();i++) g[i]=genome[0][i];
	MinInfo Info;
	Info.p=problem;
	Info.iters=21;
	double v=-tolmin(g,Info);
	/*
	Info.iters=10;
	double v=fitness_array[0];
	for(int i=1;i<=200;i++)
	{
		v=tolmin(g,Info);
		v=-v*(1.0+100.0*nn->countViolate(20.0));
		if(fabs(v)>=fabs(fitness_array[0]))
		{
			v=fitness_array[0];
			for(int j=0;j<g.size();j++) g[j]=genome[0][j];
		}
		for(int j=0;j<g.size();j++) genome[0][j]=g[j];
		fitness_array[0]=v;
	}
	*/
	nn->setWeights(g);
	for(int i=0;i<g.size();i++) genome[0][i]=g[i];
	fitness_array[0]=v;
}

GenSolver::~GenSolver()
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


void	GenSolve(Problem *p,Matrix &x,double &y,double mx,int flag)
{
    const int genome_count =genetic_chromosomes;
    const int max_generations =genetic_maxGenerations;
	GenSolver pop(genome_count,p,mx,flag);
    if(flag)
        pop.enableWeightDecay();
    pop.setSelectionRate(genetic_selectionRate);
    pop.setMutationRate(genetic_mutationRate);
	Neural *nn=(Neural *)p;
	vector<double> g;
	g.resize(x.size());
	double oldBest=-1e+100;
	double *xx=new double[x.size()];
	double x1=0,x2=0;
	double stopat=-1e+100;
	for(int i=0;i<max_generations;i++)
	{
		pop.nextGeneration();
		g=pop.getBestGenome();
		if(fabs(pop.getBestFitness())<1e-7) break;
		pop.evaluateBestFitness();
		double violate = nn->countViolate(20.0);
		x1+=fabs(pop.getBestFitness()-oldBest);
		x2+=fabs(pop.getBestFitness()-oldBest)*fabs(pop.getBestFitness()-oldBest);
		double variance=x2/(i+1)-x1/(i+1)*x1/(i+1);
		if(i>=10 && variance<=stopat) break;
   //     if(i%10==0)
   //         pop.local();
		if(pop.getBestFitness()>oldBest)
		{
			stopat=variance/2.0;
			oldBest=pop.getBestFitness();
		}
        if(i%10==0) printf("iter=%4d best=%20.8lf viol=%6.2lf\n",
            i,pop.getBestFitness(),
            1.0*nn->countViolate(20.0));
	}
//	pop.local();
	g=pop.getBestGenome();
	for(int i=0;i<g.size();i++) x[i]=g[i];
	y=-pop.getBestFitness();
	delete[] xx;	
}

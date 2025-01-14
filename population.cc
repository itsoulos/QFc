# include <population.h>	
# include <stdlib.h>
# include <string.h>
# include <math.h>
# include <iostream>
# include <problem.h>
# include <nnprogram.h>
# include "omp.h"
# define MAX_RULE	1024

Population::Population(int gcount,int gsize,vector<Program *> p)
    :QfcMethod(p)
{

    elitism=1;
    selection_rate = 0.1;
    mutation_rate  = 0.1;
    genome_count   = gcount;
    genome_size    = gsize;
    generation     = 0;
    tprogram        = p;
    maxIters = 200;
    extern int randomSeed;
    seedRandom(randomSeed);


    /* Create the population and based on genome count and size */
    /* Initialize the genomes to random */
    double f;
    genome=new int*[genome_count];
    children=new int*[genome_count];
    vector<int> g;
    g.resize(genome_size);
    for(int i=0;i<genome_count;i++)
    {
        genome[i]=new int[genome_size];
        children[i]=new int[genome_size];
    }
    fitness_array=new double[genome_count];

}


/* Population constructor */
/* Input: genome count , genome size, pointer to Program instance */
Population::Population(int gcount,int gsize,Program *p)
    :QfcMethod(p)
{
	elitism=1;
	selection_rate = 0.1;
	mutation_rate  = 0.1;
	genome_count   = gcount;
	genome_size    = gsize;
	generation     = 0;
	program        = p;
    extern int randomSeed;
    seedRandom(randomSeed);

	/* Create the population and based on genome count and size */
	/* Initialize the genomes to random */
	double f;
	genome=new int*[genome_count];
	children=new int*[genome_count];
	vector<int> g;
	g.resize(genome_size);
	for(int i=0;i<genome_count;i++)
	{
		genome[i]=new int[genome_size];
		children[i]=new int[genome_size];
	}
	fitness_array=new double[genome_count];

}

/* Reinitialize the population to random */
void	Population::init()
{
	generation = 0;
	for(int i=0;i<genome_count;i++)
		for(int j=0;j<genome_size;j++)
                genome[i][j]=getElement(j);
	for(int i=0;i<genome_count;i++)
			fitness_array[i]=-1e+100;

}

/* Return the fitness of a genome */
double 	Population::fitness(vector<int> &g)
{

	if(genome_size!=g.size())
		exit(printf("iteration: %d genome %d gsize %d \n",generation,genome_size,g.size()));

    if(isParallel())
    {
        double tf=tprogram[omp_get_thread_num()]->fitness(g);
       // printf("TF[%4d]=%10.lg\n",omp_get_thread_num(),tf);
        return tf;
    }
	return program->fitness(g);
}

/* The selection of the chromosomes according to the fitness value is performed */
void	Population::select()
{
	int itemp[genome_size];
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
				
				memcpy(itemp,genome[j],genome_size*sizeof(int));
				memcpy(genome[j],genome[j+1],genome_size*sizeof(int));
				memcpy(genome[j+1],itemp,genome_size*sizeof(int));
			}
		}
	}
}

/* Crossover operation: based on tournament selection */
/* Select the tournament_size based on the genome count : */
/*     (if genome_count > 100 ) tournament_size = 10   else   tournament_size = 4 */
/* Select 2 chromosomes based on the tournament size and cross them over based on the crossover probability */
/* There is 1 crossover point and it is random */
void	Population::crossover()
{
        int parent[2];
        int nchildren=(int)((1.0 - selection_rate) * genome_count);
	if(!(nchildren%2==0)) nchildren++;
        const int tournament_size =(genome_count<=100)?4:20;
        int count_children=0;
        while(1)
        {
		// The two parents are selected here according to the tournament selection procedure
                for(int i=0;i<2;i++)
                {
                        double max_fitness=-1e+10;
                        int    max_index=-1;
			int r;
			// Select the best parents of  the candidates 
                        for(int j=0;j<tournament_size;j++)
                        {
                r=randomInt(0,genome_count-1);
                                if(j==0 || fitness_array[r]>max_fitness)
                                {
                                        max_index=r;
                                        max_fitness=fitness_array[r];
                                }
                        }
                        parent[i]=max_index;
			
                }
		int pt1,pt2;
		// The one-point crossover is performed here (the point is pt1)
        pt1=randomInt(0,genome_size-1);
		memcpy(children[count_children],
				genome[parent[0]],pt1 * sizeof(int));
		memcpy(&children[count_children][pt1],
			&genome[parent[1]][pt1],(genome_size-pt1)*sizeof(int));
		memcpy(children[count_children+1],
				genome[parent[1]],pt1 * sizeof(int));
		memcpy(&children[count_children+1][pt1],
			&genome[parent[0]][pt1],(genome_size-pt1)*sizeof(int));
		count_children+=2;
		if(count_children>=nchildren) break;
	}
	
	vector<int> g;
	g.resize(genome_size);
	
	for(int i=0;i<nchildren;i++)
	{
		//for(int j=0;j<g.size();j++) g[j]=children[i][j];
		//double f=fitness(g);
		//if(fabs(f)<fabs(fitness_array[genome_count-i-1]))
		memcpy(genome[genome_count-i-1],
				children[i],genome_size * sizeof(int));
	}
}

void	Population::setElitism(int s)
{
	elitism = s;
}

/* Mutate the current population */
/* Standard mutation algorithm: mutate all chromosomes in the population based on the mutation probability */
void	Population::mutate()
{
	int start = elitism * (int)(genome_count*selection_rate);
	start = elitism;
	start = 1;
	for(int i=start;i<genome_count;i++)
	{
		for(int j=0;j<genome_size;j++)
		{
            double r=randomDouble();
			if(r<mutation_rate)
			{
                genome[i][j]=getElement(j);
			}
		}
	}
}

/* Evaluate the fitness for all chromosomes in the current population */
void	Population::calcFitnessArray()
{

	double dmin=1e+100;
	int icount=0;
    extern int threads;
    if(isParallel())
    {
#pragma omp parallel for num_threads(threads)
        for(int i=0;i<genome_count;i++)
        {
            vector<int> g;
            g.resize(genome_size);

            for(int j=0;j<genome_size;j++) g[j]=genome[i][j];
            fitness_array[i]=fitness(g);
	    if(rand()*1.0/RAND_MAX<=0.001)
		    localSearch(i);
            //else
            //localSearch(i);
            if(fabs(fitness_array[i])<dmin)
            {
                dmin=fabs(fitness_array[i]);
            }
            if(fabs(fitness_array[i])>=1e+100) icount++;
            
            /*if((i+1)%10==0)
            {
                printf(" %d:%.5lg ",i+1,dmin);
                fflush(stdout);
            }*/
        }
    }
    else
	for(int i=0;i<genome_count;i++)
	{
        vector<int> g;
        g.resize(genome_size);
		for(int j=0;j<genome_size;j++) g[j]=genome[i][j];	
		fitness_array[i]=fitness(g);
	    if(rand()*1.0/RAND_MAX<=0.005)
		    localSearch(i);
		//else 
		//localSearch(i);
		if(fabs(fitness_array[i])<dmin)
		{
			dmin=fabs(fitness_array[i]);
		}
		if(fabs(fitness_array[i])>=1e+100) icount++;	
		
		/*if((i+1)%10==0)
		{
			printf(" %d:%.5lg ",i+1,dmin);
			fflush(stdout);
		}*/
		
    }
	
}

/* Return the current generation */
int	Population::getGeneration() const
{
	return generation;
}

/* Return the genome count */
int	Population::getCount() const
{
	return genome_count;
}

/* Return the size of the population */
int	Population::getSize() const
{
	return genome_size;
}

void	Population::replaceWorst()
{
	vector<int> xtrial;
	xtrial.resize(genome_size);
	int randpos;
    randpos=randomInt(0,genome_count-1);
	for(int i=0;i<genome_size;i++)
	{
		double gamma;
        gamma=-0.5+2.0*randomDouble();
		xtrial[i]=(int)(fabs((1.0+gamma)*genome[0][i]-gamma*genome[randpos][i]));
	}
	double ftrial = fitness(xtrial);
	if(ftrial>fitness_array[genome_count-1])
	{
		for(int i=0;i<genome_size;i++)
		genome[genome_count-1][i]=xtrial[i];
		fitness_array[genome_count-1]=ftrial;
	}
}

/* Set the mutation rate */
void	Population::setMutationRate(double r)
{
	if(r>=0 && r<=1) mutation_rate = r;
}

/* Set the selection rate */
void	Population::setSelectionRate(double r)
{
	if(r>=0 && r<=1) selection_rate = r;
}

/* Return the selection rate */
double	Population::getSelectionRate() const
{
	return selection_rate;
}

/* Return the mutation rate */
double	Population::getMutationRate() const
{
	return mutation_rate;
}

/* Return the best fitness for this population */
double	Population::getBestFitness() const
{
	return  fitness_array[0];
}

/* Return the best chromosome */
vector<int> Population::getBestGenome() const
{
	vector<int> g;g.resize(genome_size);
	for(int i=0;i<genome_size;i++) g[i]=genome[0][i];
	return g;
}


class PopulationProblem: public Problem
{
	private:
		Population *pop;
		vector<int> currentGenome;
	public:	
	PopulationProblem(Population *p,vector<int> &x)
	{
		pop = p;
		currentGenome = x;
		setDimension(x.size());
		lmargin.resize(x.size());
		rmargin.resize(x.size());
		for(int i=0;i<x.size();i++)
		{
			lmargin[i]=0;
			rmargin[i]=MAX_RULE;
		}
	}

 double dmax(double a,double b){return a>b?a:b;}
	virtual double funmin(Matrix x)
	{
		for(int i=0;i<x.size();i++) {
			currentGenome[i]=(int)x[i];
			if(currentGenome[i]<0) currentGenome[i]=0;
		}
		double f= -pop->fitness(currentGenome);
		return f;
		
	}
	virtual void	granal(Matrix x,Matrix &g)
	{
		for(int i=0;i<x.size();i++)
		{
        double eps=0.1;//pow(1e-18,1.0/3.0)*dmax(1.0,fabs(x[i]));
		x[i]+=eps;
		double v1=funmin(x);
		x[i]-=2.0 *eps;
		double v2=funmin(x);
		g[i]=(v1-v2)/(2.0 * eps);
		x[i]+=eps;
		}
	}

};

void    Population::setMaxIters(int t)
{
    maxIters = t;
}

int     Population::getMaxIters() const
{
    return maxIters;
}

void Population::step()
{
    calcFitnessArray();
    
    /*
    const int mod=20;//localSearchGenerations;
    if((generation+1) % mod==0)
    {

       const int K=20;
       for(int i=0;i<K;i++)
           localSearch(rand() % genome_count);
       localSearch(0);
    }*/
    select();
    crossover();
    if(generation) mutate();
    ++generation;
}

void Population::report()
{
    vector<int> g;
    g.resize(genome_size);
    g = getBestGenome();
    string s = "";
    if(!isParallel())
        s=((NNprogram *)program)->printF(g);
    else
        s=((NNprogram *)tprogram[0])->printF(g);

    qDebug().noquote()<<"Iteration: "<<generation<<" Best Fitness: "<<
                        getBestFitness()<<
                        " Best program:\n"<<s.c_str();
}

bool Population::terminated()
{
    return generation>=maxIters;
}


class IntegerAnneal
{
private:
    Population *pop;
    vector<int> xpoint;
    double ypoint;
    vector<int> bestx;
    double besty;
    int neps=100;
    double T0;
    int k;
public:
    IntegerAnneal(Population *p);
    void    setNeps(int n);
    void    setT0(double t);
    void setPoint(vector<int> &g,double &y);
    void getPoint(vector<int> &g,double &y);
    void Solve();
    void    updateTemp();
    ~IntegerAnneal();
};
IntegerAnneal::IntegerAnneal(Population *p)
{
    pop = p;
    T0=1e+8;
    neps=200;
}
void    IntegerAnneal::setNeps(int n)
{
    neps  = n;
}

void    IntegerAnneal::updateTemp()
{
    const double alpha = 0.8;

    T0 =T0 * pow(alpha,k);
    k=k+1;
}
void    IntegerAnneal::setT0(double t)
{
    T0  = t;
}
void    IntegerAnneal::setPoint(vector<int> &g,double &y)
{
    xpoint = g;
    ypoint = y;

}
void    IntegerAnneal::getPoint(vector<int> &g,double &y)
{
    g = bestx;
    y = besty;
}

void    IntegerAnneal::Solve()
{
    bestx = xpoint;
    besty = ypoint;
    int i;
    k=1;
    vector<int> y;
    y.resize(bestx.size());
    while(true)
    {
        for(i=1;i<=neps;i++)
        {
        double fy;
        for(int j=0;j<y.size();j++)
            y[j]=rand() % MAX_RULE;
        fy = pop->fitness(y);
        if(isnan(fy) || isinf(fy)) continue;

        if(fy>ypoint)
        {
            xpoint = y;
            ypoint = fy;
            if(ypoint>besty)
            {
                                        bestx = xpoint;
                                        besty = ypoint;
            }
        }
        else
        {
            double r = fabs((rand()*1.0)/RAND_MAX);
            double ratio = exp(-(fy-ypoint)/T0);
            double xmin = ratio<1?ratio:1;
            if(r<xmin)
            {
                                        xpoint = y;
                                        ypoint = fy;
                                        if(ypoint>besty)
                                        {
                                            bestx = xpoint;
                                            besty = ypoint;
                                        }
            }
        }
        }
        updateTemp();
        if(T0<=1e-5) break;
       // printf("Iteration: %4d Temperature: %20.10lg Value: %20.10lg\n",
       //        k,T0,besty);

    }
}

IntegerAnneal::~IntegerAnneal()
{
    //nothing here
}

void	Population::localSearch(int pos)
{
    if(localSearchMethod=="none") return;
	vector<int> g;
	g.resize(genome_size);

	for(int i=0;i<genome_size;i++) g[i]=genome[pos][i];
    if(localSearchMethod == "siman")
    {
        IntegerAnneal mt(this);
        double t= fitness_array[pos];
        mt.setPoint(g,t);
        mt.Solve();
        double y;
        mt.getPoint(g,y);
        fitness_array[pos]=y;
        fprintf(stderr,"LOCAL SEARCH[%20.10lg]=>[%20.10lg]\n",t,y);
        for(int i=0;i<genome_size;i++)
        {
        genome[pos][i]=g[i];
        if(genome[pos][i]<0) genome[pos][i]=0;
        }
        // delete pr;
        return;
    }
    else
    if(localSearchMethod=="crossover")
    {

        for(int iters=1;iters<=100;iters++)
        {
            int gpos=randomInt(0,genome_count-1);
            int cutpoint=randomInt(0,genome_size-1);
            for(int j=0;j<cutpoint;j++) g[j]=genome[pos][j];
            for(int j=cutpoint;j<genome_size;j++) g[j]=genome[gpos][j];
            double f=fitness(g);
            if(fabs(f)<fabs(fitness_array[pos]))
            {
                printf("%4d: LOCAL[%lf]=>%lf\n",pos,fitness_array[pos],f);

                for(int j=0;j<genome_size;j++) genome[pos][j]=g[j];
                fitness_array[pos]=f;
            }
            else
            {
                for(int j=0;j<cutpoint;j++) g[j]=genome[gpos][j];
                for(int j=cutpoint;j<genome_size;j++) g[j]=genome[pos][j];
                double f=fitness(g);
                if(fabs(f)<fabs(fitness_array[pos]))
                {
                //    printf("%4d: LOCAL[%lf]=>%lf\n",pos,fitness_array[pos],f);

                    for(int j=0;j<genome_size;j++) genome[pos][j]=g[j];
                    fitness_array[pos]=f;
                }
            }
        }

    }
    else
    if(localSearchMethod=="random")
    {
        int randomA,randomB,randomC;
        do
        {
            randomA =  rand() % genome_count;
            randomB =  rand() % genome_count;
            randomC =  rand() % genome_count;
        }while(randomA == randomB || randomB == randomC || randomC == randomA);
        double CR= 0.9;
        double F = 0.8;
        int randomIndex = rand() % genome_size;
    for(int i=0;i<genome_size;i++)
    {
        if(i==randomIndex || rand()*1.0/RAND_MAX <=CR)
        {
            int old_value = genome[pos][i];
            F = -0.5 + 2.0 * rand()*1.0/RAND_MAX;
            genome[pos][i]=genome[randomA][i]+abs(F*(genome[randomB][i]-genome[randomC][i]));
            if(genome[pos][i]<0)
            {
             genome[pos][i]=old_value;
             continue;
            }

            for(int j=0;j<genome_size;j++) g[j]=genome[pos][j];
            double trial_fitness=fitness(g);
            if(fabs(trial_fitness)<fabs(fitness_array[pos]))
            {
             //printf("NEW DE VALUE[%d] = %lf=>%lf\n",pos,fitness_array[pos],trial_fitness);
                fitness_array[pos]=trial_fitness;
            }
            else	genome[pos][i]=old_value;
        }
    }

       /* for(int i=0;i<genome_size;i++)
        {
            int ipos =randomInt(0,genome_size-1);
            int new_value;
            for(int k=0;k<20;k++)
            {
            int old_value = genome[pos][ipos];

		int range = 20;
		int direction = rand()%2==1?1:-1;
		int new_value = old_value+direction*(rand()%range);
		if(new_value<0) new_value=0;
            genome[pos][ipos]=new_value;
            for(int j=0;j<genome_size;j++) g[j]=genome[pos][j];
            double trial_fitness=fitness(g);
            if(fabs(trial_fitness)<fabs(fitness_array[pos]))
            {
                //fprintf(stderr,"%4d: LOCAL[%lf]=>%lf\n",pos,fitness_array[pos],trial_fitness);

                fitness_array[pos]=trial_fitness;

            }
            else	genome[pos][ipos]=old_value;
            }
        }*/
    }
    else
    if(localSearchMethod=="bfgs")
    {
        PopulationProblem *pr=new PopulationProblem(this,g);
            double y;
            Matrix x;
            x.resize(genome_size);
            y=fitness_array[pos];
            for(int i=0;i<genome_size;i++) x[i]=g[i];


            MinInfo Info;
            Info.p = pr;
            Info.iters=2001;
            y=tolmin(x,Info);

                fprintf(stderr,"%4d: BFGS[%lf]=>%lf\n",pos,fitness_array[pos],-y);
            fitness_array[pos]=-y;
            for(int i=0;i<genome_size;i++)
            {
                genome[pos][i]=(int)x[i];
                if(genome[pos][i]<0) genome[pos][i]=0;
            }
            delete pr;
    }
}

/* Evaluate and return the best fitness for all chromosomes in the population */
double	Population::evaluateBestFitness() 
{
	vector<int> g;g.resize(genome_size);
	for(int i=0;i<genome_size;i++) g[i]=genome[0][i];	
	return fitness(g);
}


void    Population::setLocalSearchRate(double r)
{
    localSearchRate=r;
}

double  Population::getLocalSearchRate() const
{
    return localSearchRate;
}

void    Population::setLocalSearchGenerations(int g)
{
    localSearchGenerations=g;
}

int     Population::getLocalSearchGenerations() const
{
    return localSearchGenerations;
}

void    Population::setLocalSearchMethod(QString s)
{
    localSearchMethod=s;
}

QString  Population::getLocalSearchMethod() const
{
    return localSearchMethod;
}


void	Population::setBest(vector<int> &g,double f)
{
    double tf=fitness(g);
    if(tf<fitness_array[0] && fabs(tf-f)>1e-4)
    {
        return;
    }
    if(g.size()>genome_size)
    {
        int *old=new int[genome_size];
        for(int i=0;i<genome_count;i++)
        {
            for(int j=0;j<genome_size;j++) old[j]=genome[i][j];
            delete[] genome[i];
            genome[i]=new int[g.size()];
            for(int j=0;j<g.size();j++) 	genome[i][j]=0;
            for(int j=0;j<genome_size;j++) genome[i][j]=old[j];
            delete[] children[i];
            children[i]=new int[g.size()];
        }
        genome_size=g.size();
        extern int ge_length;
        ge_length=genome_size;
        delete[] old;
    }
    int pos=0;
    for(int i=0;i<genome_size;i++)
    {
        genome[pos][i]=g[i];
    }
    fitness_array[pos]=f;
}

void	Population::setGenome(int pos,vector<int> &g,double f,int k)
{
    double tf=fitness(g);
    if(tf<fitness_array[pos] && fabs(tf-f)>1e-4) return;
    if(g.size()>genome_size)
    {
        int *old=new int[genome_size];
        for(int i=0;i<genome_count;i++)
        {
            for(int j=0;j<genome_size;j++) old[j]=genome[i][j];
            delete[] genome[i];
            genome[i]=new int[g.size()];
            for(int j=0;j<g.size();j++) genome[i][j]=0;
            int ipos=0;
            for(int l=0;l<k;l++)
            {
                for(int j=0;j<genome_size/k;j++)
                {
                    genome[i][l*g.size()/k+j]=old[ipos];
                    ipos++;
                }
            }
            delete[] children[i];
            children[i]=new int[g.size()];
        }
        genome_size=g.size();
        extern int ge_length;
        ge_length=genome_size;
        delete[] old;
    }
    for(int i=0;i<genome_size;i++)
    {
        genome[pos][i]=g[i];
    }
    fitness_array[pos]=f;
}

void	Population::setGenome(int pos,vector<int> &g,double f)
{
    double tf=fitness(g);
    if(tf<fitness_array[pos] && fabs(tf-f)>1e-4)
        return;
    if(g.size()>genome_size)
    {
        int *old=new int[genome_size];
        for(int i=0;i<genome_count;i++)
        {
            for(int j=0;j<genome_size;j++) old[j]=genome[i][j];
            delete[] genome[i];
            genome[i]=new int[g.size()];
            for(int j=0;j<g.size();j++) genome[i][j]=0;
            for(int j=0;j<genome_size;j++) genome[i][j]=old[j];
            delete[] children[i];
            children[i]=new int[g.size()];
        }
        genome_size=g.size();
        extern int ge_length;
        ge_length=genome_size;
        delete[] old;
    }
    for(int i=0;i<genome_size;i++)
    {
        genome[pos][i]=g[i];
    }
    fitness_array[pos]=f;
}

void	Population::getGenome(int pos,vector<int> &g)
{
    g.resize(genome_size);
    for(int i=0;i<g.size();i++) g[i]=genome[pos][i];
}
/* Destructor */
Population::~Population()
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

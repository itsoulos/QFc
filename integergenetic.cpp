#include "integergenetic.h"
# include <population.h>
# include <omp.h>
# include <iostream>
using namespace std;
IntegerGenetic::IntegerGenetic(int gcount,int gsize,Program *p)
    :QfcMethod(p)
{
    genome_count = gcount ;
    genome_size = gsize;
    makeGenetic();

}

IntegerGenetic::IntegerGenetic(int gcount,int gsize,vector<Program *> p)
    :QfcMethod(p)
{
    genome_count = gcount ;
    genome_size =gsize;
    makeGenetic();
}

void    IntegerGenetic::makeGenetic()
{
    nsamples=50;
    maxGenerations = 200;
    maxRule = 1024;
    genome.resize(genome_count);
    children.resize(genome_count);
    setSelectionRate(0.1);
    setMutationRate(0.05);
    fitnessArray.resize(genome_count);

    for(int i=0;i<genome_count;i++)
    {
        genome[i].resize(2*maxDepth * genome_size);

        children[i].resize(genome[i].size());
        fitnessArray[i]=fitness(genome[i]);
    }
}

/* Return the fitness of a genome */
double 	IntegerGenetic::programFitness(vector<int> &g)
{

    if(isParallel())
    {
        double tf=tprogram[omp_get_thread_num()]->fitness(g);
       // printf("TF[%4d]=%10.lg\n",omp_get_thread_num(),tf);
        return -tf;
    }
    return -program->fitness(g);
}

DoubleInterval  IntegerGenetic::fitness(IDATA &x)
{
    IntervalData xx=produceInterval(x);
    vector<int> trialx;
    trialx.resize(xx.size());

    double miny=1e+100,maxy=1e+100;
    QRandomGenerator g;
    g.seed(1);
    for(int k=1;k<=nsamples;k++)
    {
        for(int i=0;i<(int)xx.size();i++)
        {
            trialx[i]=g.bounded(xx[i].leftValue(),xx[i].rightValue());// randomInt(xx[i].leftValue(),xx[i].rightValue());
        }
        double fx= programFitness(trialx);
        if(k==1 || fx>maxy) maxy=fx;
        if(k==1 || fx<miny) miny=fx;
    }
    return DoubleInterval(miny,maxy);
}

IntervalData    IntegerGenetic::produceInterval(IDATA &x)
{
    int icount=0;
    IntervalData tempx1;
    IntervalData m;
    tempx1.resize(genome_size);
    m.resize(genome_size);
    for(int i=0;i<genome_size;i++)
    {
        tempx1[i]=Interval(0,maxRule);
        m[i]=Interval(0,maxRule);
    }
    for(int j=0;j<maxDepth;j++)
    {
    for(int i=0;i<genome_size;i++)
    {
         int l,r;
            l=x[icount++];
            r=x[icount++];
            double d1=tempx1[i].leftValue();
            double d2=tempx1[i].rightValue();
            if(l==1) d1=d1/2;
            if(d1<m[i].leftValue()) d1=tempx1[i].leftValue();
            if(r==1)
            {
                d2=d2/2;
                if(d2<m[i].leftValue())
                    d2=tempx1[i].rightValue();
            }
            if(d2<d1)
            {
                double t=d1;
                d1=d2;
                d2=t;
            }
            tempx1[i]=Interval(d1,d2);
        }
    }
    return tempx1;
}

void    IntegerGenetic::setMaxGenerations(int g)
{
    maxGenerations = g;
}

int     IntegerGenetic::getMaxGenerations() const
{
    return maxGenerations;
}

void    IntegerGenetic::mutate()
{
    for(int i=1;i<genome_count;i++)
    {
       int size=genome[0].size();
       for(int j=0;j<size;j++)
       {
           double r=randomDouble();
           if(r<=mutation_rate)
           {
               int k=genome[i][j];
               if(k==1) genome[i][j]=0; else genome[i][j]=1;
           }
       }
    }
}


bool   lowerValue(DoubleInterval &a,DoubleInterval &b)
{

     return a.leftValue()<b.leftValue()
       || (a.leftValue()<=b.leftValue() &&
           fabs(a.leftValue()-b.leftValue())<1e-6
           && a.width()<b.width());
}

void    IntegerGenetic::selection()
{
    IDATA temp;
    DoubleInterval itemp;
    for(int i=0;i<genome_count;i++)
    {
        for(int j=0;j<genome_count-1;j++)
        {
            if(lowerValue(fitnessArray[j+1],fitnessArray[j]))
            {
                temp=genome[j];
                genome[j]=genome[j+1];
                genome[j+1]=temp;
                itemp=fitnessArray[j];
                fitnessArray[j]=fitnessArray[j+1];
                fitnessArray[j+1]=itemp;
            }
        }
    }
}

void    IntegerGenetic::crossover()
{

    int children_size=(int)((1.0-selection_rate)*genome_count);
    int children_count=0;
    while(true)
    {
        int parent1=tournament();
        int parent2=tournament();
        unsigned size=genome[0].size();
     /*for(int i=0;i<size;i++)
         {
             int r=rand()%2;
             if(r==0)
             {
                 children[children_count][i]=genome[parent2][i];
                 children[children_count+1][i]=genome[parent1][i];
             }
             else
             {
                 children[children_count][i]=genome[parent1][i];
                 children[children_count+1][i]=genome[parent2][i];
             }

         }
     */

        //ONE POINT CROSSOVER
      int cutPoint=rand() % size;
        for(int i=0;i<cutPoint;i++)
        {
            children[children_count][i]=genome[parent2][i];
            children[children_count+1][i]=genome[parent1][i];
        }
        for(int i=cutPoint;i<size;i++)
        {
            children[children_count][i]=genome[parent1][i];
            children[children_count+1][i]=genome[parent2][i];
        }

         children_count+=2;
         if(children_count>=children_size) break;
     }
     for(int i=0;i<children_count;i++)
     {
         genome[genome.size()-i-1]=children[i];
     }
}

int     IntegerGenetic::tournament()
{
    const int NMAX=8;
    int maxPos=-1;
    DoubleInterval maxValue=DoubleInterval(1e+10,1e+10);
    for(int i=0;i<NMAX;i++)
    {
        int randPos=randomInt(0,genome_count-1);
        if(i==0 || lowerValue(fitnessArray[randPos],maxValue))
        {
            maxPos=randPos;
            maxValue=fitnessArray[randPos];
        }
    }
    return maxPos;
}

void    IntegerGenetic::calcFitnessArray()
{
    extern int threads;
    if(isParallel())
    {
#pragma omp parallel for num_threads(threads)
    for(int i=0;i<genome_count;i++)
    {
        fitnessArray[i]=fitness(genome[i]);
    }
    }
    else
    {
        for(int i=0;i<genome_count;i++)
        {
            fitnessArray[i]=fitness(genome[i]);
        }
    }
}

void    IntegerGenetic::init()
{
    generation = 0;
    for(int i=0;i<genome_count;i++)
     for(int j=0;j<(int)genome[i].size();j++)
         genome[i][j]=randomInt(0,1);
}

void    IntegerGenetic::step()
{
    if(generation) mutate();
    calcFitnessArray();
    selection();
    crossover();
    if(generation%20==0)
    {
	    int count = 20;
#pragma omp parallel for schedule(dynamic)
	    for(int i=0;i<count;i++)
		    localSearch(rand() % genome.size());
	    selection();
    }
    ++generation;
}

void    IntegerGenetic::report()
{
    selection();
    cout<<"Generation: "<<generation<<" best value "<<fitnessArray[0]<<endl;
}

bool    IntegerGenetic::terminated()
{
    return generation>=maxGenerations;
}

void    IntegerGenetic::done()
{
    //nothing yet
    //run genetic here using the best interval
    IntervalData bestI = produceInterval(genome[0]);
    Population *pop;
    if(!isParallel())
    pop = new Population(genome_count,genome_size,program);
    else
    {
     pop = new Population(genome_count,genome_size,tprogram);
    }
    pop->setBounds(bestI);
    extern int  randomSeed;
    pop->seedRandom(randomSeed);
    pop->setLocalSearchGenerations(20000);
    pop->setMaxIters(200);
    pop->run();

    bestGenome = pop->getBestGenome();

    delete pop;
}

vector<int> IntegerGenetic::getBestGenome() const
{
    return bestGenome;
}

void    IntegerGenetic::setSelectionRate(double r)
{
    selection_rate = r;
}

double  IntegerGenetic::getSelectionRate() const
{
    return selection_rate;
}

void    IntegerGenetic::setMutationRate(double r)
{
    mutation_rate  = r;
}

double  IntegerGenetic::getMutationRate() const
{
    return mutation_rate;
}

int     IntegerGenetic::getChromosomeCount() const
{
    return genome_count;
}

int     IntegerGenetic::getGenerationCount() const
{
    return generation;
}

IntegerGenetic::~IntegerGenetic()
{
    //nothing here
}
void	IntegerGenetic::localSearch(int pos)
{
	int genome_size  = genome[0].size();
	vector<int> g;
	g.resize(genome_size);
	for(int i=0;i<genome_size;i++) g[i]=genome[pos][i];
	int genome_count = genome.size();
	
	for(int iters=1;iters<=100;iters++)
	{
		int gpos=rand() % genome_count;
		int cutpoint=rand() % genome_size;
		for(int j=0;j<cutpoint;j++) g[j]=genome[pos][j];
		for(int j=cutpoint;j<genome_size;j++) g[j]=genome[gpos][j];
                DoubleInterval fx=fitness(g);
                if(lowerValue(fx,fitnessArray[pos]))
		{
			printf("NEW MIN[%4d]=[%10.4lg,%10.4lg]\n",pos,fx.leftValue(),fx.rightValue());
			for(int j=0;j<genome_size;j++) genome[pos][j]=g[j];
			fitnessArray[pos]=fx;
		}
		else
		{
			for(int j=0;j<cutpoint;j++) g[j]=genome[gpos][j];
			for(int j=cutpoint;j<genome_size;j++) g[j]=genome[pos][j];
                	fx=fitness(g);
                	if(lowerValue(fx,fitnessArray[pos]))
			{
			//printf("NEW MIN[%4d]=[%10.4lg,%10.4lg]\n",pos,fx.leftValue(),fx.rightValue());
				for(int j=0;j<genome_size;j++) genome[pos][j]=g[j];
				fitnessArray[pos]=fx;
			}
		}
	}
}
		

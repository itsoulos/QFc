#include "psoge.h"
# include <nnprogram.h>
# define NMAX 255
PsoGE::PsoGE(int gcount,int gsize,Program *p)
    :QfcMethod(p)
{
    pso_count = gcount;
    particle_size = gsize;
    pso_iters = 200;
    makePsoPopulation();
}

void    PsoGE::makePsoPopulation()
{
    extern int randomSeed;
    seedRandom(randomSeed);
    particle=new int*[pso_count];
    velocity=new int*[pso_count];
    bestParticle=new int*[pso_count];
    fitnessArray = new double[pso_count];
    bestFitnessArray= new double[pso_count];
    bestg = new int[particle_size];

    vector<int> gt;
    gt.resize(particle_size);

    for(int i=0;i<pso_count;i++)
    {
        particle[i]=new int[particle_size];

        bestParticle[i]=new int[particle_size];
        velocity[i] = new int[particle_size];
    }
}

void    PsoGE::initParticle(int *p)
{
    for(int i=0;i<particle_size;i++)
    {
        p[i] = getElement(i);
        if(p[i]<0) p[i]=-p[i];
    }
}

void    PsoGE::initVelocity(int *v)
{
    int left = -5;
    int right = 5;
    for(int i=0;i<particle_size;i++)
    {
        v[i]=randomInt(left,right);
    }
}

PsoGE::PsoGE(int gcount,int gsize,vector<Program *> p)
    :QfcMethod(p)
{
    pso_count = gcount;
    particle_size = gsize;
    pso_iters = 200;
    makePsoPopulation();
}

void    PsoGE::init()
{
    vector<int> gt;
    gt.resize(particle_size);
    bestf=1e+100;
    for(int i=0;i<pso_count;i++)
    {
        initParticle(particle[i]);
        memcpy(bestParticle[i],particle[i],particle_size*sizeof(int));
        initVelocity(velocity[i]);
    }
    calcFitnessArray();
    for(int i=0;i<pso_count;i++)
    {
        bestFitnessArray[i]=fitnessArray[i];
        if(i==0 || fitnessArray[i]<bestf)
        {
            bestf = fitnessArray[i];
            memcpy(bestg,particle[i],sizeof(int)*particle_size);
        }
    }
    generation = 0;
}

void    PsoGE::step()
{
    ++generation;
    calcVelocity();
    calcFitnessArray();

}

bool    PsoGE::terminated()
{
    return generation>=pso_iters;
}

double  PsoGE::fitness(vector<int> &g)
{
    extern int threads;

    if(threads>1 && tprogram.size()!=0)
    {
        double tf=tprogram[omp_get_thread_num()]->fitness(g);
    //   printf("TF[%4d]=%10.lg\n",omp_get_thread_num(),tf);
        return -tf;
    }
    return -program->fitness(g);
}

void    PsoGE::calcVelocity()
{
    double inertia = 0.9 - 0.05 * generation/pso_iters;
    inertia = 0.5 + (randomDouble())/2.0;
    double c1 = 1.0;
    double c2 = 1.0;
    for(int i=0;i<pso_count;i++)
    {
        for(int j=0;j<particle_size;j++)
        {
            velocity[i][j] =(int)( inertia * velocity[i][j]
                    +c1 * randomDouble()*(bestg[j]-particle[i][j])+
                    c2 * randomDouble() *(bestParticle[i][j]-particle[i][j]));
            particle[i][j]=particle[i][j]+velocity[i][j];
            if(particle[i][j]<0) particle[i][j]=0;
            if(particle[i][j]>NMAX) particle[i][j]=NMAX;
        }
    }
}

vector<int> PsoGE::getBestParticle()
{
    vector<int> g;
    g.resize(particle_size);
    for(int i=0;i<particle_size;i++)
    {
        g[i]=bestg[i];
        if(g[i]<0) exit(printf("ERROR %d\n",g[i]));
    }
    return g;
}

void    PsoGE::report()
{
    vector<int> genome = getBestParticle();
    string s = "";
    if(!isParallel())
        s=((NNprogram *)program)->printF(genome);
    else
        s=((NNprogram *)tprogram[0])->printF(genome);

    qDebug().noquote()<<"Iteration: "<<generation<<" Best Fitness: "<<
                        getBestFitness()<<
                        " Best program:\n"<<s.c_str();
}
double      PsoGE::getBestFitness()
{
    return bestf;
}
void    PsoGE::setMaxIters(int g)
{
    pso_iters = g;
}
void    PsoGE::calcFitnessArray()
{

    extern int threads;
    if(isParallel())
    {
    #pragma omp parallel for num_threads(threads)
    for(int i=0;i<pso_count;i++)
    {
        vector<int> g;
        g.resize(particle_size);
        for(int j=0;j<particle_size;j++)
        {

            g[j]=particle[i][j];
        }
        fitnessArray[i]=fitness(g);
        if(fitnessArray[i]<bestFitnessArray[i]
                || generation<=1)
        {
            bestFitnessArray[i]=fitnessArray[i];
            memcpy(bestParticle[i],particle[i],
                   sizeof(int)*particle_size);

        }
#pragma omp critical
        if(fitnessArray[i]<bestf)
        {
            bestf = fitnessArray[i];
            memcpy(bestg,particle[i],sizeof(int)*particle_size);
        }

    }
    }
    else
    {
        vector<int> g;
        g.resize(particle_size);
        for(int i=0;i<pso_count;i++)
        {
            for(int j=0;j<particle_size;j++)
            {

                g[j]=particle[i][j];
            }
            fitnessArray[i]=fitness(g);
            if(fitnessArray[i]<bestFitnessArray[i]
                    || generation<=1)
            {
                bestFitnessArray[i]=fitnessArray[i];
                memcpy(bestParticle[i],particle[i],
                       sizeof(int)*particle_size);

            }
            if(fitnessArray[i]<bestf)
            {
                bestf = fitnessArray[i];
                memcpy(bestg,particle[i],sizeof(int)*particle_size);
            }
        }
    }
}


PsoGE::~PsoGE()
{
    for(int i=0;i<pso_count;i++)
    {
        delete[] particle[i];
        delete[] bestParticle[i];
        delete[] velocity[i];
    }
    delete[] bestg;
    delete[] particle;
    delete[] bestParticle;
    delete[] fitnessArray;
    delete[] bestFitnessArray;

}

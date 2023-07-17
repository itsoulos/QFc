#ifndef PSOGE_H
#define PSOGE_H
# include <program.h>
# include <QString>
# include "omp.h"
# include <qfcmethod.h>
class PsoGE :public QfcMethod
{
private:
    int **particle;
    int **bestParticle;
    int **velocity;
    double *fitnessArray;
    double *bestFitnessArray;
    int  *bestg;
    double bestf;

    int pso_count;
    int pso_iters;
    int particle_size;
    int generation;

    void    initParticle(int *p);
    void    initVelocity(int *v);
    double  fitness(vector<int> &g);
    void    calcVelocity();
    void    calcFitnessArray();
    void    makePsoPopulation();
public:
    PsoGE(int gcount,int gsize,Program *p);
    PsoGE(int gcount,int gsize,vector<Program *> p);
    void        setMaxIters(int g);
    vector<int> getBestParticle();
    double      getBestFitness();

    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void report();

    ~PsoGE();
};

#endif // PSOGE_H

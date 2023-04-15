#ifndef PSOGE_H
#define PSOGE_H
# include <program.h>
# include <QString>
# include "omp.h"

class PsoGE
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

    Program	*program;
    vector<Program*> tprogram;
    void    initParticle(int *p);
    void    initVelocity(int *v);
    double  fitness(vector<int> &g);
    void    calcVelocity();
    void    calcFitnessArray();
public:
    PsoGE(int gcount,int gsize,Program *p);
    PsoGE(int gcount,int gsize,vector<Program *> p);
    void    nextGeneration();
    void    setMaxIters(int g);
    vector<int> getBestParticle();
    double      getBestFitness();
    void Solve(int g=200);
    ~PsoGE();
};

#endif // PSOGE_H

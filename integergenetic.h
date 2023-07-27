#ifndef INTEGERGENETIC_H
#define INTEGERGENETIC_H
# include <qfcmethod.h>
# include <nnprogram.h>
# include <interval.h>
# include <doubleinterval.h>
# include <QVariant>
typedef vector<int> IDATA;
typedef vector<Interval> IntervalData;

class IntegerGenetic : public QfcMethod
{
private:
    int genome_count;
    int genome_size;
    int nsamples;
    int generation;
    int maxGenerations;
    double selection_rate,mutation_rate;

    vector<IDATA> genome;
    vector<IDATA> children;
    const int maxDepth=20;

    vector<DoubleInterval> fitnessArray;

    double 	programFitness(vector<int> &g);
    DoubleInterval fitness(IDATA &x);
    IntervalData produceInterval(IDATA &x);

    vector<int> bestGenome;
    //private methods
    void    makeGenetic();
    void mutate();
    void selection();
    void crossover();
    int tournament();
    void calcFitnessArray();
public:
    IntegerGenetic(int gcount,int gsize,Program *p);
    IntegerGenetic(int gcount,int gsize,vector<Program *> p);
    virtual void init();
    virtual void step();
    virtual void report();
    virtual bool terminated();
    virtual void done();
    void    setSelectionRate(double r);
    double  getSelectionRate() const;
    void    setMutationRate(double r);
    double  getMutationRate() const;
    int     getChromosomeCount() const;
    int     getGenerationCount() const;
    void    setMaxGenerations(int g);
    int     getMaxGenerations() const;
    vector<int> getBestGenome() const;
    void	localSearch(int pos);
    ~IntegerGenetic();
};

#endif // INTEGERGENETIC_H

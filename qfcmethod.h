#ifndef QFCMETHOD_H
#define QFCMETHOD_H
# include <program.h>
# include <QString>
# include <QDebug>
# include <QRandomGenerator>
# include "omp.h"

class QfcMethod
{
protected:
    Program	*program;
    vector<Program*> tprogram;
    vector<QRandomGenerator> random;
public:
    QfcMethod(Program *t);
    QfcMethod(vector<Program*> t);
    void    seedRandom(int s);
    double  randomDouble();
    int     randomInt(int low,int upper);
    virtual void init()=0;
    virtual void step()=0;
    virtual void report()=0;
    virtual bool terminated()=0;
    virtual void done();
    virtual void run();
    bool isParallel();

    ~QfcMethod();
};

#endif // QFCMETHOD_H

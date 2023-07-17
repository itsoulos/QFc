#ifndef QFCMETHOD_H
#define QFCMETHOD_H
# include <program.h>
# include <QString>
# include <QDebug>
# include "omp.h"

class QfcMethod
{
protected:
    Program	*program;
    vector<Program*> tprogram;
public:
    QfcMethod(Program *t);
    QfcMethod(vector<Program*> t);
    virtual void init()=0;
    virtual void step()=0;
    virtual void report()=0;
    virtual bool terminated()=0;
    virtual void run();
    bool isParallel();

    ~QfcMethod();
};

#endif // QFCMETHOD_H

#include "qfcmethod.h"

QfcMethod::QfcMethod(Program *t)
{
    program = t;
    tprogram.resize(0);
    random.resize(1);
}

QfcMethod::QfcMethod(vector<Program*> t)
{
    tprogram = t;
    program = NULL;
    random.resize(t.size());
}


void    QfcMethod::seedRandom(int s)
{
    for(int i=0;i<random.size();i++)
        random[i].seed(s);
}

double  QfcMethod::randomDouble()
{
    if(isParallel())
        return random[omp_get_thread_num()].generateDouble();
    else
        return random[0].generateDouble();
}

int     QfcMethod::randomInt(int low,int upper)
{
    if(isParallel())
        return random[omp_get_thread_num()].bounded(low,upper);
    else
        return random[0].bounded(low,upper);
}

void    QfcMethod::done()
{
    //nothing here
}

void    QfcMethod::run()
{
    init();
    while(!terminated())
    {
        step();
        report();
    }
    done();
}

bool QfcMethod::isParallel()
{
    return tprogram.size()!=0;
}

QfcMethod::~QfcMethod()
{

}

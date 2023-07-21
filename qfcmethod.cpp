#include "qfcmethod.h"

QfcMethod::QfcMethod(Program *t)
{
    program = t;
    tprogram.resize(0);
    random.resize(1);
    maxRule = 255;
    haveBounds  = false;
}

QfcMethod::QfcMethod(vector<Program*> t)
{
    maxRule = 255;
    tprogram = t;
    program = NULL;
    random.resize(t.size());
    haveBounds  = false;

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

void    QfcMethod::setBounds(vector<Interval> &g)
{
    haveBounds = true;
    boundGenome.resize(g.size());
    for(int i=0;i<(int)g.size();i++)
    {
        boundGenome[i]=g[i];
    }
}

void    QfcMethod::setMaxRule(int t)
{
    maxRule = t;
}

int     QfcMethod::getMaxRule() const
{
    return maxRule;
}

int     QfcMethod::getElement(int pos)
{
    if(!haveBounds)
    {
        return randomInt(0,maxRule);
    }
    else
    {
      //  printf("Pos = %d sizeof =%d \n",pos,boundGenome.size());
        return randomInt(boundGenome[pos].leftValue(),
                         boundGenome[pos].rightValue());
    }
}

bool QfcMethod::isParallel()
{
    return tprogram.size()!=0;
}

QfcMethod::~QfcMethod()
{

}

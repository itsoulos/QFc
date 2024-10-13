#include "qfcmethod.h"
# include <omp.h>
QfcMethod::QfcMethod(Program *t)
{
    program = t;
    tprogram.resize(0);
    random.resize(1);
    drandom.resize(1);
    maxRule = 255;
    haveBounds  = false;
    seedRandom(1);
}

QfcMethod::QfcMethod(vector<Program*> t)
{
    maxRule = 255;
    tprogram = t;
    program = NULL;
    random.resize(t.size());
    drandom.resize(t.size());
    haveBounds  = false; 
    seedRandom(1);
}


void    QfcMethod::seedRandom(int s)
{
	std::seed_seq seq{s+1,s+2,s+3,s+4,s+5};
    for(int i=0;i<random.size();i++)
        random[i].seed(seq);
}

double  QfcMethod::randomDouble()
{
//	return rand()*1.0/RAND_MAX;
    if(isParallel())
        return random[omp_get_thread_num()].generateDouble();
    else
        return random[0].generateDouble();
}

int     QfcMethod::randomInt(int low,int upper)
{
//	return low+(rand() % (upper-low));
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

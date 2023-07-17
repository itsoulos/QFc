#include "qfcmethod.h"

QfcMethod::QfcMethod(Program *t)
{
    program = t;
    tprogram.resize(0);
}

QfcMethod::QfcMethod(vector<Program*> t)
{
    tprogram = t;
    program = NULL;
}

void    QfcMethod::run()
{
    init();
    while(!terminated())
    {
        step();
        report();
    }
}

bool QfcMethod::isParallel()
{
    return tprogram.size()!=0;
}

QfcMethod::~QfcMethod()
{

}

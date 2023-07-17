# include <QfcRandom.h>
QRandomGenerator randomInt,randomDouble;
void     seedDouble(int s)
{
    randomDouble.seed(s);
}

void     seedInt(int s)
{
    randomInt.seed(s);
}

double   randDouble()
{
    return randomDouble.generateDouble();
}

int      randInt(int low,int upper)
{
    return randomInt.bounded(low,upper);
}

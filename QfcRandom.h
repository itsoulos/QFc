#ifndef QFCRANDOM_H
#define QFCRANDOM_H
# include <QRandomGenerator>
extern QRandomGenerator randomInt,randomDouble;
extern void     seedDouble(int s);
extern void     seedInt(int s);
extern double   randDouble();
extern int      randInt(int low,int upper);
#endif // QFCRANDOM_H

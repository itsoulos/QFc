#ifndef NNC_H
#define NNC_H
# include <model.h>
# include <nncneuralprogram.h>
# include <population.h>
class NNC :public Model
{
private:
    NeuralProgram *program;
    Population *pop;
public:
    NNC(Mapper *m);
    virtual	double 	train1();
    virtual double	train2();
    virtual double	output(Matrix &x);
    virtual void	getDeriv(Matrix &x,Matrix &g);
    virtual double	funmin(Matrix &x);
    virtual void    granal(Matrix &x,Matrix &g);

    virtual ~NNC();
};

#endif // NNC_H

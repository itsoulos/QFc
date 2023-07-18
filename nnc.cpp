#include "nnc.h"
# include <getoptions.h>
# include <converter.h>

NNC::NNC(Mapper *m)
    :Model(m)
{
    program = NULL;
    pop  = NULL;
}

double 	NNC::train1()
{

    vector<Matrix> ttx;
    Matrix tty;
    for(unsigned int i=0;i<xpoint.size();i++)
    {
        mapper->map(origx[i],xpoint[i]);

    }
    FILE *fp=fopen(testFile.toStdString().c_str(),"r");
    Matrix xx;
    xx.resize(origx[0].size());
    int d,count;
    fscanf(fp,"%d",&d);
    fscanf(fp,"%d",&count);
    ttx.resize(count);
    tty.resize(count);
    for(int i=0;i<count;i++)
    {
        ttx[i].resize(xpoint[0].size());
        for(int j=0;j<d;j++)
        {
            fscanf(fp,"%lf",&xx[j]);
        }
        mapper->map(xx,ttx[i]);
        fscanf(fp,"%lf",&tty[i]);
    }
    fclose(fp);
d=ttx[0].size();
    if(program==NULL)
    program=new NNCNeuralProgram (xpoint,ypoint,ttx,tty);
    pop=new Population (ge_chromosomes,ge_length ,program);
    pop->setSelectionRate(ge_selectionRate);
    pop->setMutationRate(ge_mutationRate);
    pop->setLocalSearchGenerations(1000);
    pop->setLocalSearchRate(0.00);
    double f;
    vector<int> genome;
    genome.resize(pop->getSize());
    string str="";
    double bestError = 1e+100;
    double old_test_error = 1e+100;
    Matrix bestWeights;
    for(int i=1;i<=ge_maxGenerations;i++)
    {
       pop->step();
       f=pop->getBestFitness();
       genome=pop->getBestGenome();
       str=program->printProgram(genome);
       program->fitness(genome);
       if(fabs(f)<bestError)
       {
            program->neuralparser->getWeights(bestWeights);
            bestError=fabs(f);
            old_test_error=program->getTestError();
        }
     //  printf("BEST[%d]=%20.10lf Solution: y(x)=%s\n",i,f,str.c_str());
       NNCNeuralProgram *p=(NNCNeuralProgram*)program;
       double class_test=p->getClassTestError(genome);
      // printf("CLASS  ERROR=%.2lf%%\n",class_test);

       //LOCALSEARCH
               if(i%20==0)
               {
                   int imax=20;//localSearchChromosomes;
                   int iflag=0;
                   for(int k=0;k<imax;k++)
                   {
                       vector<int> trial_genome;
                       trial_genome.resize(pop->getSize());
                       int trial_pos;
                       Data x;
       again:
                       trial_pos=randomInt(0,pop->getCount()-1);
                       iflag=1;

                       pop->getGenome(trial_pos,trial_genome);
                       program->fitness(trial_genome);
                       program->neuralparser->getWeights(x);
                       double value=0;
                       MinInfo Info1;
                       Info1.iters=2001;
                       Info1.p=this;
                       double old_f=1e+100;
                       int tries=0;
                       do
                       {
                           value=program->getTrainError();
                           if(value>=1e+8) {iflag=1;goto again;}
                           value=tolmin(x,Info1);
                           if(value>=1e+8) {iflag=1;goto again;}
                           program->neuralparser->getWeights(x);
                           if(fabs(old_f-value)<1e-5) break;
                           old_f=value;

                           fflush(stdout);
                           tries++;
                           if(tries>=20) break;
                           break;
                       }while(1);

                       program->neuralparser->getWeights(x);
                       value=program->getTrainError();

                       if((std::isnan(value) || std::isinf(value)))
                       {
                            if(!k) continue;
                           iflag=1;
                            goto again;
                       }


                       Converter con(x,x.size()/(d+2),d);
                       con.convert(trial_genome);
                       for(int i=0;i<trial_genome.size();i++)
                       {
                           //if(abs(trial_genome[i])>255) trial_genome[i]=0;
                       }
                       double trial_fitness=-value;
                       pop->setGenome(trial_pos,trial_genome,trial_fitness);

                       if(fabs(value)<=bestError)
                       {
                           bestWeights=x;
                           bestError=fabs(value);
                           old_test_error=program->getTestError();
                           pop->setBest(trial_genome,trial_fitness);
                           f=trial_fitness;
                       }
                       if(value<f)
                       {
                           bestWeights=x;
                           bestError=fabs(value);
                           old_test_error=program->getTestError();
                           pop->setBest(trial_genome,trial_fitness);
                           f=trial_fitness;
                       }
                   }
                   pop->select();
               }
       if(fabs(bestError)<1e-6) break;
    }
    program->neuralparser->setWeights(bestWeights);
    old_test_error=program->getTestError();
    str=program->printProgram(genome);
    printf("TRAIN ERROR =%.10lf\n",bestError);
    printf("TEST  ERROR =%.10lf\n",old_test_error);
    NNCNeuralProgram *p=(NNCNeuralProgram*)program;
    double class_test=p->getClassTestError(genome);
    printf("CLASS  ERROR=%.2lf%%\n",class_test);
    printf("SOLUTION: y(x)=%s\n",str.c_str());
    delete pop;
    return bestError;
}

double	NNC::train2()
{
    return train1();
}

double	NNC::output(Matrix x)
{
    return program->neuralparser->eval(x);
}

void	NNC::getDeriv(Matrix x,Matrix &g)
{
    program->neuralparser->setWeights(x);
    program->getDeriv(g);

}
void    NNC::granal(Matrix x,Matrix &g)
{
    program->neuralparser->setWeights(x);
    program->getDeriv(g);
}

double	NNC::funmin(Matrix x)
{
    program->neuralparser->setWeights(x);
    return program->getTrainError();
}

NNC::~NNC()
{
delete program;
}

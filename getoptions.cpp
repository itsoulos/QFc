# include <getoptions.h>
/** General parameters **/
QString  trainFile="";
QString  testFile="";
int      features=1;
int      randomSeed=1;
QString  featureCreatemodel="rbf";
QString  featureEvaluateModel="neural";
int      testIters=30;
int      threads=1;
/** Parameters for the neural network **/
int      neural_weights=1;
QString  neural_trainingMethod="bfgs";

/** Parameters for the KNN model **/
int     knn_weights=1;

/** Parameters for the RBF model **/
int     rbf_weights=1;

/** Parameters for the Grammatical Evolution algorithm **/
int      ge_chromosomes=500;
int      ge_maxGenerations=200;
double   ge_selectionRate=0.10;
double   ge_mutationRate=0.05;
int      ge_length = 40;
double   ge_localSearchRate=0.05;
QString  ge_localSearchMethod="crossover";//available options are: random, crossover, grs, bfgs
int      ge_localSearchGenerations=20;

/** Parameters for the genetic algorithm **/
int      genetic_chromosomes=500;
int      genetic_maxGenerations=500;
double   genetic_selectionRate=0.10;
double   genetic_mutationRate=0.05;
double   genetic_localSearchRate=0.00;
QString  genetic_localSearchMethod="bfgs";

/** Parameters for the bfgs method **/
int      bfgs_iterations=2001;

/** Parameters for the lbfgs method **/
int      lbfgs_iterations=2001;

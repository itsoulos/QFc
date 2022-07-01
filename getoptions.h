#ifndef GETOPTIONS_H
#define GETOPTIONS_H
# include <QString>
# include <unistd.h>
# include <string.h>
# include <stdio.h>
# include <stdlib.h>

/** General parameters **/
extern QString  trainFile;
extern QString  testFile;
extern int      features;
extern int      randomSeed;
extern QString  featureCreatemodel;
extern QString  featureEvaluateModel;
extern int      testIters;
extern int      threads;

/** Parameters for the neural network **/
extern int      neural_weights;
extern QString  neural_trainingMethod;

/** Parameters for the KNN model **/
extern  int     knn_weights;

/** Parameters for the RBF model **/
extern  int     rbf_weights;

/** Parameters for the Grammatical Evolution algorithm **/
extern int      ge_chromosomes;
extern int      ge_maxGenerations;
extern double   ge_selectionRate;
extern double   ge_mutationRate;
extern int      ge_length;
extern int      ge_localSearchGenerations;
extern double   ge_localSearchRate;
extern QString  ge_localSearchMethod;

/** Parameters for the genetic algorithm **/
extern int      genetic_chromosomes;
extern int      genetic_maxGenerations;
extern double   genetic_selectionRate;
extern double   genetic_mutationRate;
extern double   genetic_localSearchRate;
extern QString  genetic_localSearchMethod;

/** Parameters for the bfgs method **/
extern int      bfgs_iterations;

/** Parameters for the lbfgs method **/
extern int      lbfgs_iterations;
#endif // GETOPTIONS_H

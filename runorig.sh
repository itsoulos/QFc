#!/bin/sh
PROGRAM=./QFc
DATAPATH=~/Desktop/ERGASIES/FeatureConstruction2/datasets/tenfolding/
DATAFILE=$1
TRAINMODEL=copy #Available options are: rbf, neural, knn, armaRbf, osamaRbf, nnc
EVALMODEL=neural #Available options are: rbf, neural, knn, armaRbf, osamaRbf, nnc, weightNeural
LOCALMETHOD=none #Available options are: crossover, random, grs, bfgs
NNMETHOD=de #Available options are: bfgs, genetic, conj, leve, grs, de
WEIGHTS=10
 $PROGRAM --trainFile=$DATAPATH/$DATAFILE.train --testFile=$DATAPATH/$DATAFILE.test --featureCreateModel=$TRAINMODEL  --rbf_weights=$WEIGHTS --neural_weights=$WEIGHTS --knn_weights=3 --featureEvaluateModel=$EVALMODEL --ge_localSearchMethod=$LOCALMETHOD --ge_maxGenerations=200 --ge_localSearchGenerations=20 --neural_trainingMethod=$NNMETHOD --threads=1 --de_fvalue=0.8 --de_crvalue=0.9  --de_np=200 --de_maxiters=200 --de_fmethod=migrant --de_lrate=0.005 --de_usekmeans=no

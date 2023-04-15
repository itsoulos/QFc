#!/bin/sh
PROGRAM=./QFc
DATAPATH=~/Desktop/ERGASIES/FeatureConstruction2/datasets/tenfolding/
DATAFILE=$1
TRAINMODEL=copy #Available options are: rbf, neural, knn, armaRbf, osamaRbf, nnc
EVALMODEL=neural #Available options are: rbf, neural, knn, armaRbf, osamaRbf, nnc, weightNeural
LOCALMETHOD=random #Available options are: crossover, random, grs, bfgs
NNMETHOD=bfgs #Available options are: bfgs, genetic, conj, leve, grs
WEIGHTS=10
$PROGRAM --trainFile=$DATAPATH/$DATAFILE.train --testFile=$DATAPATH/$DATAFILE.test --featureCreateModel=$TRAINMODEL  --rbf_weights=$WEIGHTS --neural_weights=$WEIGHTS --knn_weights=3 --featureEvaluateModel=$EVALMODEL --ge_localSearchMethod=$LOCALMETHOD --ge_maxGenerations=200 --ge_localSearchGenerations=20 --neural_trainingMethod=$NNMETHOD

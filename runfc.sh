#!/bin/sh
PROGRAM=./QFc
DATAPATH=~/Desktop/ERGASIES/FeatureConstruction2/datasets/tenfolding/
DATAFILE=$1
FEATURES=$2
SEED=$3
TRAINMODEL=rbf #Available options are: rbf, neural, knn, armaRbf, osamaRbf, nnc
EVALMODEL=nnc #Available options are: rbf, neural, knn, armaRbf, osamaRbf, nnc, weightNeural (for weight decay)
LOCALMETHOD=random #Available options are: crossover, random, grs, bfgs
NNMETHOD=genetic #Available options are: bfgs, genetic, conj, leve, grs
THREADS=1
WEIGHTS=10
$PROGRAM --trainFile=$DATAPATH/$DATAFILE.train --testFile=$DATAPATH/$DATAFILE.test --featureCreateModel=$TRAINMODEL --features=$FEATURES  --rbf_weights=$WEIGHTS --neural_weights=$WEIGHTS --knn_weights=3 --featureEvaluateModel=$EVALMODEL --randomSeed=$SEED --ge_localSearchMethod=$LOCALMETHOD --ge_maxGenerations=200 --ge_localSearchGenerations=20 --neural_trainingMethod=$NNMETHOD --threads=$THREADS

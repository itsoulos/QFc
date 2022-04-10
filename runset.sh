#!/bin/sh
PROGRAM=./QFc
DATAPATH=~/Desktop/ERGASIES/FeatureConstruction2/datasets/tenfolding/
DATAFILE=$1
FEATURES=$2
TRAINMODEL=rbf #Available options are: rbf, neural, knn, armaRbf, osamaRbf, nnc
EVALMODEL=neural #Available options are: rbf, neural, knn, armaRbf, osamaRbf, nnc
LOCALMETHOD=random #Available options are: crossover, random, grs, bfgs
NNMETHOD=genetic #Available options are: bfgs, genetic, conj, leve, grs
WEIGHTS=10

rm -f $DATAFILE.out
for i in $(seq 1 10); 
do 
	SEED=$i
$PROGRAM --trainFile=$DATAPATH/$DATAFILE.train --testFile=$DATAPATH/$DATAFILE.test --featureCreateModel=$TRAINMODEL --features=$FEATURES  --rbf_weights=$WEIGHTS --neural_weights=$WEIGHTS --knn_weights=3 --featureEvaluateModel=$EVALMODEL --randomSeed=$SEED --ge_localSearchMethod=$LOCALMETHOD --ge_maxGenerations=200 --ge_localSearchGenerations=20 --ge_chromosomes=50 --neural_trainingMethod=$NNMETHOD >> $DATAFILE.out
done

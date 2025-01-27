#!/bin/sh
PROGRAM=./QFc
DATAPATH=~/Desktop/ERGASIES/FeatureConstruction2/datasets/tenfolding/
DATAFILE=$1
FEATURES=$2
GEMETHOD=genetic #Available options are: genetic,pso,igenetic
SEED=$3
TRAINMODEL=rbf #Available options are: rbf, neural, knn, armaRbf, osamaRbf, nnc
EVALMODEL=rbf #Available options are: rbf, neural, knn,  osamaRbf, nnc
LOCALMETHOD=random #Available options are: crossover, random, grs, bfgs,none, siman
NNMETHOD=bfgs #Available options are: bfgs, genetic, lbfgs
THREADS=1
WEIGHTS=20
$PROGRAM --trainFile=$DATAPATH/$DATAFILE.train --testFile=$DATAPATH/$DATAFILE.test --featureCreateModel=$TRAINMODEL --features=$FEATURES  --rbf_weights=$WEIGHTS --neural_weights=$WEIGHTS --knn_weights=3 --featureEvaluateModel=$EVALMODEL --randomSeed=$SEED --ge_localSearchMethod=$LOCALMETHOD --ge_maxGenerations=200 --ge_localSearchGenerations=20 --neural_trainingMethod=$NNMETHOD --threads=$THREADS --export_train_file=test1.train --export_test_file=test1.test --export_cpp_file=test1.cpp --ge_method=$GEMETHOD --de_lrate=0.005 --de_fmethod=random --de_usekmeans=yes

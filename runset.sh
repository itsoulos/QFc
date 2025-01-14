#!/bin/sh
PROGRAM=./QFc
DATAPATH=~/Desktop/ERGASIES/FeatureConstruction2/datasets/tenfolding/
DATAFILE=$1
FEATURES=$2
TRAINMODEL=rbf #Available options are: rbf, neural, knn, armaRbf, osamaRbf, nnc
EVALMODEL=rbf #Available options are: rbf, neural, knn, armaRbf, osamaRbf, nnc
LOCALMETHOD=none #Available options are: none,crossover, random, grs, bfgs, siman
NNMETHOD=bfgs #Available options are: bfgs, genetic, conj, leve, grs
WEIGHTS=10
GENERATIONS=200

rm -f $DATAFILE.out
for i in $(seq 1 10) 
do 
	SEED=$i
$PROGRAM --trainFile=$DATAPATH/$DATAFILE.train --testFile=$DATAPATH/$DATAFILE.test --featureCreateModel=$TRAINMODEL --features=$FEATURES  --rbf_weights=$WEIGHTS --neural_weights=$WEIGHTS --knn_weights=3 --featureEvaluateModel=$EVALMODEL --randomSeed=$SEED --ge_localSearchMethod=$LOCALMETHOD --ge_maxGenerations=$GENERATIONS --ge_localSearchGenerations=20 --ge_chromosomes=200 --neural_trainingMethod=$NNMETHOD >> $DATAFILE.out
done

grep train $DATAFILE.out > $DATAFILE.out.xx
echo -n "TRAIN ERROR: "
cat $DATAFILE.out.xx | awk '{ sum += $2 } END { if (NR > 0) print sum / NR }'
echo -n "TEST ERROR: "
cat $DATAFILE.out.xx | awk '{ sum += $4 } END { if (NR > 0) print sum / NR }'
echo -n "CLASS ERROR: "
cat $DATAFILE.out.xx | awk '{ sum += $6 } END { if (NR > 0) print sum / NR }'

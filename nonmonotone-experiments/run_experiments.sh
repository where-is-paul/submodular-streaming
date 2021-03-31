#!/bin/bash

cd cpp;
make;
for dataset in yahoo bing gowalla
	do
	./main -f ../data/$dataset.prob | tee ../logs/experiments-$dataset.log;
done

cd ..;
python plot-data.py
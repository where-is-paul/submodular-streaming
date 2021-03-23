#!/bin/bash

for f in ./steiner-triple-covering/data.*
do
	echo $f;
	./setcover -f $f -k 20
	echo "=================================================";
done

for f in ./freq-items/*.dat
do
	echo $f;
	./setcover -g $f -k 20
	echo "=================================================";
done

python plot-data.py
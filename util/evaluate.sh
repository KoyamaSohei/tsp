#!/bin/bash

a=("ni" "nn" "fi" "greedy")
b=("2opt" "sa" "tabu" "combopt")
c=("ulysses22" "a280")

for k in "${c[@]}"
do
  eval "gzip -d lib/${k}.tsp.gz"
done

for i in "${a[@]}"
do
  for j in "${b[@]}"
  do
    echo "build ${i}_${j}.out"
    eval "g++ -O2 -I/usr/include/X11/ -L/usr/lib/ -lX11 -lm util/tspShow.c -lm build/${i}.cc improve/${j}.cc bootstrap.cc heuristics.cc -o ${i}_${j}.out"
    for k in "${c[@]}"
    do
      echo "build: ${i} improve: ${j} target: ${k}"
      eval "time ./${i}_${j}.out -n -c lib/${k}.tsp | ./export.out > dist/${k}.${i}_${j}.svg"
      sleep 3
    done
  done
done
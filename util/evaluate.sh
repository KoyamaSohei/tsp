#!/bin/bash

a=("ni" "nn" "fi" "greedy")
b=("2opt" "sa" "tabu" "combopt")
c=("a280" "usa13509")

for i in "${a[@]}"
do
  for j in "${b[@]}"
  do
    for k in "${c[@]}"
    do
      echo "build: ${i} improve: ${j} target: ${k}"
      eval "time ./${i}_${j}.out -n -c lib/${k}.tsp | ./export.out > dist/${k}.${i}_${j}.svg"
    done
  done
done